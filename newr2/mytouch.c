#include<fcntl.h>
#include<sys/stat.h>
#include<stdio.h>
#include<unistd.h>
#include<time.h>


#define NEW_FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

static struct timespec newtime[2];

void mytouch(char *file) {
    open(file, O_CREAT | O_WRONLY, NEW_FILE_MODE);
    utimensat(AT_FDCWD,file,newtime,0);
}

int main(int argc, char **argv) {
    newtime[0].tv_nsec = UTIME_NOW;
    newtime[1].tv_nsec = UTIME_NOW;
    for(int i=1; i<argc; i++)
        mytouch(argv[i]);
    return 0;
}