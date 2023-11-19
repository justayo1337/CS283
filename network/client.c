//ayomide adetunji

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <fcntl.h>
#define MSGSIZE 4096
//gethostbyname
//access tux3
//use connect & send  function
//use threads to listen and send
void * smsg(void *x);
void *rmsg(void *x);
#define h_addr h_addr_list[0]
void handle_signals();



int connsock;
int main(int argc, char** argv){
    char* name,*p;
    int port,n;
    int addrs;
    struct sockaddr_in conn;
    char buf[MSGSIZE];
    pthread_t sendtid;

    signal(SIGINT, handle_signals);
    if (argc != 4){
        printf("Usage: %s hostname/dns port name\n",argv[0]);
        exit(1);
    }
    port = htons(atoi(argv[2]));
    name = argv[3];
    connsock = socket(AF_INET,SOCK_STREAM,0);
    if (connsock < 0 ) {
		perror("socket");
		exit(1);
	}
    conn.sin_family = AF_INET;
    conn.sin_port = port;
    //check that the hostname used is legitimate and a dns record exists
    if (gethostbyname(argv[1])){
        conn.sin_addr = *(struct in_addr *) gethostbyname(argv[1])->h_addr;  
    }else {
        perror("gethostbyname");
        exit(1);
    }

    //connect to the sockew
    if (connect(connsock,(const struct sockaddr *) &conn,sizeof(struct sockaddr_in)) < 0){
        perror("connect");
        exit(1);
    }
    
    //send required login username
    n = send(connsock,name,strlen(name),MSG_NOSIGNAL);
    if (n<0){
        perror("Can't Login!\n");
        exit(1);
    }

    printf("Logged in!\n");

    //create the socket for sending/writing
    pthread_create(&sendtid, NULL ,smsg, (void *) &connsock);
    while(1){
        //empty the buffer before reading from the socket again.
        memset(buf,0,sizeof(buf));
        n = recv(connsock, buf, MSGSIZE, MSG_NOSIGNAL);
        if (n > 0){
            //handling newlines myself to try control how messages are printed.
             p  = strrchr(buf,'\n');
             if (p != NULL){
                *p = '\0';
             }
            
            printf("%s\n", buf);
        }
        //for if there was an issue receiving or the other side of the connection closes the socket. and also close on error
        if (n <= 0){
            handle_signals();
        }
    }

    pthread_join(sendtid,NULL);
    close(connsock);
}

void * smsg(void * x){
    char buf[MSGSIZE];
    int *connsock = x;
    int n;

    while(1){
        memset(buf,0,sizeof(buf));
        if (fgets(buf,MSGSIZE,stdin) > 0 ){
            n = send(*connsock,&buf,strlen(buf),MSG_NOSIGNAL);
            if (n<0){
                perror("Can't Send!\n");
                exit(1);
            }
        }else{
            handle_signals();
        }

    }
}

void handle_signals(){
    //function aimed at closing connection properly when CTRL-C is hit.
    int n;
    printf("\nClosing connection......\n");
    n = shutdown(connsock,SHUT_RDWR);
    if (n != 0 ){
        perror("Unable to shutdown\n");
    }
    close(connsock);
    exit(0);
}