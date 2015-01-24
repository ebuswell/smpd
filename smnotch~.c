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
#include <sonicmaths/notch.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	t_sample Q;
	struct sm2order filter;
} t_smnotch;

t_class *smnotch_class;

static void *smnotch_new() {
	t_smnotch *x = (t_smnotch *) pd_new(smnotch_class);
	sm2order_init(&x->filter);
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 0.0f); /* freq */
	floatinlet_new(&x->o, &x->Q); /* Q */
	return x;
}

static void smnotch_free(t_smnotch *x) {
	sm2order_destroy(&x->filter);
}

static t_int *smnotch_perform(t_int *w) {
	t_smnotch *x = (t_smnotch *) w[1];
	t_sample *in = (t_sample *) w[2];
	t_sample *freq = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];
	t_sample Q = x->Q;

	while (n--) {
		*out++ = smnotch(&x->filter, *in++, *freq++, Q);
	}

	return w + 6;
}

static void smnotch_dsp(t_smnotch *x, t_signal **sp) {
	dsp_add(smnotch_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smnotch_tilde_setup() {
	smnotch_class = class_new(gensym("smnotch~"),
				  (t_newmethod) smnotch_new,
				  (t_method) smnotch_free,
				  sizeof(t_smnotch),
				  CLASS_DEFAULT, A_NULL);
	class_addmethod(smnotch_class,
			(t_method) smnotch_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smnotch_class, t_smnotch, zero); /* input */
}
