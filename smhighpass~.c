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
#include <sonicmaths/highpass.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	t_sample Q;
	struct sm2order filter;
} t_smhighpass;

t_class *smhighpass_class;

static void *smhighpass_new() {
	t_smhighpass *x = (t_smhighpass *) pd_new(smhighpass_class);
	sm2order_init(&x->filter);
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 0.0f); /* freq */
	floatinlet_new(&x->o, &x->Q); /* Q */
	return x;
}

static void smhighpass_free(t_smhighpass *x) {
	sm2order_destroy(&x->filter);
}

static t_int *smhighpass_perform(t_int *w) {
	t_smhighpass *x = (t_smhighpass *) w[1];
	t_sample *in = (t_sample *) w[2];
	t_sample *freq = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];
	t_sample Q = x->Q;

	while (n--) {
		*out++ = smhighpass(&x->filter, *in++, *freq++, Q);
	}

	return w + 6;
}

static void smhighpass_dsp(t_smhighpass *x, t_signal **sp) {
	dsp_add(smhighpass_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smhighpass_tilde_setup() {
	smhighpass_class = class_new(gensym("smhighpass~"),
				     (t_newmethod) smhighpass_new,
				     (t_method) smhighpass_free,
				     sizeof(t_smhighpass),
				     CLASS_DEFAULT, A_NULL);
	class_addmethod(smhighpass_class,
			(t_method) smhighpass_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smhighpass_class, t_smhighpass, zero); /* input */
}
