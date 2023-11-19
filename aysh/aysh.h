#define BUFSIZE 2048
#define MAXTOKS 1024
#define HISTORY 1024
#include <setjmp.h>

void close_fd(int * fd);
void runcmd(char * cmd[],int * redir,int toclose[][2],int ccnt);
int tokenize(char * s,char * tok[],char* sep);
void run(char * toks[],int * redir,int toclose[][2],int ccnt);
int runcd(char * dir);
void parseLine(char* buf,char* toks[],int toclose[][2],int ccnt);
char * cleanStr(char* str);
void pipeline(char* buf,char* toks[] );
void handle_signals();
char * findRedir(char* buf);
void redirection(char* buf);
extern int fd[2];
extern jmp_buf env;