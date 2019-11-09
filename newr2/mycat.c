#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int oriIn=dup(0);
    if(argc>=2) {
        int g=dup(0);
        for(int i=1; i<argc; i++) {
            if((g=open(argv[i], O_RDONLY))==-1)
                printf("Error: no such file %s\n", argv[i]);
            else {
                dup2(g, 0);
                char ch;
                while((ch=getchar())!=EOF)  putchar(ch);
            }
            fflush(stdin);
            fflush(stdout);
            close(0);
        }
    }
    else {
        char ch;
        while((ch=getchar())!=EOF)  putchar(ch);
    }
    dup2(oriIn, 0);
    return 0;
}