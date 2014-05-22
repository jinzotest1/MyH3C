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

#ifndef MYH3C_H
#define MYH3C_H

#define DEBUG_DISPLAY_BINARY_DATA 0

#include <cstdio>

#include <netpacket/packet.h>

#define ETH_P_H3C             0x888E
#define EAPOL_VERSION         1
#define EAPOL_EAPPACKET       0
#define EAPOL_START           1
#define EAP_REQUEST           1
#define EAP_RESPONSE          2
#define EAP_SUCCESS           3
#define EAP_FAILURE           4
#define EAP_REQUEST_TYPE_ID   1
#define EAP_REQUEST_TYPE_MD5  4

static const size_t USERNAME_MAXLEN = 32;

static const size_t PASSWORD_MAXLEN = 32;

static const size_t DEVICE_NAME_MAXLEN = 32;

static const size_t VERSION_INFO_MAXLEN = 4000;

typedef struct {
  char username[USERNAME_MAXLEN];
  char password[PASSWORD_MAXLEN];
  char device_name[DEVICE_NAME_MAXLEN];
  char version_info[VERSION_INFO_MAXLEN];
  size_t version_info_len;
  int socket;
  struct sockaddr_ll adr;
} myh3c_t;

typedef enum myh3c_error {
  kMyH3C_Success, kMyH3C_Failure, kMyH3C_Privilege, kMyH3C_NoDevice
} myh3c_error_t;

myh3c_error_t myh3c_init(myh3c_t *myh3c, const char devname[], const char username[],
    const char password[]);

myh3c_error_t myh3c_send_start(const myh3c_t *myh3c);

myh3c_error_t myh3c_handle_request(const myh3c_t *myh3c, unsigned char packet[]);

myh3c_error_t myh3c_destroy(myh3c_t *myh3c);

#endif /* end of include guard: MYH3C_H */
