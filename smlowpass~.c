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
#include <sonicmaths/lowpass.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	t_sample Q;
	struct sm2order filter;
} t_smlowpass;

t_class *smlowpass_class;

static void *smlowpass_new() {
	t_smlowpass *x = (t_smlowpass *) pd_new(smlowpass_class);
	sm2order_init(&x->filter);
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 0.0f); /* freq */
	floatinlet_new(&x->o, &x->Q); /* Q */
	return x;
}

static void smlowpass_free(t_smlowpass *x) {
	sm2order_destroy(&x->filter);
}

static t_int *smlowpass_perform(t_int *w) {
	t_smlowpass *x = (t_smlowpass *) w[1];
	t_sample *in = (t_sample *) w[2];
	t_sample *freq = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];
	t_sample Q = x->Q;

	while (n--) {
		*out++ = smlowpass(&x->filter, *in++, *freq++, Q);
	}

	return w + 6;
}

static void smlowpass_dsp(t_smlowpass *x, t_signal **sp) {
	dsp_add(smlowpass_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smlowpass_tilde_setup() {
	smlowpass_class = class_new(gensym("smlowpass~"),
				    (t_newmethod) smlowpass_new,
				    (t_method) smlowpass_free,
				    sizeof(t_smlowpass),
				    CLASS_DEFAULT, A_NULL);
	class_addmethod(smlowpass_class,
			(t_method) smlowpass_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smlowpass_class, t_smlowpass, zero); /* input */
}
