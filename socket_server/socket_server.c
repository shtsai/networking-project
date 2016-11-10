#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<netinet/udp.h>
#include<arpa/inet.h>
#include<netinet/if_ether.h>  // for ETH_P_IP

void error(const char *msg);
void printHex(char *buffer, int recvlen);
int getProtocol(char *buffer);

int main (int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "usage %s port\n", argv[0]);
    exit(0);
  }

  int sockfd;
  int portno = atoi(argv[1]);
  struct sockaddr_in cli_addr, serv_addr;
  socklen_t clilen = sizeof(cli_addr);
  int recvlen;
  
  char *buffer;
  buffer = (char *) malloc(2048);
  memset(buffer, 0, 2048);

  sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
  //  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
  if (sockfd < 0) {
    error("ERROR socket");
  }

  /* attemps to make TCP connection
  serv_addr.sin_family = AF_INET;
  // serv_addr.sin_addr.s_addr = INADDR_ANY;
  if (inet_pton(AF_INET, "10.10.10.2", &serv_addr.sin_addr) <= 0) {
    printf("inet_pton error occurred\n");
    exit(0);
  }
  serv_addr.sin_port = htons(portno);
  
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    error("ERROR on binding");
  }
  */
  
  /* set specific interface
  if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, "enp0s8", strlen("enp0s8") + 1) < 0) {
    error("ERROR setting socket option");
  }
  */

  int count = 0;
  while (1) {
    recvlen = recvfrom(sockfd, buffer, 2047, 0, (struct sockaddr *) &cli_addr, &clilen);
    if (recvlen > 0) {
      printf("Packet %d, %d bytes, ",count, recvlen);
      //      printHex(buffer, recvlen);
      int protocol = getProtocol(buffer);
      printf("Protocol = %d\n", protocol);
    }
    memset(buffer, 0 ,2048);
    recvlen = 0;
    printf("/n");
    count++;
  }

  free(buffer);
  close(sockfd);
  return 0;
}

int getProtocol(char *buffer) {
  // input is a buffer of a received packet (Ethernet header included),
  // this function returns the protocol number in the IP header
  int ethhdr_len = (int) sizeof(struct ethhdr);
  char *p_iphdr = buffer + ethhdr_len;
  struct ip *iphdr = (struct ip *) p_iphdr;
  return iphdr->ip_p;
}

void printHex(char *buffer, int recvlen) {
  printf("Here is the Hex format of the packet\n");
  for (int i = 0; i < recvlen; i++) {
    printf("%x ", buffer[i]);
  }
  printf("\n\n");
}

void error(const char *msg) {
  perror(msg);
  exit(1);
}

