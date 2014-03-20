/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) Junyu Wu, shibuyanorailgun@gmail.com, 2014
 */

#define DEBUG_DISPLAY_BINARY_DATA 0

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "lib/shortcuts/shortcuts.h"

#define ETH_P_H3C         0X888E

#define ETHERTYPE_PAE     0x888e
#define EAPOL_VERSION     1
#define EAPOL_EAPPACKET   0

#define EAPOL_START       1
#define EAPOL_LOGOFF      2
#define EAPOL_KEY         3
#define EAPOL_ASF         4

#define EAP_REQUEST       1
#define EAP_RESPONSE      2
#define EAP_SUCCESS       3
#define EAP_FAILURE       4

#define EAP_TYPE_ID       1
#define EAP_TYPE_MD5      4
#define EAP_TYPE_H3C      7

void display_binary_data(const char packet[])
{
  size_t i = 0, j;
  const size_t col = 6, row = 10;
  for (; i < row; i++) {
    for (j = 0; j < col; j++) {
      printf("%#8.2x", (unsigned char) packet[i * col + j]);
    }
    printf("\n");
  }
}

typedef struct {
  char username[BUFSIZ], password[BUFSIZ];
  char device_name[BUFSIZ];
  char version_info[BUFSIZ];
  size_t version_info_len;
  int socket;
  struct sockaddr_ll adr;
} eap_auth_t;

char *make_ethernet_header(char packet[], const int socket, const char devname[])
{
  static char macaddr[BUFSIZ] = "";
  struct ifreq ifr = {};
  if (!strcmp("", macaddr)) {
    strcpy(ifr.ifr_name, devname);
    if (ioctl(socket, SIOCGIFHWADDR, &ifr))
      perror("ioctl for mac addr");
    memcpy(macaddr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
  }
  memcpy(packet, "\x01\x80\xc2\x00\x00\x03", ETH_ALEN);
  memcpy(packet + ETH_ALEN, macaddr, ETH_ALEN);
  *(uint16_t *) &(packet[2 * ETH_ALEN]) = htons(ETH_P_H3C);
  return packet + 2 * ETH_ALEN + sizeof(uint16_t);
}

char *make_eapol(char *packet, const uint8_t type, const char payload[],
    const size_t payload_len)
{
  *(uint8_t *)  &(packet[0]) = EAPOL_VERSION;
  *(uint8_t *)  &(packet[1]) = type;
  *(uint16_t *) &(packet[2]) = htons(payload_len);
  memcpy(packet + 4, payload, payload_len);
  return packet + 4 + payload_len;
}

char *make_eap(char packet[], const uint8_t code, const uint8_t id,
    const uint8_t type, const char *data, const size_t datalen)
{
  *(uint8_t *) &(packet[0]) = code;
  *(uint8_t *) &(packet[1]) = id;
  if (code == EAP_SUCCESS || code == EAP_FAILURE) {
    *(uint16_t *) &(packet[2]) = htons(4);
  } else {
    *(uint16_t *) &(packet[2]) = htons(5 + datalen);
    *(uint8_t *)  &(packet[4]) = type;
    memcpy(packet + 5, data, datalen);
  }
  return packet + 5 + datalen;
}

char *make_eapol_and_eap(char *packet, const uint8_t eapol_type,
    const uint8_t code, const uint8_t id, const uint8_t eap_type,
    const char data[], const size_t datalen)
{
  packet[0] = EAPOL_VERSION; // make EAPOL
  packet[1] = eapol_type;
  packet[4] = code; // make EAP
  packet[5] = id;
  if (code == EAP_SUCCESS || code == EAP_FAILURE) {
    *(uint16_t *) &(packet[2]) = htons(4); // EAPOL
    *(uint16_t *) &(packet[6]) = htons(4); // EAP
    return packet + 8;
  } else {
    *(uint16_t *) &(packet[2]) = htons(5 + datalen); // EAPOL
    *(uint16_t *) &(packet[6]) = htons(5 + datalen); // EAP
    packet[8] = eap_type;
    memcpy(packet + 9, data, datalen);
    return packet + 9 + datalen;
  }
}

void eap_auth_handle(char packet[], const eap_auth_t *eapauth)
{
  char *eapol = packet + ETHER_HDR_LEN, *end = NULL;
  uint8_t /* vers = eapol[0], */ type = eapol[1];
  // uint16_t eapol_len = ntohs(*(uint16_t *) &(eapol[2]));
  if (type != EAPOL_EAPPACKET)
    fprintf(stderr, "%s\n", "Got unknown EAP type!");
  uint8_t code = eapol[4], id = eapol[5];
  // uint16_t eap_len = ntohs(*(uint16_t *) &(eapol[6]));

  if (code == EAP_SUCCESS) {
    printf("Got EAP Success\n");
  }
  else if (code == EAP_FAILURE) {
    printf("Got EAP Failure\n");
  }
  else if (code == EAP_RESPONSE) {
    printf("Got EAP Response\n");
  }
  else if (code == EAP_REQUEST) {
    uint8_t reqtype = eapol[8];
    const char *reqdata = &(eapol[9]);
    char data[BUFSIZ] = {'\0'};
    size_t datalen = 0;

    if (reqtype == EAP_TYPE_ID) { // Send response for identity
      printf("%s\n", "Got EAP request for identity");

      memcpy(data, eapauth->version_info, eapauth->version_info_len);
      strcpy(data + eapauth->version_info_len, eapauth->username);
      datalen = eapauth->version_info_len + strlen(eapauth->username);

      eapol = make_ethernet_header(packet, eapauth->socket, eapauth->device_name);
      end = make_eapol_and_eap(eapol, EAPOL_EAPPACKET, EAP_RESPONSE, id,
          EAP_TYPE_ID, data, datalen);
      display_binary_data(packet);
      sendto(eapauth->socket, packet, end - packet, 0,
          (const struct sockaddr *) &eapauth->adr, sizeof(eapauth->adr));
      perror("sendto EAP response for identity");
    }
    else if (reqtype == EAP_TYPE_MD5) {
      printf("%s\n", "Got EAP request for MD5");

      size_t i;
      data[0] = 16;
      // printf("md5data length: %d\n", reqdata[0]);
      for (i = 0; i < 16; i++) {
        data[i + 1] = (i >= strlen(eapauth->password)) ? '\0'
          : eapauth->password[i];
        *(unsigned char *) &(data[i + 1]) =
          ((unsigned char) (data[i + 1])) ^ ((unsigned char) (reqdata[i + 1]));
        // printf("%#4.2x ", (unsigned char) data[i + 1]);
      }
      // printf("\n");
      strcpy(data + 17, eapauth->username);
      datalen = 17 + strlen(eapauth->username);

      eapol = make_ethernet_header(packet, eapauth->socket, eapauth->device_name);
      // display_binary_data(packet);
      // printf("End\n");
      end = make_eapol_and_eap(eapol, EAPOL_EAPPACKET, EAP_RESPONSE, id,
          EAP_TYPE_MD5, data, datalen);
      // display_binary_data(packet);
      sendto(eapauth->socket, packet, end - packet, 0,
          (struct sockaddr *) &eapauth->adr, sizeof(eapauth->adr));
      perror("sendto EAP response for MD5");
    }
    else {
      fprintf(stderr, "%s\n", "Unknown request type");
      return;
    }
  }
}

int main(int argc, const char *argv[])
{
  if (getuid() != 0) {
    printf("Need root privilege to run link layer level application\n");
    exit(EXIT_FAILURE);
  }
  char packet[BUFSIZ] = "";
  eap_auth_t eapauth = {
    .username = "",
    .password = "",
    .device_name = "eth0",
    .version_info = "\x06\x07""bjQ7SE8BZ3MqHhs3clMregcDY3Y=\x20\x20"
  };
  eapauth.version_info_len = strlen(eapauth.version_info);
  eapauth.socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_H3C));
  eapauth.adr.sll_family   = AF_PACKET;
  eapauth.adr.sll_halen    = ETH_ALEN;
  eapauth.adr.sll_hatype   = ARPHRD_ETHER;
  eapauth.adr.sll_ifindex  = if_nametoindex(eapauth.device_name);
  eapauth.adr.sll_pkttype  = PACKET_BROADCAST;
  eapauth.adr.sll_protocol = ETH_P_H3C;

  char *const eapol = make_ethernet_header(packet, eapauth.socket, eapauth.device_name);
  char *const eap   = make_eapol(eapol, EAPOL_START, "", 0);
  sendto(eapauth.socket, packet, eap - packet, 0,
      (const struct sockaddr *) &eapauth.adr, sizeof(eapauth.adr));
  perror("sendto EAPOL_START");
  while (1) {
    struct sockaddr_ll sadr = {};
    socklen_t slen = 0;
    memset(packet, 0, sizeof(packet));
    recvfrom(eapauth.socket, packet, BUFSIZ, 0, (struct sockaddr *) &sadr, &slen);
    perror("recv");
#if DEBUG_DISPLAY_BINARY_DATA == 1
    display_binary_data(packet);
#endif
    eap_auth_handle(packet, &eapauth);
  }
  return EXIT_SUCCESS;
}
