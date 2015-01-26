/*
 * Copyright 2015 Evan Buswell
 * 
 * This file is part of smpd.
 * 
 * smpd is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2.
 * 
 * smpd is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with smpd.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stddef.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
} t_sminv;

t_class *sminv_class;

static void *sminv_new() {
	t_sminv *x = (t_sminv *) pd_new(sminv_class);
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	return x;
}

static t_int *sminv_perform(t_int *w) {
	t_sample *in = (t_sample *) w[1];
	t_sample *out = (t_sample *) w[2];
	t_int n = w[3];

	while (n--) {
		*out++ = 1.0f / *in++;
	}

	return w + 4;
}

static void sminv_dsp(t_sminv *x __attribute__((unused)), t_signal **sp) {
	dsp_add(sminv_perform, 3,
		sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

void sminv_tilde_setup() {
	sminv_class = class_new(gensym("sminv~"),
				(t_newmethod) sminv_new,
				NULL,
				sizeof(t_sminv),
				CLASS_DEFAULT, A_NULL);
	class_addmethod(sminv_class, (t_method) sminv_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(sminv_class, t_sminv, zero); /* input */
}
