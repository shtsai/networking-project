#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[])
{
  int sockfd;
  struct sockaddr_in serv_addr, cli_addr;
  socklen_t clilen = sizeof(cli_addr);
  int portno;
  char buffer[256];
  int recvlen, n;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    error("ERROR opening socket");
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
    error("ERROR on binding");
  }

  printf("waiting on port %d\n", portno);
  recvlen = recvfrom(sockfd, buffer, 255, 0, (struct sockaddr *) &cli_addr, &clilen);
  if (recvlen > 0) {
    buffer[recvlen] = 0;
    printf("received message: %s\n", buffer);
  }

  n = sendto(sockfd, "I got your message", 18,0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
  if (n < 0) error ("ERROR writing to socket");
  
  close(sockfd);
  return 0;
}
