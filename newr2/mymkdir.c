#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    for(int i=1; i<argc; i++) {
        if(mkdir(argv[i], 0755))
            printf("Error in mkdir %s\n", argv[i]);
    }
    return 0;
}