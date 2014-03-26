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

#include "myh3c.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "lib/daemon.h"
#include "lib/shortcuts/struct.h"

#define MYH3C_ERRLOG_FNAME "/tmp/MyH3C-ERRLOG"

#if DEBUG_DISPLAY_BINARY_DATA == 1
static
void display_binary_data(const char packet[])
{
  static const size_t col = 6, row = 10;
  size_t i = 0, j;
  for (; i < row; i++) {
    for (j = 0; j < col; j++)
      printf("%#8.2x", (unsigned char) packet[i * col + j]);
    printf("\n");
  }
}
#endif

myh3c_error_t myh3c_init(myh3c_t *myh3c, const char devname[], const char username[],
    const char password[])
{
  strcpy(myh3c->device_name, devname);
  strcpy(myh3c->username, username);
  strcpy(myh3c->password, password);
  strcpy(myh3c->version_info, "\x06\x07""bjQ7SE8BZ3MqHhs3clMregcDY3Y=\x20\x20");
  myh3c->version_info_len = strlen(myh3c->version_info);

  myh3c->socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_H3C));
  if (myh3c->socket == EACCES) {
    errno = 0;
    return kMyH3C_Privilege;
  }
  myh3c->adr.sll_family   = AF_PACKET;
  myh3c->adr.sll_halen    = ETH_ALEN;
  myh3c->adr.sll_hatype   = ARPHRD_ETHER;
  myh3c->adr.sll_ifindex  = if_nametoindex(myh3c->device_name);
  if (errno != 0) {
    errno = 0;
    return kMyH3C_NoDevice;
  }
  myh3c->adr.sll_pkttype  = PACKET_BROADCAST;
  myh3c->adr.sll_protocol = ETH_P_H3C;
  return kMyH3C_Success;
}

static
char *make_ethernet_header(char packet[], const int socket, const char devname[])
{
  static char macaddr[BUFSIZ] = "";
  struct ifreq ifr = {};
  if (!*macaddr) {
    strcpy(ifr.ifr_name, devname);
    if (ioctl(socket, SIOCGIFHWADDR, &ifr))
      perror("Error::ioctl for mac addr");
    memcpy(macaddr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
  }
  memset(packet, 0, ETH_ALEN);
  memcpy(packet + ETH_ALEN, macaddr, ETH_ALEN);
  *(uint16_t *) &(packet[2 * ETH_ALEN]) = htons(ETH_P_H3C);
  return packet + 2 * ETH_ALEN + sizeof(uint16_t);
}

static
char *make_eapol(char *packet, const uint8_t eapol_type, const char payload[],
    const size_t payload_len)
{
  pack("BBH", packet, (unsigned) EAPOL_VERSION, (unsigned) eapol_type,
     htons(payload_len));
  memcpy(packet + 4, payload, payload_len);
  return packet + 4 + payload_len;
}

static
char *make_eapol_and_eap(char *packet, const uint8_t eapol_type,
    const uint8_t code, const uint8_t id, const uint8_t eap_type,
    const char data[], const size_t datalen)
{
  if (code == EAP_SUCCESS || code == EAP_FAILURE) {
    pack("BBHBBH", packet, (unsigned) EAPOL_VERSION, (unsigned) eapol_type,
       htons(4), (unsigned) code, (unsigned) id, htons(4));
    return packet + 8;
  } else {
    pack("BBHBBHB", packet, (unsigned) EAPOL_VERSION, (unsigned) eapol_type,
       htons(5 + datalen), (unsigned) code, (unsigned) id, htons(5 + datalen),
       (unsigned) eap_type);
    memcpy(packet + 9, data, datalen);
    return packet + 9 + datalen;
  }
}

myh3c_error_t myh3c_send_start(const myh3c_t *myh3c)
{
  char packet[BUFSIZ];
  char *const eapol = make_ethernet_header(packet, myh3c->socket, myh3c->device_name);
  char *const eap   = make_eapol(eapol, EAPOL_START, "", 0);
  sendto(myh3c->socket, packet, eap - packet, 0,
      (const struct sockaddr *) &myh3c->adr, sizeof(myh3c->adr));
  perror("Out::send EAPOL_START");
  return errno ? kMyH3C_Failure : kMyH3C_Success;
}

myh3c_error_t myh3c_handle_request(const myh3c_t *myh3c, char packet[])
{
  char *eapol = packet + ETHER_HDR_LEN, *end = NULL;
  uint8_t type = eapol[1];
  if (type != EAPOL_EAPPACKET) {
    fprintf(stderr, "%s\n", "Error::Got unknown EAP type!");
    exit(EXIT_FAILURE);
  }
  uint8_t code = eapol[4], id = eapol[5];

  if (code == EAP_SUCCESS) {
    printf("In ::Got EAP Success\n");
    daemonize(MYH3C_ERRLOG_FNAME);
    return kMyH3C_Success;
  }
  else if (code == EAP_FAILURE) {
    fprintf(stderr, "In ::Got EAP Failure\n");
    return kMyH3C_Failure;
  }
  else if (code == EAP_RESPONSE) {
    printf("In ::Got EAP Response\n");
    return kMyH3C_Success;
  }
  else if (code == EAP_REQUEST) {
    uint8_t reqtype = eapol[8];
    const char *reqdata = &(eapol[9]);
    char data[BUFSIZ] = {'\0'};
    size_t datalen = 0;

    if (reqtype == EAP_REQUEST_TYPE_ID) {
      printf("%s\n", "In ::recv EAP_REQUEST ID");

      memcpy(data, myh3c->version_info, myh3c->version_info_len);
      strcpy(data + myh3c->version_info_len, myh3c->username);
      datalen = myh3c->version_info_len + strlen(myh3c->username);

      eapol = make_ethernet_header(packet, myh3c->socket, myh3c->device_name);
      end = make_eapol_and_eap(eapol, EAPOL_EAPPACKET, EAP_RESPONSE, id,
          EAP_REQUEST_TYPE_ID, data, datalen);
      sendto(myh3c->socket, packet, end - packet, 0,
          (const struct sockaddr *) &myh3c->adr, sizeof(myh3c->adr));
      perror("Out::send EAP_RESPONSE ID");
    }
    else if (reqtype == EAP_REQUEST_TYPE_MD5) {
      printf("%s\n", "In ::recv EAP_REQUEST MD5");

      data[0] = reqdata[0];
      uint8_t i;
      for (i = 0; i < (uint8_t) data[0]; i++) {
        data[i + 1] = (i >= strlen(myh3c->password)) ? '\0'
          : myh3c->password[i];
        data[i + 1] = (unsigned char) data[i + 1] ^ (unsigned char) reqdata[i + 1];
      }
      strcpy(data + (uint8_t) data[0], myh3c->username);
      datalen = (uint8_t) data[0] + strlen(myh3c->username);

      eapol = make_ethernet_header(packet, myh3c->socket, myh3c->device_name);
      end = make_eapol_and_eap(eapol, EAPOL_EAPPACKET, EAP_RESPONSE, id,
          EAP_REQUEST_TYPE_MD5, data, datalen);
      sendto(myh3c->socket, packet, end - packet, 0,
          (struct sockaddr *) &myh3c->adr, sizeof(myh3c->adr));
      perror("Out::send EAP_RESPONSE MD5");
    }
    else {
      fprintf(stderr, "%s\n", "Error::Unknown request type");
      return kMyH3C_Failure;
    }
    return errno ? kMyH3C_Failure : kMyH3C_Success;
  } /* End else-if code == EAP_REQUEST */
  else { 
    return kMyH3C_Success;
  }
}

myh3c_error_t myh3c_destroy(myh3c_t *myh3c)
{
  close(myh3c->socket);
  return kMyH3C_Success;
}
