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

#include "./array.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_INIT_DEFAULT_CAPACITY 16

#define MTL_OUTOFMEMORY 1

int array_init(array_t *parr, const size_t bsize)
{
	allocator_init(&(parr->alloc));
	parr->vptr = allocator_alloc(&(parr->alloc),
			parr->capacity = ARRAY_INIT_DEFAULT_CAPACITY * bsize);
	if (NULL == parr->vptr)
		return MTL_OUTOFMEMORY;
	parr->bsize = bsize, parr->nitem = 0;
	return 0;
}

void array_destroy(array_t *parr)
{
	allocator_free(&(parr->alloc), parr->vptr);
	allocator_destroy(&(parr->alloc));
}

void *array_at(array_t *parr, const size_t idx)
{
	return parr->vptr + parr->bsize * idx;
}

void *array_front(array_t *parr)
{
	return parr->vptr;
}

void *array_back(array_t *parr)
{
	return array_at(parr, parr->nitem - 1);
}

void *array_append(array_t *parr, void *pitem)
{
	size_t bsize = parr->bsize, offset = parr->bsize * parr->nitem;
	if (parr->nitem >= parr->capacity)
		parr->vptr = realloc(parr->vptr, parr->capacity *= 2);
	memcpy(parr->vptr + offset, pitem, bsize);
	return parr->vptr + bsize * parr->nitem++;
}

void *array_find(array_t *parr, const void *pitem)
{
	for (size_t i = 0; i < parr->nitem; i++) {
		if (!memcmp(array_at(parr, i), pitem, parr->bsize))
			return array_at(parr, i);
	}
	return NULL;
}
