#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if(argc!=3) {
        printf("cp arguments error");
        return 0;
    }
    int oriIn=dup(0);
    int oriOut=dup(2);
    if(freopen(argv[1], "r", stdin)==NULL)
        printf("No %s\n", argv[1]);
    freopen(argv[2], "w", stdout);
    char ch;
    while((ch=getchar())!=EOF)  putchar(ch);
    dup2(oriIn, 0);
    dup2(oriOut, 1);
    return 0;
}