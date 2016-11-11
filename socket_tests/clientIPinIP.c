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

int main (int argc, char *argv[]) {
  /*
   *  This program requires four arguments:
   *     - src_server is the original server that creates this packet
   *     - dst_server receives the packet from the src_server, encapsulates it, and forwards it to forward server
   *     - forward_server receives the IP-in-IP packet
   *     - port is the port number 
   *
   *  For now, the message sent is hardcoded.
   */
  if (argc < 5) {
    fprintf(stderr, "usage: %s src_server dst_server forward_server port\n", argv[0]);
    exit(0);
  }
  
  int sockfd, recvsockfd;
  int one = 1;
  const int *val = &one;
  char *src_server = argv[1];
  char *dst_server = argv[2];
  char *forward_server = argv[3];
  int portno = atoi(argv[4]);

  // initialize received buffer
  char *buffer;
  buffer = (char *)malloc(2048);
  memset(buffer, 0, 2048);

  // create socket and set socket option
  sockfd = socket(AF_INET, SOCK_RAW, 4);
  if (sockfd < 0) {
    error("ERROR socket openning");
  }
  if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
    error("ERROR setting socket option");
  }

  /* This part is used for receive packets  
  recvsockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (recvsockfd < 0) {
    error("ERROR recvsockfd openning");
  }
  if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
    error ("ERROR setting recvsocket option");
  }

  // receiving info
  cli_addr.sin_family = AF_INET;
  cli_addr.sin_port = htons(51717);
  cli_addr.sin_addr.s_addr = INADDR_ANY;
  int clilen = sizeof cli_addr;
  if (bind(recvsockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
    error("ERROR on binding");
  }
  */

  // allocate space for the packet we are creating
  char *datagram, *data, source_ip[32], *pseudogram;
  datagram = (char *) malloc(2048);
  memset(datagram, 0, 2048);
  
  // IP header
  struct ip *iphdr1 = (struct ip *) datagram; // outer IP header
  struct ip *iphdr2 = (struct ip *) (datagram + sizeof(struct ip)); // inner IP header
  
  // UDP header
  struct udphdr *udph = (struct udphdr *) (datagram + 2 * sizeof(struct ip));
  struct sockaddr_in serv_addr, cli_addr;
  struct pseudo_header psh;

  // Data
  data = datagram + sizeof(struct ip) * 2 + sizeof(struct udphdr);
  char *msg = "Hello world! This is a message from clientRAW.";
  int str_len = strlen(msg);
  for (int i = 0; i < str_len; i++) {
    data[i] = msg[i];
  }
  
  // destination info (to forward server)
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  serv_addr.sin_addr.s_addr = inet_addr(forward_server);

  // Fill in the inner IP header (iphdr2)
  iphdr2->ip_hl = 5;
  iphdr2->ip_v = 4;
  iphdr2->ip_tos = 0;
  iphdr2->ip_len = htons(sizeof(struct ip) + sizeof(struct udphdr) + strlen(data));
  iphdr2->ip_id = htons(54321);
  iphdr2->ip_off = 0;
  iphdr2->ip_ttl = 254;
  iphdr2->ip_p = IPPROTO_UDP;
  iphdr2->ip_sum = 0;
  
  struct in_addr ip2_src;
  ip2_src.s_addr = inet_addr(src_server);
  struct in_addr ip2_dst;
  ip2_dst.s_addr = inet_addr(dst_server);
  iphdr2->ip_src = ip2_src;
  iphdr2->ip_dst = ip2_dst;

  iphdr2->ip_sum = checksum((unsigned short *)datagram + sizeof(struct ip), ntohs(iphdr2->ip_len));
  
  // fill in UDP header
  udph->source = htons(51818);
  udph->dest = htons(portno);
  udph->len = htons(8 + strlen(data));
  udph->check = 0;

  psh.source_address = inet_addr(src_server);
  psh.dest_address = inet_addr(dst_server);
  psh.placeholder = 0;
  psh.protocol = IPPROTO_UDP;
  psh.udp_length = sizeof(struct udphdr) + strlen(data);

  int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(data);
  pseudogram = malloc(psize);

  memcpy(pseudogram, (char*) &psh, sizeof(struct pseudo_header));
  memcpy(pseudogram + sizeof(struct pseudo_header), udph, sizeof(struct udphdr) + strlen(data));
  // check sum
  udph->check = checksum((unsigned short*) pseudogram, psize);
  
  // Fill in the outer IP header (iphdr1)
  iphdr1->ip_hl = 5;
  iphdr1->ip_v = 4;
  iphdr1->ip_tos = 0;
  iphdr1->ip_len = htons(sizeof(struct ip) * 2 + sizeof(struct udphdr) + strlen(data));
  iphdr1->ip_id = htons(54320);
  iphdr1->ip_off = 0;
  iphdr1->ip_ttl = 255;
  iphdr1->ip_p = 4;
  iphdr1->ip_sum = 0;
  
  struct in_addr ip1_src;
  ip1_src.s_addr = inet_addr(dst_server);
  struct in_addr ip1_dst;
  ip1_dst.s_addr = inet_addr(forward_server);
  iphdr1->ip_src = ip1_src;
  iphdr1->ip_dst = ip1_dst;

  iphdr1->ip_sum = checksum((unsigned short *)datagram, ntohs(iphdr1->ip_len));

  // send IP in IP packet
  int sendlen;
  sendlen = sendto(sockfd, datagram,ntohs(iphdr1->ip_len), 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if (sendlen < 0) {
    error("ERROR sendto failed");
  } else {
    printf("Successfully send packet: %d bytes\n", sendlen);
  }

  /* 
     // this part tries to receive an incoming packet
  int recvlen = recvfrom(recvsockfd, buffer, 2047, 0, (struct sockaddr *) &cli_addr, &clilen);
  if (recvlen < 0) {
    error("ERROR receive from");
  } else {
    printf("Successfully receive packet: %d bytes\n", recvlen);
  }

  int iphdr_len1 = printIPheader(buffer);
  int iphdr_len2 = printIPheader(buffer + iphdr_len1);
  printUDPheader(buffer, iphdr_len1 + iphdr_len2);
  */   
 
  free(pseudogram);
  free(datagram);
  free(buffer);
		       
  close(sockfd);
  close(recvsockfd);
  
  return 0;  
}

int printIPheader(char *buffer) {
  // print the IP header info and return the IP header length

  struct ip *iphdr = (struct ip*) buffer;
  
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
  // this function skips the IP header and prints the info contained in UDP packet
  
  struct udphdr *udp_hdr = (struct udphdr*) (buffer + iphdr_len);
  
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
  // this function is used for calculating checksum
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
