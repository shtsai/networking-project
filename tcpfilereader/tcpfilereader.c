#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>

int tcponly = 0;

// Q: The stanford tutorial uses the following struct instead of 
// the ip struct in netinet/ip.h
// Why? What are the differences?
struct my_ip {
  u_int8_t  ip_vhl;
#define IP_V(ip) (((ip)->ip_vhl & 0xf0) >> 4)
#define IP_HL(ip) ((ip)->ip_vhl & 0x0f)
  u_int8_t  ip_tos;
  u_int16_t ip_len;
  u_int16_t ip_id;
  u_int16_t ip_off;
#define IP_DF 0x4000
#define IP_MF 0x2000
#define IP_OFFMASK 0x1fff
  u_int8_t ip_ttl;
  u_int8_t ip_p;
  u_int16_t ip_sum;
  struct in_addr ip_src, ip_dst;
};


u_int16_t handle_ethernet (u_char *args, const struct pcap_pkthdr* pkthdr, const u_char* packet);
u_char* handle_IP (u_char* args, const struct pcap_pkthdr* pkthdr, const u_char* packet);

void my_callback(u_char *args, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
  u_int16_t type = handle_ethernet(args, pkthdr, packet);

  //fprintf(stdout, "ETHERTYPE = %x\n", type);

  if (type == ETHERTYPE_IP) {   // 0x0800
    //    fprintf(stdout,"This is an IP packet");
    handle_IP(args, pkthdr, packet);
  } else if (type == ETHERTYPE_ARP) {   // 0x0806
    //    fprintf(stdout,"This is an ARP packet");
  } else if (type == ETHERTYPE_REVARP) {  // 0x0835
    //    fprintf(stdout,"This is an RARP packet");
  } else {
    //    fprintf(stdout,"Cannot identify");
  }

}

u_int16_t handle_ethernet (u_char *args, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
  struct ether_header *eptr;
  eptr = (struct ether_header*) packet;

  /** 
  fprintf(stdout, "ethernet header source: %s", ether_ntoa((const struct ether_addr*) &eptr->ether_shost));
  fprintf(stdout, " destination: %s ", ether_ntoa((const struct ether_addr*) &eptr->ether_dhost));
  **/

  return ntohs(eptr->ether_type);
}

u_char* handle_IP (u_char *args, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
  const struct my_ip* ip;
  u_int length = pkthdr->len;
  u_int hlen, off, version;
  int i;
  int len;
  u_char protocol;
  const struct tcphdr* tcp_hdr;

  // skip ether_header part
  ip = (struct my_ip*) (packet + sizeof(struct ether_header));
  length -= sizeof(struct ether_header);

  if (length < sizeof(struct my_ip)) {
    printf("truncated ip %d", length);
    return NULL;
  }

  len = ntohs(ip->ip_len);
  hlen = IP_HL(ip);
  version = IP_V(ip);

  // check if this IP header is valid
  if (version != 4) {
    fprintf(stdout, "Unknown version %d\n", version);
    return NULL;
  }
  if (hlen < 5) {
    fprintf(stdout, "bad-hlen %d \n", hlen);
  }
  if (length < len) {
    fprintf(stdout, "\ntruncated IP - %d bytes missing \n", len - length); 
  }
  
  protocol = ip->ip_p;

  // Q: What is this test for? ( off & 0x1fff == 0 )
  off = ntohs(ip->ip_off);

  if (protocol == 6 || !tcponly) {
  
    if ((off & 0x1fff) == 0 ) {
      fprintf(stdout, "0x%x:       ", protocol);
      fprintf(stdout, "%s       ", inet_ntoa(ip->ip_src));
      fprintf(stdout, "%s           ", inet_ntoa(ip->ip_dst));

      if (protocol == 6) {
	// skip ip header part
	tcp_hdr = (struct tcphdr*) (packet + sizeof(struct ether_header) 
				    + sizeof(struct tcphdr));
	// check validity
	length -= sizeof(struct tcphdr);
	if (length < sizeof(struct tcphdr)) {
	    printf("truncated tcp %d", length);
	    return NULL;
        }
	
	fprintf(stdout, "%d             ", ntohs(tcp_hdr->th_sport));
	fprintf(stdout, "%d ", ntohs(tcp_hdr->th_dport));

      }
      fprintf(stdout, "\n");
    }
  }
  
  return NULL;
}

int main (int argc, char **argv) {

  char *dev;
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t* handle;
  struct bpf_program fp;
  //  bpf_u_int32 maskp;
  //  bpf_u_int32 netp;
  u_char* args = NULL;

  if (argc < 2) {
    fprintf(stdout, "Usage: %s numpacket\n", argv[0]); 
    return 0;
  }

  handle = pcap_open_offline(argv[1], errbuf);
  if (handle == NULL) {
    fprintf(stdout, "cannot open file: %s\n", errbuf);
    exit(1);
  }

  // use filter to obtain only "TCP" records
  // Q: pcap_compile requires maskp and netp,
  //    What is that for? 
  //    When we read from a pcap file, we don't have a device and thus we cannot use 
  //    pcap_lookupnet to obtain netp and maskp, what should we do?
  if (argc == 3) {
    if (pcap_compile(handle, &fp, argv[2], 0, 0) == -1) {
      fprintf(stderr, "Error calling pcap_compile\n");
      exit(1);
    }

    if (pcap_setfilter(handle, &fp) == -1) {
      fprintf(stderr, "Error setting filter\n");
      exit(1);
    }
  }

  fprintf(stdout, "Protocol  |   Source IP   |   Destination IP  | Src tcp port | Dst tcp port \n");

  pcap_loop(handle, -1, my_callback, args);

  fprintf(stdout, "\n Done!\n");
  return 0;
}
