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
int encapsulate(char *buffer, char *newbuffer, struct sockaddr_in *cli_addr);
unsigned short checksum(unsigned short *ptr, int nbytes);
void error(const char *msg);

int main(int argc, char *argv[]){
  if (argc < 2) {
    fprintf(stderr, "usage %s port\n", argv[0]);
    exit(0);
  }

  int sockfd, sendsockfd, recvsockfd;
  struct sockaddr_in serv_addr, cli_addr, cli_addr2;
  int portno;
  int recvlen, iphdr_len;
  socklen_t clilen = sizeof(cli_addr);
  socklen_t clilen2 = sizeof(cli_addr2);
  int one = 1;
  const int *val = &one;

  char *buffer;
  buffer = (char *) malloc(2048);
  memset(buffer, 0, 2048);
  char *newbuffer;
  newbuffer = (char *) malloc(2048);
  memset(newbuffer, 0, 2048);
  
  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sockfd < 0) {
    error("ERROR socket");
  }

  sendsockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sendsockfd < 0) {
    error("ERROR sendsocket");
  }

  recvsockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (recvsockfd < 0) {
    error("ERROR recvsocket");
  }
  
  if (setsockopt(sendsockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
    error("ERROR setting socket option");
  }

  if (setsockopt(recvsockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
    error("ERROR setting socket option");
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
  recvlen = recvfrom(sockfd, buffer, 2047, 0, (struct sockaddr *) &cli_addr, &clilen);
  if (recvlen > 0) {
    printf("Here is the received packet, received %d bytes\n", recvlen);
    
    iphdr_len = printIPheader(buffer);
    printUDPheader(buffer, iphdr_len);

    int newbuffer_len = encapsulate(buffer, newbuffer, &cli_addr);

    //printf("\nHere is the encapsulated packet:\n");
    //printIPheader(newbuffer);
    //printIPheader(newbuffer + iphdr_len);
    //printUDPheader(newbuffer, iphdr_len * 2);
    
    int sendlen;
    sendlen = sendto(sendsockfd, newbuffer, newbuffer_len, 0,(struct sockaddr *) &cli_addr, sizeof cli_addr);
    if (sendlen > 0){
      printf("encapsulation sent successfully: %d bytes\n\n", sendlen);
    } else {
      printf("fail to send encapsulation\n");
    }

    cli_addr2.sin_family = AF_INET;
    cli_addr2.sin_port = htons(51717);
    struct ip iphdr2 = *(struct ip *) buffer;
    cli_addr2.sin_addr = iphdr2.ip_src;

  }
  
  // receive encapsulated packet
  memset(buffer, 0, 2048);
  recvlen = recvfrom(recvsockfd, buffer, 2047, 0, (struct sockaddr *) &cli_addr2, &clilen2);
  if (recvlen > 0) {
    printf("Here is the received packet, received %d bytes\n", recvlen);
    iphdr_len = printIPheader(buffer);
    printf("iphdr_len = %d\n", iphdr_len);
    //    printIPheader(buffer + iphdr_len);
    printUDPheader(buffer, iphdr_len);
  }
  
  close(sockfd);
  close(sendsockfd);
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

int encapsulate(char *buffer, char *newbuffer, struct sockaddr_in *cli_addr ) {
  // This function adds an IP header to the packet
  // and stores the result in newbuffer.
  // It also updates cli_addr with destination info
  
  struct ip iphdr = *(struct ip *) buffer;
  struct ip *newiphdr = (struct ip*) newbuffer;

  // fill in new ip header info
  newiphdr->ip_hl = iphdr.ip_hl;
  newiphdr->ip_v = iphdr.ip_v;
  newiphdr->ip_tos = iphdr.ip_tos;
  newiphdr->ip_len = htons(ntohs(iphdr.ip_len) + sizeof(struct ip));
  newiphdr->ip_id = iphdr.ip_id;
  newiphdr->ip_off = iphdr.ip_off;
  newiphdr->ip_ttl = iphdr.ip_ttl;
  newiphdr->ip_p = 4;
  newiphdr->ip_sum = 0;
  newiphdr->ip_src = iphdr.ip_dst;
  //newiphdr->ip_dst = iphdr.ip_src;
  struct in_addr ip_dst;
  ip_dst.s_addr = inet_addr("10.10.10.4");
  newiphdr->ip_dst = ip_dst;

  // copy old packet as payload
  memcpy(newbuffer + sizeof(struct ip), buffer, iphdr.ip_len);
  
  // checksum
  newiphdr->ip_sum = checksum((unsigned short *)newbuffer, newiphdr->ip_len);

  // update cli_addr with destination info
  cli_addr->sin_family = AF_INET;
  cli_addr->sin_port = htons(51717);
  cli_addr->sin_addr = newiphdr->ip_dst;

  return ntohs(newiphdr->ip_len);
}

unsigned short checksum(unsigned short *ptr, int nbytes){
  register long sum;
  unsigned short oddbyte;
  register short answer;

  sum = 0;
  while (nbytes > 1) {
    sum += *ptr++;
    nbytes -= 2;
  }

  if (nbytes == 1) {
    oddbyte == 0;
    *((u_char *) &oddbyte) = *(u_char *)ptr;
    sum += oddbyte;
  }
  sum = (sum >> 16) + (sum & 0xffff);
  sum = sum + (sum >> 16);
  answer = (short) ~sum;
  return answer;
}

void error(const char *msg){
  perror(msg);
  exit(1);
}
