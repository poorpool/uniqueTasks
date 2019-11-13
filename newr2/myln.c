#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    if(strcmp(argv[1], "-s")==0) {
        if(argc!=4) {
            printf("arguments number error\n");
            return 0;
        }
        if(symlink(argv[2], argv[3]))
            printf("Error in creating symlink\n");
    }
    else {
        if(argc!=3) {
            printf("arguments number error\n");
            return 0;
        }
        if(link(argv[1], argv[2]))
            printf("Error in creating link\n");
    }
    return 0;
}