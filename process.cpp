#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n,clock_ts,add_ts,server_ts=0,clock1_ts;
    time_t t;
    float wait_time;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    srand((unsigned) time(&t));

    char buffer[256];   
   
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    char ch;
    int sz=0;
    //calculating file size of transactions.txt
    

    //printf("Number of lines in transactions.txt is %d",sz);

    //send intrst = 0 to indicate it does not calculate interest
    clock_ts = rand() % 60;
    printf("\nThe local clock time is %d\n",clock_ts);
    n = write(sockfd,&clock_ts,sizeof(clock_ts));
        if (n < 0) 
        {
            error("ERROR writing to socket");
        }
    //send transaction file size to servver
        bzero(buffer,256);
    n = read(sockfd,buffer,256);
    if (n < 0) error("ERROR reading from socket");
    server_ts = atoi(buffer);
    printf("\nReceived time daemon timestamp:%d\n",server_ts);
    clock1_ts = clock_ts - server_ts;
    printf("\nSending (server_ts-local_ts)=%d value to time_daemon\n",clock1_ts);
    n = write(sockfd,&clock1_ts,sizeof(clock1_ts));
        if (n < 0) 
        {
            error("ERROR writing to socket");
        }
        bzero(buffer,256);
    n = read(sockfd,buffer,256);
    if (n < 0) error("ERROR reading from socket");
    add_ts = atoi(buffer);
    //printf("\nserver_ts is :%d",add_ts);
    clock_ts += add_ts;
    printf("\nAdjustment:%d received from time_daemon\n",add_ts);
    printf("\nclock_ts is set to %d\n",clock_ts);
        /*bzero(buffer,256);
    n = read(sockfd,buffer,256);
    if (n < 0) error("ERROR reading from socket");
    printf("buffer is:%s",buffer);
    add_ts = atoi(buffer);
    clock_ts += add_ts;
    printf("The clock time is set to %d",clock_ts); */   
    close(sockfd);
    return 0;
}
