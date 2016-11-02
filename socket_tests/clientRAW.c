#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<errno.h>
#include<netinet/udp.h>
#include<netinet/ip.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int printIPheader(char *buffer);
void printUDPheader(char *buffer, int iphdr_len);
unsigned short checksum(unsigned short *ptr, int nbytes);
void error(char *msg);

// 12 bytes pseudo header 
// for udp header checksum calculation
struct pseudo_header {
  u_int32_t source_address;
  u_int32_t dest_address;
  u_int8_t placeholder;
  u_int8_t protocol;
  u_int16_t udp_length;
};

/*********************************************************************************
 *   WARNING !!                                                                  *
 *   This program is still in test, all the addresses and ports are hardcoded    *
 *********************************************************************************/

int main (int argc, char *argv[]) {
  int sockfd;
  int one = 1;
  const int *val = &one;
  
  char *buffer;
  buffer = (char *)malloc(2048);
  memset(buffer, 0, 2048);
  
  if (argc < 3) {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(0);
  }

  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sockfd < 0) {
    error("ERROR socket openning");
  }

  if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
    error("ERROR setting socket option");
  }
  
  char *datagram, *data, source_ip[32], *pseudogram;
  datagram = (char *) malloc(2048);
  memset(datagram, 0, 2048);
  
  // IP header
  struct ip *iphdr = (struct ip *) datagram;
  
  // UDP header
  struct udphdr *udph = (struct udphdr *) (datagram + sizeof(struct ip));
  struct sockaddr_in serv_addr;
  struct pseudo_header psh;

  // Data
  data = datagram + sizeof(struct ip) + sizeof(struct udphdr);
  //  strcpy(data, "Hello world! This is a message from clientRAW.");
  char *msg = "Hello world! This is a message from clientRAW.";
  int str_len = strlen(msg);
  for (int i = 0; i < str_len; i++) {
    data[i] = msg[i];
  }
  
  strcpy(source_ip, "10.10.10.3");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(atoi(argv[2]));
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);  // second argument is dest ip

  // Fill in the IP header
  iphdr->ip_hl = 5;
  iphdr->ip_v = 4;
  iphdr->ip_tos = 0;
  iphdr->ip_len = htons(sizeof(struct ip) + sizeof(struct udphdr) + strlen(data));
  iphdr->ip_id = htons(54321);
  iphdr->ip_off = 0;
  iphdr->ip_ttl = 255;
  iphdr->ip_p = IPPROTO_UDP;
  iphdr->ip_sum = 0;
  
  struct in_addr ip_src;
  ip_src.s_addr = inet_addr(source_ip);
  iphdr->ip_src = ip_src;
  iphdr->ip_dst = serv_addr.sin_addr;

  iphdr->ip_sum = checksum((unsigned short *)datagram, iphdr->ip_len);
  
  // fill in UDP header
  udph->source = htons(51818);
  udph->dest = htons(atoi(argv[2]));  // third argument is dest port
  udph->len = htons(8 + strlen(data));
  udph->check = 0;

  psh.source_address = inet_addr(source_ip);
  psh.dest_address = serv_addr.sin_addr.s_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_UDP;
  psh.udp_length = sizeof(struct udphdr) + strlen(data);

  int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(data);
  pseudogram = malloc(psize);

  memcpy(pseudogram, (char*) &psh, sizeof(struct pseudo_header));
  memcpy(pseudogram + sizeof(struct pseudo_header), udph, sizeof(struct udphdr) + strlen(data));
  // check sum
  udph->check = checksum((unsigned short*) pseudogram, psize);

  int sendlen;
  sendlen = sendto(sockfd, datagram,ntohs(iphdr->ip_len), 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if (sendlen < 0) {
    error("ERROR sendto failed");
  } else {
    printf("Successfully send packet\n");
  }

  /*
    // this part of code receives incoming packets, e.g. an ACK from the server
    // not used in this case

  int servlen = sizeof serv_addr;
  int recvlen = recvfrom(sockfd, buffer, 2047, 0, (struct sockaddr *) &serv_addr, &servlen);
  if (recvlen < 0) {
    error("ERROR receive from");
  }

  int iphdr_len1 = printIPheader(buffer);
  int iphdr_len2 = printIPheader(buffer + iphdr_len1);
  printUDPheader(buffer, iphdr_len1 + iphdr_len2);
  */    

  free(pseudogram);
  free(datagram);
  free(buffer);
		       
  close(sockfd);
  
  return 0;  
}

int printIPheader(char *buffer) {
  struct ip *iphdr = (struct ip*) buffer;
  // print the IP header info and return the IP header length
  printf("\n");
  printf("Below are the IP header info\n");
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
  printf("-------------------------------------\n");

  return (iphdr->ip_hl)*4;
}

void printUDPheader(char *buffer, int iphdr_len) {
  // buffer is a pointer to the received packet
  // iphdr_len is the length of the IP header
  
  struct udphdr *udp_hdr = (struct udphdr*) (buffer + iphdr_len);
  // print the UDP header info
  printf("\n");
  printf("Below are the UDP header info\n");
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
  printf("-------------------------------------\n");
}

unsigned short checksum(unsigned short *ptr, int nbytes) {
  register long sum;
  unsigned short oddbyte;
  register short answer;

  sum = 0;
  while(nbytes > 1) {
    sum += *ptr++;
    nbytes-=2;
  }
  if(nbytes == 1) {
    oddbyte = 0;
    *((u_char*) &oddbyte) = *(u_char*)ptr;
    sum += oddbyte;
  }
  sum = (sum >> 16) + (sum & 0xffff);
  sum = sum + (sum >>16);
  answer = (short)~sum;
  return answer;
}

void error(char *msg) {
  perror(msg);
  exit(1);
}
