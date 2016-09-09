#include <stdio.h>
#include <pcap.h>
#include <string.h>

#define MAXBYTES2CAPTURE 2048

void processPacket(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char * packet) {
  int i = 0, *counter = (int *) arg;

  printf("Packet Count: %d\n", ++(*counter));
  printf("Received Packet Size: %d\n", pkthdr->len);
  printf("Payload:\n");

  for (i=0; i<pkthdr->len; i++) {
    if (isprint(packet[i])) 
      printf("%c ", packet[i]);
    else
      printf(". ");

    if ((i%16 == 0 && i != 0) || i == pkthdr->len-1)
      printf("\n");
  }

  return;
}

int main(int args, char *argv[])
{
  int i = 0, count = 0;
  pcap_t *handle;
  char *dev;
  char errbuf[PCAP_ERRBUF_SIZE];
  struct bpf_program fp;
  char filter_exp[] = "port 23";
  bpf_u_int32 mask;
  bpf_u_int32 net;
  struct pcap_pkthdr header;
  const u_char *packet;

  dev = pcap_lookupdev(errbuf);
  if (dev == NULL) {
    fprintf(stderr, "Couldn't find default device: %s\n",errbuf);
    return(2);
  }

  printf("Device: %s\n", dev);
 
  // find properties for the device
  if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
    fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
    net = 0;
    mask = 0;
  }

  // open the session
  handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
  if (handle == NULL) {
    fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
    return(2);
  }

  /**
  // compile and apply the filter
  if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
    fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
    return(2);
  }
  if (pcap_setfilter(handle, &fp) == -1) {
    fprintf(stderr, "Couldn't install filter %s: %s", filter_exp, pcap_geterr(handle));
  }
  **/

  /**
  // grab a packet
  packet = pcap_next(handle, &header);
  printf("Jacked a packet with length of [%d]\n", header.len);
  pcap_close(handle);
  return(0);
  **/

  pcap_loop(handle, -1, processPacket, (u_char *)&count);

  return 0;

}
