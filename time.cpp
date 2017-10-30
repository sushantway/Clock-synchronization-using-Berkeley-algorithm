//Server code to accept client requests
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

//#define STACK_MAX 100
static int num_proc = 0;
static int total_ts = 0;
static int server_ts = 50;
static int ack = 0;
static int flag = 0;

//mutex to lock the crtical resource for concurrency control
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//array of structures to store records
//struct process proc[1000];
void * connection_handler(void *); /*connection handler for each client request*/
void startprocess(int);

static int arg_proc = 0;

//Default error meassage
void error(const char *msg)
{
    perror(msg);
    exit(1);
}


int main(int argc, char *argv[])
{   
    //printf("\nHi");
    int *new_sock;
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     int fd;
     struct stat mystat;
     //static int server_ts = 20;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     arg_proc = atoi(argv[2]);
     //printf("\nPort number is:%d",portno);
     printf("\nTotal number of processes is:%d\n",arg_proc);

     serv_addr.sin_family = AF_INET;

     serv_addr.sin_addr.s_addr = INADDR_ANY;

     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     //printf("\nHey");
     listen(sockfd,10);
     clilen = sizeof(cli_addr);
     
     //printf("\nWaiting for connections");
     printf("\nMulticasting time_daemon timestamp:%d to processes\n",server_ts);
     while (newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen)) {
         //printf("Connection accepted \n");
         num_proc++;
         new_sock = (int *)malloc(1);   
        *new_sock = newsockfd;
         pthread_t pid;
         if(pthread_create(&pid,NULL,connection_handler,(void*)new_sock) < 0)    
         {
            perror("No thread created");
            return 1;
         }

        //printf("handler assigned\n");
     } // end of while 
     //printf("\nServer_ts set to :%d",server_ts);
     close(sockfd);
     return 0; 
}

void * connection_handler(void *socknewfd)
{
    //printf("\nInside connection handler\n");
    int sock1 = *(int *)socknewfd;
    //printf("\nConnected to client id:%d\n",sock1);
    startprocess(sock1);
}

void startprocess (int sock)
{
   int clock_ts,n=0,avg_ts,send_ts,diff_ts;
   char buffer[256];   
   
   //n = read(sock,buffer1,255);
   //printf("Inside startproces\n");
    n = read(sock,&clock_ts,sizeof(clock_ts));
    if (n < 0) error("ERROR reading from socket");
    //printf("\nClock_ts:%d received from process\n",clock_ts);
    bzero(buffer,256);
    sprintf(buffer,"%d",server_ts);

    n = write(sock,buffer,strlen(buffer));
        if (n < 0) 
        {
            error("ERROR writing to socket");
        }
        n = read(sock,&diff_ts,sizeof(diff_ts));
    if (n < 0) error("ERROR reading from socket");
    printf("\ndifference_ts:%d received from process\n",diff_ts);

    total_ts += diff_ts;
    while(num_proc != arg_proc)
    {
        sleep(1);
        //printf("Hi");
        //printf("\nnum_proc:%d",num_proc);
    //printf("\narg_proc:%d",arg_proc);
    }
    //printf("\ntotal_ts is:%d",total_ts);
    avg_ts = total_ts/(arg_proc+1);
    //printf("\navg_ts is :%d",avg_ts);
    send_ts = avg_ts - diff_ts;
    printf("\nSending time to be adjusted to process\n:%d",send_ts);
    bzero(buffer,256);
    sprintf(buffer,"%d",send_ts);
    n = write(sock,buffer,strlen(buffer));
        if (n < 0) 
        {
            error("ERROR writing to socket");
        }
        if(flag == 0)
        {
            server_ts += avg_ts;
            flag = 1;
        }
        printf("\nServer_ts is set to %d\n",server_ts);

}






