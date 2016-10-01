#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>

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

  if (type == ETHERTYPE_IP) {
    fprintf(stdout,"This is an IP packet");
    handle_IP(args, pkthdr, packet);
  } else if (type == ETHERTYPE_ARP) {
    fprintf(stdout,"This is an ARP packet");
  } else if (type == ETHERTYPE_REVARP) {
    fprintf(stdout,"This is an REVARP packet");
  } else {
    fprintf(stdout,"Cannot identify");
  }
  fprintf(stdout,"\n");
}

u_int16_t handle_ethernet (u_char *args, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
  struct ether_header *eptr;
  eptr = (struct ether_header*) packet;
  
  fprintf(stdout, "ethernet header source: %s", ether_ntoa((const struct ether_addr*) &eptr->ether_shost));
  fprintf(stdout, " destination: %s ", ether_ntoa((const struct ether_addr*) &eptr->ether_dhost));

  if (ntohs (eptr->ether_type) == ETHERTYPE_IP) {
    fprintf(stdout, "(IP)");
  } else if (ntohs (eptr->ether_type) == ETHERTYPE_ARP) {
    fprintf(stdout, "(ARP)");
  } else if (ntohs (eptr->ether_type) == ETHERTYPE_REVARP) {
    fprintf(stdout, "(RARP)");
  } else {
    fprintf(stdout,"(UNKNOWN)");
  }

  return ntohs(eptr->ether_type);
}

u_char* handle_IP (u_char *args, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
  const struct my_ip* ip;
  u_int length = pkthdr->len;
  u_int hlen, off, version;
  int i;
  int len;

  ip = (struct my_ip*) (packet + sizeof(struct ether_header));
  length -= sizeof(struct ether_header);

  if (length < sizeof(struct my_ip)) {
    printf("truncated ip %d", length);
    return NULL;
  }

  len = ntohs(ip->ip_len);
  hlen = IP_HL(ip);
  version = IP_V(ip);

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

  off = ntohs(ip->ip_off);
  if ((off & 0x1fff) == 0 ) {
    fprintf(stdout, "IP: ");
    fprintf(stdout, "%s ", inet_ntoa(ip->ip_src));
    fprintf(stdout, "%s %d %d %d %d\n", inet_ntoa(ip->ip_dst), hlen, version, len, off);
  }
  
  return NULL;
}

int main (int argc, char **argv) {

  char *dev;
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t* handle;
  struct bpf_program fp;
  bpf_u_int32 maskp;
  bpf_u_int32 netp;
  u_char* args = NULL;

  if (argc < 2) {
    fprintf(stdout, "Usage: %s numpacket\n", argv[0]); 
    return 0;
  }

  dev = pcap_lookupdev(errbuf);
  if (dev == NULL) {
    fprintf(stdout, "cannot find a device: %s\n", errbuf);
    exit(1);
  }

  pcap_lookupnet(dev, &netp, &maskp, errbuf);

  handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
  if (handle == NULL) {
    fprintf(stdout, "cannot open live: %s\n", errbuf);
    exit(1);
  }

  if (argc > 2) {
    if (pcap_compile(handle, &fp, argv[2], 0, netp) == -1) {
      fprintf(stderr, "Error calling pcap_compile\n");
      exit(1);
    }

    if (pcap_setfilter(handle, &fp) == -1) {
      fprintf(stderr, "Error setting filter\n");
      exit(1);
    }
  }

  pcap_loop(handle, atoi(argv[1]), my_callback, args);

  fprintf(stdout, "\n Done!\n");
  return 0;
}
