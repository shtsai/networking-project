#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>

void error (const char *msg);

int main(int argc, char *argv[])
{
  // The first argument of this program is the destination IP address
  // The second argument is the port number
  // The user will then be prompted to enter a message

  int sockfd;
  int portno;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char buffer[256];
  int sendlen, recvlen;
  
  if (argc < 3){
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(0);
  }
  
  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
    error("ERROR openning socket");

  // fill in destination address
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <=0){
    error("ERROR inet_pton error occured");
  }
  serv_addr.sin_port = htons(portno);

  // get user message
  printf("Please enter the message: ");
  bzero(buffer, 256);
  fgets(buffer, 255, stdin);

  // send the packet
  sendlen = strlen(buffer) + 1;
  if (sendto(sockfd, buffer, sendlen, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) > 0) {
    printf("send message succesfully\n");
  }

  /* this part can be actived to receive the response from the server
  recvlen = recvfrom(sockfd, buffer, 255, 0, NULL, NULL);
  if (recvlen > 0) {
    buffer[recvlen] = 0;
    printf("received message: %s\n", buffer);
  }
  */

  close(sockfd);
  return 0;
}

void error (const char *msg)
{
  perror(msg);
  exit(0);
}
