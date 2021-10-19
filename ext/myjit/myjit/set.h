/*
 * MyJIT
 * Copyright (C) 2015 Petr Krajca, <petr.krajca@upol.cz>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License aint64_t with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SET_H
#define SET_H

#include <assert.h>

#include "jitlib-core.h"
#include "../../../ext/vec/src/vec.h"
static jit_set * jit_set_new()
{
	jit_set * s = JIT_MALLOC(sizeof(vec_int_t));
	vec_init(&s->vec);
	return s;
}

static jit_set * jit_set_clone(jit_set * s)
{
	jit_set * clone = jit_set_new();
	vec_extend(&clone->vec, &s->vec);
	return clone;
}

static void jit_set_free(jit_set * s)
{
	vec_deinit(&s->vec);
	JIT_FREE(s);
}

static int jit_set_get(jit_set * s, int value)
{
	int iter;
	vec_find(&s->vec, value, iter);
	return iter!=-1;
}

static void jit_set_add(jit_set * s, int value)
{
	if(!jit_set_get(s, value))
	vec_push(&s->vec,value);
}

static void jit_set_addall(jit_set * target, jit_set * s)
{
	int iter;
	int v;
	vec_foreach(&s->vec, v, iter) jit_set_add(target,v);
}

static void jit_set_remove(jit_set * s, int value)
{
	vec_remove(&s->vec,value);
}
static int IntCmp(const void*a,const void *b) {
	return *(int*)a-*(int*)b;
}
static int ContainsAll(jit_set * super, jit_set * sub) {
	int v,iter;
	vec_foreach(&super->vec,v,iter) {
		int iter2;
		vec_find(&sub->vec, v, iter2);
		if(iter2==-1) return 0;
	}
	return 1;
}
static  int jit_set_equal(jit_set * s1, jit_set * s2)
{
	return ContainsAll(s1, s2)&&ContainsAll(s2, s1);
}
#endif
