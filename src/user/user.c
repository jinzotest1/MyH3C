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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define USER_FILE_PATH "user.conf"

#define USER_PATTERN "username: %s password: %s device_name: %s"

static user_t create_user()
{
  FILE *pf = fopen(USER_FILE_PATH, "w");
  user_t user;
  printf("%s:\n", "Enter username");
  scanf("%s", user.username);
  printf("%s:\n", "Enter password");
  scanf("%s", user.password);
  printf("%s:\n",
      "Enter ethernet device name, for newbies, you can simply Enter `eth0' if "
      "you don't know what it means");
  scanf("%s", user.device_name);
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
      user = create_user(pf);
    else
      exit(EXIT_FAILURE);
  }
  return user;
}
