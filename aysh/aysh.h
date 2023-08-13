#define BUFSIZE 2048
#define MAXTOKS 1024
#define HISTORY 1024

void runcmd(char * cmd[],int redir,int dir);
int tokenize(char * s,char * tok[],char* sep);
void run(char * toks[],int redir,int dir);
int runcd(char * dir);
void parseLine(char* buf,char* toks[]);
char * cleanStr(char* str);
