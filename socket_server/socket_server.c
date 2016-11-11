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
#include<netinet/if_ether.h>  // for ETH_P_IP

#include<print_packet.h>  // for print packets

void error(const char *msg);
void printHex(char *buffer, int recvlen);
char* skipEthHdr(char *buffer);
int getProtocol(char *packet);
void handleUDP(char *packet);
void handleIPinIP(char *packet);
void handleOSPF(char *packet);

int main (int argc, char *argv[]) {
  int sockfd;
  struct sockaddr_in cli_addr, serv_addr;
  socklen_t clilen = sizeof(cli_addr);
  int recvlen;

  // initialize receive buffer
  char *buffer;
  buffer = (char *) malloc(2048);
  memset(buffer, 0, 2048);

  // create ethernet socket
  // use ETH_P_IP will receive all IP packets to this server, along with their Ethernet headers
  sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP)); 
  if (sockfd < 0) {
    error("ERROR socket");
  }
  
  /* set specific interface (OPTIONAL)
  if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, "enp0s8", strlen("enp0s8") + 1) < 0) {
    error("ERROR setting socket option");
  }
  */

  int count = 0;   // used for keeping track of the number of packets received
  while (1) {
    printf("waiting for packet ...\n");
    
    recvlen = recvfrom(sockfd, buffer, 2047, 0, (struct sockaddr *) &cli_addr, &clilen);
    if (recvlen > 0) {
      printf("Packet %d, %d bytes, ",count, recvlen);

      char *packet = skipEthHdr(buffer);  // all packets received contain ethernet headers, skip ethernet headers here
      int protocol = getProtocol(packet); // this function reads the protocol value from the IP header
      printf("Protocol = %d\n", protocol);

      switch (protocol){	
      case 4:
	handleIPinIP(packet);
	break;
      case 17:
	handleUDP(packet);
	break;
      case 89:  // handle OSPF here, haven't test it yet because no OSPF packet
	handleOSPF(packet);
	break;
	
      /*  // forget TCP packets for now, b/c need to deal with TCP handshakes
      case 6:
        handleTCP(packet);
        break;
      */
	
      default:
	printf("Other packet");
      }
	
    }
    memset(buffer, 0 ,2048);    // reset receive buffer
    recvlen = 0;               
    printf("\n");
    count++;
  }

  free(buffer);
  close(sockfd);
  return 0;
}

char* skipEthHdr(char *buffer) {
  // given a pointer to a received packet,
  // this function skips its ethernet header
  // and returns a pointer to the start of its IP header
  
  int ethhdr_len = (int) sizeof(struct ethhdr);
  return buffer + ethhdr_len;
}

int getProtocol(char *packet) {
  // given a pointer to a received packet (Ethernet header included),
  // this function returns the protocol number in the IP header
  struct ip *iphdr = (struct ip *) packet;
  return iphdr->ip_p;
}

void handleUDP(char *packet) {
  // this function handles UDP packet by printing the information it contains
  int ip_len = printIPheader(packet);
  printUDPheader(packet, ip_len);
}

void handleIPinIP(char *packet) {
  // this function handles IP-in-IP packet by printing the information it contains
  
  // print outer header
  int ip_len1 = printIPheader(packet);
  
  int protocol = getProtocol(packet + ip_len1);

  // only handles UDP packets for now, will add others
  if (protocol == 17) {
    handleUDP(packet + ip_len1);
  }
}

void handleOSPF(char *packet) {
  // this function handles OSPF packet
  // for now it just prints a message, haven't test it yet
  printf("This is a OSPF packet\n");
}

void printHex(char *buffer, int recvlen) {
  // this function prints the packet in Hex value
  // for debugging and testing purpose
  
  printf("Here is the Hex format of the packet\n");
  for (int i = 0; i < recvlen; i++) {
    printf("%x ", buffer[i]);
  }
  printf("\n\n");
}

void error(const char *msg) {
  perror(msg);
  exit(1);
}

