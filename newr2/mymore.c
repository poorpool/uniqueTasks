#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

int height, width, fd;
struct termios oriTs;


void termInit() {//获得终端长宽，设置字符回显
    struct winsize *ws=(struct winsize *)malloc(sizeof(struct winsize));
    struct termios ts;
    tcgetattr(0, &oriTs);
    memset(ws, 0, sizeof(struct winsize));
    ioctl(0, TIOCGWINSZ, ws);
    height = ws->ws_row;
    width = ws->ws_col;
    tcgetattr(0, &ts);
    ts.c_lflag &= ~ECHO;
    ts.c_lflag &= ~ICANON;
    ts.c_cc[VMIN] = 1;
    ts.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &ts);
}

void termBack() {
    tcsetattr(0, TCSAFLUSH, &oriTs);
}

void seeMore() {
    char buf[64];
    for(int i=1; i<height; i++) {
        int isLineEnd=0;
        for(int j=1; j<=width; j++) {
            if(isLineEnd) write(STDOUT_FILENO, " ", 1);
            else {
                int rd=read(fd, buf, 1);
                if(rd==0) {
                    close(fd);
                    termBack();
                    exit(0);
                }
                if(buf[0]=='\n')    isLineEnd = 1;
                else {
                    write(STDOUT_FILENO, buf, 1);
                }
            }
            
        }
        printf("\n");
    }
    printf("\033[7mmore?>\033[m");
}

void moveOffset() {
    int lineNumber=0;
    lseek(fd, -1, SEEK_CUR);
    char buf[64];
    while(lineNumber<height) {
        
        read(fd, buf, 1);
        if(buf[0]=='\n') {
            lineNumber++;
            if(lineNumber==height)    break;
        }
        if(lseek(fd, -2, SEEK_CUR)<0) {
            lseek(fd, -1, SEEK_CUR);
            break;
        }
    }
}

void seeLess() {
    moveOffset();
    moveOffset();
    seeMore();
}

int main(int argc, char **argv) {
    termInit();
    fd = open(argv[1], O_RDONLY);
    if(fd<0) {
        printf("ERROR in %s\n", argv[1]);
        return 0;
    }
    seeMore();
    char ch;
    while((ch=getc(stdin))!=EOF) {
        if(ch=='q')   break;
        else if(ch==' ')    seeMore();
        else if(ch=='b')    seeLess();
    }
    close(fd);
    termBack();
    return 0;
}
