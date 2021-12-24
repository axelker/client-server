/*
UNIX Daemon Server Programming Sample Program
Levent Karakas <levent at mektup dot at> May 2001

To compile:	cc -o exampled examped.c
To run:		./exampled
To test daemon:	ps -ef|grep exampled (or ps -aux on BSD systems)
To test log:	tail -f /tmp/exampled.log
To test signal:	kill -HUP `cat /tmp/exampled.lock`
To terminate:	kill `cat /tmp/exampled.lock`
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define SIZE 1024
#define LOCK_FILE	"exampled.lock"
#define LOG_FILE	"exampled.log"

void log_message(filename,message)
char *filename;
char *message;
{
FILE *logfile;
	logfile=fopen(filename,"a");
	if(!logfile) return;
	fprintf(logfile,"%s\n",message);
	fclose(logfile);
}

void signal_handler(sig)
int sig;
{
	switch(sig) {
	case SIGHUP:
		log_message(LOG_FILE,"hangup signal catched");
		break;
	case SIGTERM:
		log_message(LOG_FILE,"terminate signal catched");
		exit(0);
		break;
	}
}

void write_file(int sockfd)
{
    int n; 
    FILE *fp;
    char *filename2 = "file2.txt";
    char buffer[SIZE];

    fp = fopen(filename2, "w");
    if(fp==NULL)
    {
        perror("[-]Error in creating file.");
        exit(1);
    }
    while(1)
    {
        n = recv(sockfd, buffer, SIZE, 0);
        if(n<=0)
        {
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    return;
    
}

void daemonize()
{
int i,lfp;
char str[10];
	if(getppid()==1) return; /* deja un daemon */
	i=fork();
	if (i<0) exit(1); /* erreur */
	if (i>0) {/* père*/
	    exit(0);
	}
    //fils
	setsid(); /* obtain a new process group */
	i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */
	umask(0); /* autorisation*/
	 lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);
	 if (lfp<0) exit(1); /* can not open */
	 if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */
	
	signal(SIGCHLD,SIG_IGN); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP,signal_handler); /* catch hangup signal */
	signal(SIGTERM,signal_handler); /* catch kill signal */
    
}

int main()
{
	daemonize();
   
    char *ip = "127.0.0.1";
    int port = 20;
    int e;

    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0)
    {
        perror("[-]Error in socket");
        exit(1);
    }
     printf("[+]Server socket created. \n");

     server_addr.sin_family = AF_INET;
     server_addr.sin_port = port;
     server_addr.sin_addr.s_addr = inet_addr(ip);

     e = bind(sockfd,(struct sockaddr*)&server_addr, sizeof(server_addr));
     if(e<0)
     {
         perror("[-]Error in Binding");
         exit(1);
     }
     printf("[+]Binding Successfull.\n");
     
    e = listen(sockfd, 10);
    if(e==0)
    {
        printf("[+]Listening...\n");
    }
    else 
    {
        perror("[-]Error in Binding");
        exit(1);
    }
        
    while(1){
        addr_size = sizeof(new_addr);
        new_sock = accept(sockfd,(struct sockaddr*)&new_addr, &addr_size);
        if(fork()==0){
            write_file(new_sock);
            printf("[+]Data written in the text file \n");
            exit(0);
        }
        else {
            close(new_sock);
        }
     }
    
    
    return 0;
}


