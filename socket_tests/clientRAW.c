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

// 12 bytes pseudo header 
// for udp header checksum calculation
// why???
struct pseudo_header {
  u_int32_t source_address;
  u_int32_t dest_address;
  u_int8_t placeholder;
  u_int8_t protocol;
  u_int16_t udp_length;
};

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

int main (int argc, char *argv[]) {
  int sockfd;

  if (argc < 3) {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(0);
  }

  sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_RAW);
  if (sockfd < 0) {
    error("ERROR socket openning");
  }

  char *datagram, *data, source_ip[32], *pseudogram;
  datagram = (char *) malloc(4096);
  memset(datagram, 0, 4096);

  // IP header
  struct iphdr *iph = (struct iphdr *) datagram;

  // UDP header
  struct udphdr *udph = (struct udphdr *) (datagram + sizeof(struct ip));
  struct sockaddr_in sin;
  struct pseudo_header psh;

  // Data
  data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
  strcpy(data, "Hello world! This is a message from clientRAW.");

  strcpy(source_ip, "10.10.10.3");

  sin.sin_family = AF_INET;
  sin.sin_port = htons(51717);
  //  sin.sin_addr.s_addr = inet_addr("10.10.10.2");
  sin.sin_addr.s_addr = inet_addr(argv[1]);  // second argument is dest ip

  // Fill in the IP header
  iph->ihl = 5;
  iph->version = 4;
  iph->tos = 0;
  iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(data);
  iph->id = htonl(54321);
  iph->frag_off = 0;
  iph->ttl = 255;
  iph->protocol = IPPROTO_UDP;
  iph->check = 0;
  iph->saddr = inet_addr(source_ip);
  iph->daddr = sin.sin_addr.s_addr;

  // IP checksum
  iph->check = checksum((unsigned short *)datagram, iph->tot_len);

  // fill in UDP header
  udph->source = htons(51818);
  //  udph->dest = htons(51717);
  udph->dest = htons(atoi(argv[2]));  // third argument is dest port
  udph->len = htons(8 + strlen(data));
  udph->check = 0;

  psh.source_address = inet_addr(source_ip);
  psh.dest_address = sin.sin_addr.s_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_UDP;
  psh.udp_length = htons(sizeof(struct udphdr) + strlen(data));

  int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(data);
  pseudogram = malloc(psize);

  memcpy(pseudogram, (char*) &psh, sizeof(struct pseudo_header));
  memcpy(pseudogram + sizeof(struct pseudo_header), udph, sizeof(struct udphdr) + strlen(data));

  udph->check = checksum((unsigned short*) pseudogram, psize);

  int sendlen;
  sendlen = sendto(sockfd, datagram, iph->tot_len, 0, (struct sockaddr *) &sin, sizeof(sin));
  if (sendlen < 0) {
    error("ERROR sendto failed");
  } else {
    printf("Successfully send packet\n");
  }
  
  free(pseudogram);
  free(datagram);
  close(sockfd);
  
  return 0;
  
}



