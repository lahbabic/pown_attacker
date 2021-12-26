#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#define NUMBEROFPORTs 65535 
#define MAXLINE 1024

typedef struct arg{
    int sockfd;
    char buffer[MAXLINE];
    int msg_waitall;
    struct sockaddr **cliaddr;
    int *len;
}args;

typedef struct sendarg{
    int sockfd;
    char buffer[MAXLINE];
    int len;
    int msg_confirm;
    struct sockaddr **cliaddr;
}sendargs;

int main(int argc, char const *argv[])
{
    pthread_t threadids[131070];
    struct sockaddr_in servaddr[65536], cliaddr[65536];
    args *recvfrom_args[65536];
    sendargs *sendto_args[65536];
    for(int i = 1; i < 65536; i++)
    {
        recvfrom_args[i] = (args *)malloc(sizeof(args));
        sendto_args[i] = (sendargs *)malloc(sizeof(sendargs));
        if ( (recvfrom_args[i]->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
            perror("socket creation failed");
            exit(EXIT_FAILURE);
        }
        memset(&servaddr[i], 0, sizeof(servaddr));
        memset(&recvfrom_args[i]->cliaddr, 0, sizeof(&recvfrom_args[i]->cliaddr));
        servaddr[i].sin_family    = AF_INET; // IPv4
        servaddr[i].sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr[i].sin_port = htons(i);
        if ( bind(recvfrom_args[i]->sockfd, (const struct sockaddr *)&servaddr[i], sizeof(servaddr[i])) < 0 )
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        recvfrom_args[i]->len = sizeof(cliaddr);
        recvfrom_args[i]->msg_waitall = MSG_WAITALL;
    }       
    // Creating socket file descriptor   
       
       
       
    int len, n;
     
    while(1){
        sleep(1);
        for(int i = 1; i < 65536; i++){
            pthread_create(&threadids[i], NULL, recvfrom, &recvfrom_args[i]);
            recvfrom_args[i]->buffer[1024] = '\0';
            printf("Client : %s\n", recvfrom_args[i]->buffer);         
            sendto_args[i]->sockfd = recvfrom_args[i]->sockfd;
            sendto_args[i]->buffer[0] = -1;
            sendto_args[i]->len = 1;
            sendto_args[i]->msg_confirm = MSG_CONFIRM;
            sendto_args[i]->cliaddr = recvfrom_args[i]->cliaddr;
        }
        for(int i = 65536; i < 131070; i++){
            pthread_create(&threadids[i], NULL, sendto, &sendto_args[i]); 
            printf("Hello message sent.\n"); 
        }
    }
    return 0;
}
