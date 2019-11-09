#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dirent.h> 
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <time.h>


char **names=NULL;
DIR *dir;
int cnt, columnsCount[41], widths[41], blocks=1;

int cmp(void *a, void *b) {
    return strcmp(*(char **)a,*(char **)b);
}

int min(int a, int b){
    return a<b?a:b;
}

int max(int a, int b) {
    return a>b?a:b;
}

int detectColumn(int maxColumn) {//看最大放几行
    memset(widths, 0, sizeof(widths));
    int maxRow=cnt/maxColumn+(cnt%maxColumn!=0);
    int totalLen=0;
    for(int i=0; i<cnt; i++)
        widths[i/maxRow] = max(widths[i/maxRow], strlen(names[i]));
    for(int i=0; i<maxColumn; i++)
        totalLen += widths[i];
    if(totalLen+(maxColumn-1)*2>80) return 0;
    return 1;
}

void printAll(int maxColumn) { //打印ls
    memset(widths, 0, sizeof(widths));
    int maxRow=cnt/maxColumn+(cnt%maxColumn!=0);
    for(int i=0; i<cnt; i++)
        widths[i/maxRow] = max(widths[i/maxRow], strlen(names[i]));
    for(int i=0; i<maxRow; i++) {
        for(int j=0; j<maxColumn; j++)
            if(i+j*maxRow<cnt) {
                printf("%s", names[i+j*maxRow]);
                
                if(j==maxColumn-1)  ;
                else    printf("%*c", widths[j]-strlen(names[i+j*maxRow])+2, ' ');
            }
        printf("\n");
    }
}

void ls() {
    int maxColumn=0, i;
    for(i=min(40, cnt); i>=1; i--)
        if(detectColumn(i)) {
            maxColumn = i;
            break;
        }
    if(i<1) maxColumn = 1;
    printAll(maxColumn);
}

void ls_l() {
    struct stat stats;
    int maxlen=0;
    for(int i=0; i<cnt; i++) {
        stat(names[i], &stats);
        int j=0, k=stats.st_size;
        while(k) {
            j++;
            k /= 10;
        }
        maxlen = max(maxlen, j);
    }
    for(int i=0; i<cnt; i++) {
        stat(names[i], &stats);
        if(S_ISBLK(stats.st_mode))    putchar('b');
        else if(S_ISCHR(stats.st_mode))   putchar('c');
        else if(S_ISDIR(stats.st_mode))   putchar('d');
        else if(S_ISFIFO(stats.st_mode))   putchar('f');
        else if(S_ISLNK(stats.st_mode))   putchar('1');
        else if(S_ISREG(stats.st_mode))   putchar('-');
        else if(S_ISSOCK(stats.st_mode))   putchar('s');

        char s[10]="---------";
        if(stats.st_mode & S_IRUSR) s[0] = 'r';
        if(stats.st_mode & S_IWUSR) s[1] = 'w';
        if(stats.st_mode & S_IXUSR) s[2] = 'x';
        if(stats.st_mode & S_IRGRP) s[3] = 'r';
        if(stats.st_mode & S_IWGRP) s[4] = 'w';
        if(stats.st_mode & S_IXGRP) s[5] = 'x';
        if(stats.st_mode & S_IROTH) s[6] = 'r';
        if(stats.st_mode & S_IWOTH) s[7] = 'w';
        if(stats.st_mode & S_IXOTH) s[8] = 'x';
        printf("%s %d ", s, stats.st_nlink);

        struct passwd *usr;
        struct group *grp;
        usr = getpwuid(stats.st_uid);
        grp = getgrgid(stats.st_gid);
        printf("%s %s", usr->pw_name, grp->gr_name);

        printf("%*d", maxlen+1, stats.st_size);//注意大小！

        struct tm *localTime=localtime(&stats.st_mtime);
        printf(" %2d月%3d%3d:%02d", localTime->tm_mon+1, localTime->tm_mday, localTime->tm_hour, localTime->tm_sec);
        
        printf(" %s\n", names[i]);
    }
}

int main(int argc, char **argv) {
    dir = opendir(".");
    struct dirent *files;
    while((files=readdir(dir))!=NULL) {
        if(files->d_name[0]!='.') {
            if(cnt==blocks-1) {
                blocks <<= 1;
                names = (char **)realloc(names, sizeof(char *)*blocks);
            }
            names[cnt++] = files->d_name;
        }
    }
    qsort(names, cnt, sizeof(char *), cmp);
    if(argc==2 && strcmp(argv[1], "-l")==0) ls_l();
    else ls();
    free(names);
    return 0;
}