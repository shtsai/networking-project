#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include<netinet/tcp.h>
#include<netinet/ip.h>
#include<arpa/inet.h>

struct pseudo_header
{
  u_int32_t source_address;
  u_int32_t dest_address;
  u_int8_t placeholder;
  u_int8_t protocol;
  u_int16_t tcp_length;
};

int createTCP(char *datagram, char *src, char *dest, int port, struct sockaddr_in serv_addr, char *msg);

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
  int one = 1;
  const int *val = &one;
  char *src, *dest;
  int port;

  if (argc < 3) {
    fprintf(stderr, "usage: %s hostname port\n", argv[0]);
  }

  // create socket
  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
  if (sockfd < 0)
    error("ERROR socket");
  if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
    error("ERROR setting socket option");

  // allocate space 
  char *datagram;
  datagram = (char *) malloc(4096);
  memset(datagram, 0 ,4096);
  char *msg = "Hello world from TCP RAW";

  // address
  src = "10.10.10.3";
  dest = argv[1];
  port = atoi(argv[2]);
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(dest);
  serv_addr.sin_port = htons(port);
  
  int packet_len = createTCP(datagram, src, dest, port, serv_addr, msg); 
  
  // send the packet
  int sendlen;
  sendlen = sendto(sockfd, datagram, packet_len, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if (sendlen < 0) {
    error("ERROR sendto failed");
  } else {
    printf("Successfully sent packet\n");
  }

  free(datagram);
  close(sockfd);

  return 0;
  
}

int createTCP(char *datagram, char *src, char *dest, int port, struct sockaddr_in serv_addr, char *msg) {
  // This function takes the given parameters and creates a TCP packet.

  char *data, *pseudogram;
  
  // IP header
  struct ip *iphdr = (struct ip *) datagram;

  // TCP header
  struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
  struct pseudo_header psh;

  // data
  data = datagram + sizeof(struct iphdr) + sizeof(struct tcphdr);
  strcpy(data, msg);

  // Fill in the IP header
  iphdr->ip_hl = 5;
  iphdr->ip_v = 4;
  iphdr->ip_tos = 0;
  iphdr->ip_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(data));
  iphdr->ip_id = htons(54321);
  iphdr->ip_off = 0;
  iphdr->ip_ttl = 255;
  iphdr->ip_p = IPPROTO_TCP;
  iphdr->ip_sum = 0;
  
  struct in_addr ip_src;
  ip_src.s_addr = inet_addr(src);
  iphdr->ip_src = ip_src;
  iphdr->ip_dst = serv_addr.sin_addr;

  iphdr->ip_sum = checksum((unsigned short *)datagram, iphdr->ip_len);

  // Fill in the TCP header
  tcph->source = htons(51919);
  tcph->dest = htons(port);
  tcph->seq = 0;
  tcph->ack_seq = 0;
  tcph->doff = 5;
  tcph->fin = 0;
  tcph->syn = 1;
  tcph->rst = 0;
  tcph->psh = 0;
  tcph->ack = 0;
  tcph->urg = 0;
  tcph->window = htons(5840);
  tcph->check = 0;
  tcph->urg_ptr = 0;

  // Fill in pseudo header
  psh.source_address = inet_addr(src);
  psh.dest_address = serv_addr.sin_addr.s_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_TCP;
  psh.tcp_length = htons(sizeof(struct tcphdr) + strlen(data));

  int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data);
  pseudogram = malloc(psize);
  memcpy(pseudogram, (char *) &psh, sizeof(struct pseudo_header));
  memcpy(pseudogram + sizeof(struct pseudo_header), tcph, sizeof(struct tcphdr) + strlen(data));

  tcph->check = checksum((unsigned short*) pseudogram, psize);

  free(pseudogram);
  return ntohs(iphdr->ip_len);
}
