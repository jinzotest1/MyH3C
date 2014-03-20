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

#ifndef MTL_STACK_H
#define MTL_STACK_H

#include "./array.h"

typedef array_t stack_t;

#define stack_init array_init

#define stack_destroy array_destroy

#define stack_push array_append

#endif /* end of include guard: MTL_STACK_H */
