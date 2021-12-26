
/*Linoy Elimeleh 319122619*/
/*Mor Revivo 318572716*/

/* define libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/wait.h>

#define COMPERATOR_APP "./compare.exe"
#define SIZE 255
#define DEBUG_SIGNAL 1

/* define configuration struct */
struct config {
	char students_dir[SIZE];
	char input[SIZE];
	char output[SIZE];

};

struct config creatConfigFile(char* args);
void executeComm(char** params, char* names);
char** readFromFile(char* name,int numUsers);
int Comparison(char** params);


int main(int argc, char* argv[]){
	/* define configuration */
	struct config conf;
	FILE * listFile;
	FILE * inputFile;
	FILE * csvFile;
	char studentName[SIZE];
	/* check if we get all the arguments we need */
	if (argc < 2)
	{
		perror("Error arguments required\n");
		return -1;
	}

	/* create the config file*/
	printf("before create config\n");
	conf = creatConfigFile(argv[1]);

	/* open the end csv file */
	printf("before open result file\n");
	csvFile = fopen("result.csv", "w");

	/* check if there is an error in the scv file */
	if (csvFile==NULL)
	{
		fclose(csvFile);
		perror("Error in create file\n");
		exit(-1);
	}
	/* execute ls command at student dir */
	printf("before excute command ls\n");
	executeComm((char *[]) {"/bin/ls", conf.students_dir, NULL } , "users.txt");

	/* check users.txt file if null, close everything */
	if((listFile = fopen("users.txt", "r"))==NULL){
		fclose(listFile);
		fclose(csvFile);
	};

	/* open  csv for overwrite */
	bzero(&studentName,SIZE*sizeof(char));

	/* until there is no children */
	while (fgets(studentName, SIZE, listFile) != NULL)
	{
		char numberA[256];
		char numberB[256];
		/* initialize studentName buffer */
		studentName[strlen(studentName) - 1 ] = '\0';
		char dirUsers[SIZE];
		char mainUsers[SIZE];
		char resultUsers[SIZE];

		bzero(&dirUsers,SIZE*sizeof(char));
		bzero(&mainUsers,SIZE*sizeof(char));
		bzero(&resultUsers,SIZE*sizeof(char));

		/* create the path */
		strcat(dirUsers,conf.students_dir);
		strcat(dirUsers,"/");
		strcat(dirUsers,studentName);
		strcat(dirUsers,"/");

		strcat(mainUsers,dirUsers);
		strcat(mainUsers,"main.exe");

		strcat(resultUsers,dirUsers);
		strcat(resultUsers,"output.txt");
		if((inputFile = fopen(conf.input, "r"))==NULL){
                	fclose(inputFile);
        	        fclose(csvFile);
	        };

		fgets(numberA, sizeof(numberA), inputFile);
		fgets(numberB, sizeof(numberB), inputFile);

		if(DEBUG_SIGNAL == 1) {
			printf("studentName : %s\n",studentName);
			printf("dirUsers : %s\n",dirUsers);
			printf("mainUsers : %s\n",mainUsers);
			printf("resultUsers : %s\n",resultUsers);
			printf("numberA : %s\n", numberA);
			printf("numberB : %s\n", numberB);
		}
		executeComm((char *[]) {mainUsers, numberA, numberB, NULL }, resultUsers);

		/* comparison */
		int gradeUsers = Comparison((char *[]) { COMPERATOR_APP, resultUsers, conf.output, NULL });
		fprintf(csvFile, "%s, %d\n", studentName, gradeUsers);
		bzero(&studentName, SIZE*sizeof(char));

	}
	fclose(listFile);
	fclose(csvFile);
	return 0;
}



struct config creatConfigFile(char * args)
{
	struct config conf;

	/* open the file with read permissions */
	FILE * configID = fopen(args, "r");

	/* if the file is empty, end the program */
	if (configID == NULL) {
		perror("can't open config file\n");
		fclose(configID);
		exit(-1);
	}

	printf("load configuration\n");
	/* initialize the config struct */
	fscanf(configID,"%s %s %s",conf.students_dir, conf.input, conf.output);
	fclose(configID);
	return conf;
}


void executeComm(char ** params, char* names)
{
	/* create new proccess by fork action */
	int exe;
	int pid = fork();

	/* if the parrent proccess is empty returns failed*/
	if (pid<0) {
		perror("fork failed\n");
		exit(-1);
	}

	/* check the children */
	if (pid == 0) {

		/* output redirection */
		int FD = open(names, O_CREAT | O_WRONLY, S_IRWXU);
		close(1);
		dup(FD);
		close(FD);

		/* try to execute the file */
		exe = execvp(params[0], params);
		/* if the execute is failed exit the exe */
		if (exe<0) {
			perror("execvp failed\n");
			exit(-1);
		}
	}
	else {
		/* ask for the parent - pid>0 - wait */
		wait(NULL);
	}
}


char ** readFromFile(char * name, int numUsers)
{
	char b;
	int FD, readFD=0, j = 0, k=0;

	/* pointer allocation */
	char ** file = (char**)malloc(numUsers * sizeof(char*));
	for (int i = 0; i < numUsers; i++) {
		 file[i] = (char*)malloc(SIZE* sizeof(char));
	}

	/* check if we can open the file as readonly */
	FD = open(name, O_RDONLY);
	if (FD<0) {
		perror("can't open users.txt\n");
	}

	/* check id we can read from the file */
	readFD = read(FD, &b, 1);
	if (readFD < 0) {
		perror("can't read from users.txt\n");
	}

	/* read from the file line after line */
	while (readFD != 0 || j<=numUsers) {

		if (b == '\n') {
			file[j][k] ='\0';
			k = 0;
			j++;
		}
		else {
			file[j][k] = b;
			k++;
		}
		readFD = read(FD, &b, 1);
		if (readFD < 0) {
			perror("can't read from users.txt again\n");
		}
	}
	close(FD);
	return file;
}


int Comparison(char ** params)
{
	/* create new proccess by fork action */
	int exe, stat, ex1;
	int pid = fork();

	/* if the parrent proccess is empty returns failed*/
	if (pid<0) {
		perror("fork failed\n");
	}

	/* check the children */
	if (pid == 0) {
		exe = execvp(params[0], params);
		if (exe<0)
			perror("can't execvp\n");
		close(1);
	}

	/* check the parent- wait until the children end */
	if (pid>0) {
		wait(&stat);
		ex1 = WEXITSTATUS(stat);
	}

	if (ex1 == 1)
		return 0;
	else
		return 100;
}
