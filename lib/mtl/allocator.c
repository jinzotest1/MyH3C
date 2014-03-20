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
 * Copyright (C) Junyu Wu, 2014
 */

#include "allocator.h"

#include <stdlib.h>

int allocator_init(allocator_t *allocator)
{
  (void) allocator;
  return 0;
}

int allocator_destroy(allocator_t *allocator)
{
  (void) allocator;
  return 0;
}


void *allocator_alloc(allocator_t *allocator, const size_t sz)
{
  (void) allocator;
	return malloc(sz);
}

void allocator_free(allocator_t *allocator, void *ptr)
{
  (void) allocator;
	free(ptr);
}
