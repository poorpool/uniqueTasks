#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    if(argc!=3) {
        printf("cp arguments error");
        return 0;
    }
    int oriIn=dup(STDIN_FILENO);
    int oriOut=dup(STDOUT_FILENO);
    int fd=open(argv[1], O_RDONLY);
    if(fd==-1) {
        printf("No %s\n", argv[1]);
        return 0;
    }
    fflush(stdin);
    fflush(stdout);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    dup2(fd, STDIN_FILENO);
    int ffd=open(argv[2], O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    dup2(ffd, STDOUT_FILENO);
    char ch;
    while((ch=getchar())!=EOF)  putchar(ch);
    remove(argv[1]);
    fflush(stdin);
    fflush(stdout);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    dup2(oriIn, STDIN_FILENO);
    dup2(oriOut, STDOUT_FILENO);
    return 0;
}