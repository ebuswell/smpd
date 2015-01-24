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
#include <sonicmaths/bandpass.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	t_sample Q;
	struct sm2order filter;
} t_smbandpass;

t_class *smbandpass_class;

static void *smbandpass_new() {
	t_smbandpass *x = (t_smbandpass *) pd_new(smbandpass_class);
	sm2order_init(&x->filter);
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 0.0f); /* freq */
	floatinlet_new(&x->o, &x->Q); /* Q */
	return x;
}

static void smbandpass_free(t_smbandpass *x) {
	sm2order_destroy(&x->filter);
}

static t_int *smbandpass_perform(t_int *w) {
	t_smbandpass *x = (t_smbandpass *) w[1];
	t_sample *in = (t_sample *) w[2];
	t_sample *freq = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];
	t_sample Q = x->Q;

	while (n--) {
		*out++ = smbandpass(&x->filter, *in++, *freq++, Q);
	}

	return w + 6;
}

static void smbandpass_dsp(t_smbandpass *x, t_signal **sp) {
	dsp_add(smbandpass_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smbandpass_tilde_setup() {
	smbandpass_class = class_new(gensym("smbandpass~"),
				     (t_newmethod) smbandpass_new,
				     (t_method) smbandpass_free,
				     sizeof(t_smbandpass),
				     CLASS_DEFAULT, A_NULL);
	class_addmethod(smbandpass_class,
			(t_method) smbandpass_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smbandpass_class, t_smbandpass, zero); /* input */
}
