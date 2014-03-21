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

#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int daemonize(void)
{
  pid_t pid = fork();
  if (pid < 0)
    goto daemonize_exit;
  else if (pid > 0)
    exit(EXIT_SUCCESS);
  umask(0);
  pid_t sid = setsid();
  if (sid < 0 || chdir("/") < 0)
    goto daemonize_exit;
daemonize_exit:
  return pid;
}
