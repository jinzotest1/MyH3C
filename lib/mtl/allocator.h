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

#ifndef MTL_ALLOCATOR_H
#define MTL_ALLOCATOR_H

#include <stddef.h>

typedef struct {
} allocator_t;

int allocator_init(allocator_t *);

int allocator_destroy(allocator_t *);

void *allocator_alloc(allocator_t *, const size_t);

void allocator_free(allocator_t *, void *);

#endif /* end of include guard: MTL_ALLOCATOR_H */
