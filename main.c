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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib/shortcuts/shortcuts.h"
#include "myh3c.h"

int main(int argc, const char *argv[])
{
  if (getuid() != 0) {
    fprintf(stderr, "Need root privilege to run link layer level application\n");
    exit(EXIT_FAILURE);
  }

  if (argc < 3) {
    fprintf(stderr, "Usage: ${program} ${username}, ${password}");
    exit(EXIT_FAILURE);
  }

  myh3c_t myh3c = {};
  myh3c_error_t err = myh3c_init(&myh3c, "eth0", argv[1], argv[2]);
  if (err)
    goto main_exit;
  err = myh3c_send_start(&myh3c);
  if (err)
    goto main_exit;
  while (1) {
    struct sockaddr_ll sadr = {};
    socklen_t slen = 0;
    char packet[BUFSIZ] = "";
    recvfrom(myh3c.socket, packet, BUFSIZ, 0, (struct sockaddr *) &sadr, &slen);
    err = myh3c_handle_request(&myh3c, packet);
    if (err)
      goto main_exit;
  }
main_exit:
  myh3c_destroy(&myh3c);
  return EXIT_SUCCESS;
}
