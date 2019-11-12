#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <assert.h>
#include <readline/readline.h>

int argc, dividedArgc, mainPid;
char *argv[64], *dividedArgv[64], *finalOutput;
int originalInFd;
int originalOutFd;

void sigintHandler() {
    if(getpid()!=mainPid)   exit(0);
    else    ;
    //todo: ^C在主进程不是很友好
}

void shellExit() {
    exit(0);
}

void shellPwd() {
    char buf[1024];
    if(getcwd(buf, 1024)==NULL)
        printf("ERROR get pwd\n");
    printf("%s\n", buf);
}

void shellEcho() {
    for(int i=1; i<argc; i++)
        printf("%s ", argv[i]);
    printf("\n");
}

void shellCd() {
    int val=chdir(argv[1]);
    if(val==-1) printf("Error in cd.\n");
}

void shellKill() {
    for(int i=1; i<argc; i++) {
        int pid=atoi(argv[i]);
        if(pid!=0)  kill(pid, SIGTERM);
    }
}

void shellExport() {//simple version
    freopen("paths", "a", stdout);
    for(int i=1; i<argc; i++)   printf("%s\n", argv[i]);
}
void executeCommand() {
    if(argc<1)  return ;
    char buf[1024];
    int flg=0;
    if(strcmp(argv[argc-1], "&")==0) {
        flg = 1;
        argv[argc--] = NULL;
    }
    if(strcmp(argv[0], "exit")==0)  shellExit();
    else if(strcmp(argv[0], "pwd")==0) shellPwd();
    else if(strcmp(argv[0], "echo")==0) shellEcho();
    else if(strcmp(argv[0], "cd")==0)   shellCd();
    else if(strcmp(argv[0], "kill")==0) shellKill();
    else if(strcmp(argv[0], "export")==0) shellExport();
    else {
        FILE *fl=fopen("paths", "r");
        while(fgets(buf, 1024, fl)!=NULL) {
            if(buf[strlen(buf)-1]=='\n')    buf[strlen(buf)-1] = '\0';            
            if(buf[strlen(buf)-1]!='/') {
                buf[strlen(buf)+1] = '\0';
                buf[strlen(buf)] = '/';
            }
            strcat(buf, argv[0]);
            if(access(buf, 0)==0) 
                break;
        }
        fclose(fl);
        pid_t pid=fork();
        if(pid<0) {
            printf("Error in fork.\n");
            exit(0);
        }
        else if(pid==0) {
            if(flg) {
                freopen(".__tempfile", "w", stdout);
            }
            if(execv(buf, argv)<0) {
                printf("Error in %s, maybe there is no %s\n", argv[0], argv[0]);
                exit(0);
            }
            exit(0);
        }
        else
            if(!flg)    waitpid(pid, NULL, WUNTRACED);
    }
}

void dividePipes() {
    
    int isFirst=1, lst=0, xargsCnt=0;
    char buf[64][64];
    for(int i=0; i<dividedArgc; i++) 
        if(strcmp(dividedArgv[i], "|")==0) {
            xargsCnt = 0;
            memset(argv, 0, sizeof(argv));
            memset(buf, 0, sizeof(buf));
            argc = 0;
            if(!isFirst)
                freopen("__tempfile1", "r", stdin);
            int qwqwq=open("__tempfile2", O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            close(1);
            dup2(qwqwq, 1);
            for(; lst<i; lst++)
                argv[argc++] = dividedArgv[lst];
            if(strcmp(argv[0], "xargs")==0) {
                for(int j=0; j<argc-1; j++) {
                    argv[j] = argv[j+1];
                }
                argc--;
                // printf("%d\n")
                
                if(strcmp(argv[0], "-d")==0) {
                    char div=argv[1][1], ch;//todo: 无法处理转意字符
                    for(int j=0; j<argc-2; j++) {
                        argv[j] = argv[j+2];
                    }
                    argc -= 2;
                    if(argc==0) argv[argc++] = "echo";
                    int xargsLen=0;
                    while((ch=getchar())!=EOF) {
                        if(ch==div) {
                            argv[argc++] = buf[xargsCnt++];
                            xargsLen = 0;
                        }
                        else {
                            buf[xargsCnt][xargsLen++] = ch;
                        }
                    }
                    if(xargsLen)    argv[argc++] = buf[xargsCnt++];
                }
                else {
                    if(argc==0) argv[argc++] = "echo";
                    while(1) {
                        if(scanf("%s", buf[xargsCnt++])==EOF)   break;
                        argv[argc++] = buf[xargsCnt-1];
                    }
                }
                if(argc==0) argv[argc++] = "echo";
            }
            lst = i + 1;
            executeCommand();
            // system("mv __tempfile2 __tempfile1");
            argc = 3;
            argv[0] = "mv"; argv[1] = "__tempfile2"; argv[2] = "__tempfile1";
            executeCommand();
            isFirst = 0;
        }
    
    xargsCnt = 0;
    memset(argv, 0, sizeof(argv));
    argc = 0;
    if(!isFirst)
        freopen("__tempfile1", "r", stdin);
    if(finalOutput!=NULL)   freopen(finalOutput, "w", stdout);
    else
        dup2(originalOutFd, 1);
    for(; lst<dividedArgc; lst++)
        argv[argc++] = dividedArgv[lst];
    
    if(strcmp(argv[0], "xargs")==0) {
        for(int j=0; j<argc-1; j++) {
            argv[j] = argv[j+1];
        }
        argc--;
        printf("%d as argc\n", argc);
        if(strcmp(argv[0], "-d")==0) {
            char div=argv[1][1], ch;//todo: 无法处理转意字符
            int xargsLen=0;
            for(int j=0; j<argc-2; j++) {
                argv[j] = argv[j+2];
            }
            argc -= 2;
            if(argc==0) argv[argc++] = "echo";
            while((ch=getchar())!=EOF) {
                if(ch==div) {
                    argv[argc++] = buf[xargsCnt++];
                    xargsLen = 0;
                }
                else {
                    buf[xargsCnt][xargsLen++] = ch;
                }
            }
            if(xargsLen)    argv[argc++] = buf[xargsCnt++];
        }
        else {
            if(argc==0) argv[argc++] = "echo";
            while(1) {
                if(scanf("%s", buf[xargsCnt++])==EOF)   break;
                argv[argc++] = buf[xargsCnt-1];
            }
        }
        
        
    }
    lst++;
    executeCommand();
}

void divideCommands(char *buf) {
    dividedArgc = 0;
    memset(dividedArgv, 0, sizeof(dividedArgv));
    char *begin=buf, *end=buf;
    while(1) {
        while(*begin==' ' || *begin=='\t')    begin++;
        if(*begin=='\0') break;
        end = begin;
        while(*end!=' ' && *end!='\t' && *end!='\0')   end++;
        if(*end=='\0') {
            dividedArgv[dividedArgc++] = begin;
            return ;
        }
        *end = '\0';
        dividedArgv[dividedArgc++] = begin;
        begin = end + 1;
    }
}

void divideRedirects() {
    char *tmpArgv[16];
    int tmpCnt=0;
    finalOutput = NULL;
    for(int i=0; i<dividedArgc; i++) {
        if(strcmp(dividedArgv[i], "<")==0) {
            FILE *fl=freopen(dividedArgv[i+1], "r", stdin);
            if(fl==NULL)    printf("Error in redirect %s\n", dividedArgv[i+1]);
            i++;
        }
        else if(strcmp(dividedArgv[i], ">")==0) {
            finalOutput = dividedArgv[i+1];
            i++;
        }
        else
            tmpArgv[tmpCnt++] = dividedArgv[i];
    }
    memset(dividedArgv, 0, sizeof(dividedArgv));
    dividedArgc = tmpCnt;
    for(int i=0; i<tmpCnt; i++)
        dividedArgv[i] = tmpArgv[i];
    
}


int main() {
    signal(SIGINT, sigintHandler);
    mainPid = getpid();
    while(1) {
        char *buf=readline("\e[35;1mpoorpool @ localhost $ \e[0m");
        if(buf==NULL) break;
        divideCommands(buf);
        if(dividedArgc==0)  continue;
        originalInFd=dup(0);
        originalOutFd=dup(1);
        divideRedirects();
        dividePipes();
        fflush(stdout);
        fflush(stdin);
        close(0);
        close(1);
        dup2(originalInFd, 0);
        dup2(originalOutFd, 1);
        free(buf);
    }
    return 0;
}