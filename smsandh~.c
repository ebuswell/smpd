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
#include <sonicmaths/sample-and-hold.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	struct smsandh sandh;
} t_smsandh;

t_class *smsandh_class;

static void *smsandh_new() {
	t_smsandh *x = (t_smsandh *) pd_new(smsandh_class);
	smsandh_init(&x->sandh);
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 0.0f); /* t */
	return x;
}

static void smsandh_free(t_smsandh *x) {
	smsandh_destroy(&x->sandh);
}

static t_int *smsandh_perform(t_int *w) {
	t_smsandh *x = (t_smsandh *) w[1];
	t_sample *in = (t_sample *) w[2];
	t_sample *t = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];

	while (n--) {
		*out++ = smsandh(&x->sandh, *in++, *t++);
	}

	return w + 6;
}

static void smsandh_dsp(t_smsandh *x, t_signal **sp) {
	dsp_add(smsandh_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smsandh_tilde_setup() {
	smsandh_class = class_new(gensym("smsandh~"),
				  (t_newmethod) smsandh_new,
				  (t_method) smsandh_free,
				  sizeof(t_smsandh),
				  CLASS_DEFAULT, A_NULL);
	class_addmethod(smsandh_class, (t_method) smsandh_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smsandh_class, t_smsandh, zero); /* input */
}
