#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>

void error (const char *msg)
{
  perror(msg);
  exit(0);
}

int main(int argc, char *argv[])
{
  int sockfd;
  int portno;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char buffer[256];
  int sendlen, recvlen;
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  if (argc < 3){
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(0);
  }
  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
    error("ERROR openning socket");

  serv_addr.sin_family = AF_INET;
  if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <=0){
    error("ERROR inet_pton error occured");
  }
  serv_addr.sin_port = htons(portno);

  printf("Please enter the message: ");
  bzero(buffer, 256);
  fgets(buffer, 255, stdin);
  sendlen = strlen(buffer) + 1;
  if (sendto(sockfd, buffer, sendlen, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) > 0) {
    printf("send message succesfully\n");
  }

  /*
  recvlen = recvfrom(sockfd, buffer, 255, 0, NULL, NULL);
  if (recvlen > 0) {
    buffer[recvlen] = 0;
    printf("received message: %s\n", buffer);
  }
  */

  close(sockfd);
  return 0;
}
