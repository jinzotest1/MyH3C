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

#ifndef MTL_ARRAY_H
#define MTL_ARRAY_H

#include <stddef.h>

#include "./allocator.h"

typedef struct array {
	void *vptr;
	size_t bsize;
	size_t nitem, capacity;
	allocator_t alloc;
} array_t;

int array_init(array_t *parr, const size_t bsize);

void array_destroy(array_t *parr);

void *array_at(array_t *parr, const size_t idx);

void *array_front(array_t *parr);

void *array_back(array_t *parr);

void *array_append(array_t *parr, void *pitem);

void *array_find(array_t *parr, const void *pitem);

#endif /* end of include guard: MTL_ARRAY_H */
