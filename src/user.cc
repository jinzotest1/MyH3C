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

#include "user.h"

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <ifaddrs.h>

#define USER_FILE_PATH "/etc/MyH3C/user.conf"

#define USER_PATTERN "username: %s password: %s device_name: %s"

static
user_t create_user()
{
  FILE *pf = fopen(USER_FILE_PATH, "w");
  user_t user;
  printf("%s:\n", "Enter your netid username");
  scanf("%s", user.username);
  printf("%s:\n", "Enter password");
  scanf("%s", user.password);

  struct ifaddrs *adrs = NULL;
  if (0 != getifaddrs(&adrs)) {
    perror("Fail to get network interface list, you might need "
        "select the interface by yourself");
    printf("%s:\n",
        "Enter ethernet device name, for newbies, you can simply "
        "enter `eth0' if you don't know what it means");
    scanf("%s", user.device_name);
  } else {
    perror("Initialize ethernet interface list");
    errno = 0; // FIXME: what caused this?
    struct ifaddrs *cur = NULL;
    for (cur = adrs; cur != NULL; cur = cur->ifa_next) {
      if (cur->ifa_addr->sa_family != AF_PACKET)
        continue;
      printf("AF_PACKET network interface found: %s\n",
          cur->ifa_name);
      if (!strcasestr(cur->ifa_name, "wlan") // Suggested interface
          && !strcasestr(cur->ifa_name, "lo")) {
        sprintf(user.device_name, "%s", cur->ifa_name);
      }
    }
    while (1) {
      if (user.device_name[0] != '\0') {
        printf("Interface %s suggested, are you sure?<Y/N>\n",
            user.device_name);
        char ch;
        scanf(" %c", &ch);
        if ('Y' == toupper(ch))
          break;
      }
      printf("Enter your interface name\n");
      scanf("%s", user.device_name);
    }
  }
  fprintf(pf, USER_PATTERN, user.username, user.password, user.device_name);
  fclose(pf);
  return user;
}

user_t read_user(void)
{
  FILE *pf = fopen(USER_FILE_PATH, "r");
  user_t user = {};
  int scaned = 0;
  if (pf) {
    scaned = fscanf(pf, USER_PATTERN, user.username, user.password,
        user.device_name);
    fclose(pf);
  }
  if (!pf || scaned != 3) {
    fprintf(stderr, "%s\n",
        "No user data exist or user data corrupted, create one?<Y/N>");
    int ch = getchar();
    if (tolower(ch) == 'y')
      user = create_user();
    else
      exit(EXIT_FAILURE);
  }
  return user;
}
