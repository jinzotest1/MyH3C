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

#include "./struct.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

char *pack(const char fm[], char buf[], ...)
{
	va_list vl;
	va_start(vl, buf);

	size_t i, j = 0;
	for (i = 0; fm[i] != '\0'; i++) {
		switch (fm[i]) {
			/* padding */
			case 'X':
				buf[j] = '\0';
				j += sizeof(uint8_t);
				break;

				/* byte */
			case 'B':
				buf[j] = va_arg(vl, uint32_t);
				j += sizeof(uint8_t);
				break;

				/* 16 bit short */
			case 'H':
				*(uint16_t *) (buf + j) = va_arg(vl, uint32_t);
				j += sizeof(uint16_t);
				break;

				/* 32 bit int */
			case 'I':
				*(uint32_t *) (buf + j) = va_arg(vl, uint32_t);
				j += sizeof(uint32_t);
				break;

			default:
				va_end(vl);
				return NULL;
		}
	}
	va_end(vl);
	return buf;
}
