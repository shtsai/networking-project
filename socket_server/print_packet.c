#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/ip.h>
#include<netinet/udp.h>
#include<arpa/inet.h>

#include<print_packet.h>


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
