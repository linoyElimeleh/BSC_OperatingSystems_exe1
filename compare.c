/*Linoy Elimeleh 319122619*/
/*Mor Revivo 318572716*/

#include <stdio.h>
#include <sys/fcntl.h>
#include <errno.h>

int main (int argc, char **argv){
        int fdl = open(argv[1], O_RDONLY);
        int fd2 = open(argv[2], O_RDONLY);
        char buf1[1];
        char buf2[1];
        int count1 = read(fdl, buf1 , 1);
        int count2 = read(fd2, buf2 , 1);;
        if ( fdl==-1 || fd2==-1) { exit(0); };
        while (count1==1 && count2==1)
        {
                if ( count1 == -1 || count2 == -1) { exit(0); };
                if (buf1[0] != buf2[0]) { exit(1); };
                count1 = read(fdl, buf1 , 1);
                count2 = read(fd2, buf2 , 1);
        }
        if (count1==0 && count2==0) {exit(2);};
        if (count1==1 || count2==1) {exit(1);};
}