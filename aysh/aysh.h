#define BUFSIZE 2048
#define MAXTOKS 1024
#define HISTORY 1024

void close_fd(int * fd);
void runcmd(char * cmd[],int * redir);
int tokenize(char * s,char * tok[],char* sep);
void run(char * toks[],int * redir);
int runcd(char * dir);
void parseLine(char* buf,char* toks[]);
char * cleanStr(char* str);
void piping(char* buf,char* toks[]);
void handle_signals();
char * findRedir(char* buf);
void redirection(char* buf);
