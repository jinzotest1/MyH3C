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
 * Copyright (C) Junyu Wu, 2013
 */

#ifndef LIB_SHORTCUTS_SHORTCUTS_H
#define LIB_SHORTCUTS_SHORTCUTS_H

/*
 * Naming convention:
 * local variable: all lower case
 * global variable: starts with g_, lower case with underline
 * function: underline
 * macro: upper case with underline
 * macro function: Camel
 * constant: starts with k, camel
 * class & class member method: camel
 * class member: lowercase with trailing _
 * class getter / setter: getter all lower case, setter as class member method
 *
 * Only well known and POSIX conventional abbreviations permitted.
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define Chkerr(stmt, prompt) \
  do {                       \
    stmt;                    \
    perror(prompt);          \
    if (errno != 0) {        \
      exit(EXIT_FAILURE);    \
    }                        \
  } while (0)

#define dref(type) *(type *)

#define cdref(type) *(const type *)

#define cast(Type, v) ((Type) o)

#define castp(Type, p) ((Type *) p)

#define ccastp(Type, p) ((const Type *) p)

#define vptradd(vptr, offset) ((char *) vptr + offset)

#define Arrlen(arr) (sizeof(arr) / sizeof(arr[0]))

#define Swap(a, b) {        \
	__typeof__(a) *pa = &(a); \
	__typeof__(b) *pb = &(b); \
	(void) (pa == pb);        \
	__typeof__(a) tmp = *pa;  \
	*pa = *pb;                \
	*pb = tmp;                \
}

#define Max(v1, v2)          \
  ({                         \
   __typeof__ (v1) _v1 = v1; \
   __typeof__ (v2) _v2 = v2; \
   (void) (&_v1 == &_v2);    \
   _v1 > _v2 ? _v1 : _v2;    \
   })

#define Min(v1, v2)          \
  ({                         \
   __typeof__ (v1) _v1 = v1; \
   __typeof__ (v2) _v2 = v2; \
   (void) (&_v1 == &_v2);    \
   _v1 < _v2 ? _v1 : _v2;    \
   })

#endif /* end of include guard: LIB_SHORTCUTS_SHORTCUTS_H */
