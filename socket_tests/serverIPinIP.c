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

int printIPheader(char *buffer);
void printUDPheader(char *buffer, int iphdr_len);

void error(const char *msg){
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[]){
  int sockfd;
  struct sockaddr_in serv_addr, cli_addr, cli_addr2;
  int portno;
  int recvlen, iphdr_len;
  socklen_t servlen = sizeof(serv_addr);
  socklen_t clilen = sizeof(cli_addr);
  socklen_t clilen2 = sizeof(cli_addr2);
  int one = 1;
  const int *val = &one;

  char *buffer;
  buffer = (char *) malloc(2048);
  memset(buffer, 0, 2048);
  
  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sockfd < 0) {
    error("ERROR socket");
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
  recvlen = recvfrom(sockfd, buffer, 2047, 0, (struct sockaddr *) &serv_addr, &servlen);
  if (recvlen > 0) {
    printf("Here is the received packet, received %d bytes\n", recvlen);
    
    iphdr_len = printIPheader(buffer);
    printIPheader(buffer + iphdr_len);
    printUDPheader(buffer, 2 * iphdr_len);

  }

  close(sockfd);
  return 0;		     
}

int printIPheader(char *buffer) {
  struct ip *iphdr = (struct ip*) buffer;
  // print the IP header info and return the IP header length
  printf("IP header info: \n");
  printf("-------------------------------------\n");
  printf("header length = %d\n", iphdr->ip_hl);
  printf("version = %d\n", iphdr->ip_v);
  printf("type of service = %d\n", iphdr->ip_tos);
  printf("total length = %d\n", ntohs(iphdr->ip_len));
  printf("identification = %d\n", ntohs(iphdr->ip_id));
  printf("fragment offset = %d\n", ntohs(iphdr->ip_off) & 0x1fff);
  printf("time to live = %d\n", iphdr->ip_ttl);
  printf("protocol = %d\n", iphdr->ip_p);
  printf("checksum = %d\n", ntohs(iphdr->ip_sum));
  printf("source address = %s\n", inet_ntoa(iphdr->ip_src));
  printf("destination address = %s\n", inet_ntoa(iphdr->ip_dst));
  printf("-------------------------------------\n\n");

  return (iphdr->ip_hl)*4;
}

void printUDPheader(char *buffer, int iphdr_len) {
  // buffer is a pointer to the received packet
  // iphdr_len is the length of the IP header
  
  struct udphdr *udp_hdr = (struct udphdr*) (buffer + iphdr_len);
  // print the UDP header info
  printf("UDP header info: \n");
  printf("-------------------------------------\n");
  printf("source port = %d\n", ntohs(udp_hdr->uh_sport));
  printf("destination port = %d\n", ntohs(udp_hdr->uh_dport));
  printf("udp length = %d\n", ntohs(udp_hdr->uh_ulen));
  printf("udp checksum = %d\n", ntohs(udp_hdr->uh_sum));

  printf("data: \n");
  int datalen = ntohs(udp_hdr->uh_ulen) - 8;
  for (int i = 0; i < datalen; i++) {
    // + 8 because udp header length is 8
    printf("%c", *(buffer + iphdr_len + 8 + i));
  }
  printf("\n");
  printf("-------------------------------------\n\n");
}
