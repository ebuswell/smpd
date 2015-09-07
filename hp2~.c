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
#include <sonicmaths/second-order.h>
#include <sonicmaths/highpass2.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	float x_f;
	struct sm2order filter;
} t_hp2;

static t_class *hp2_class;

static void *hp2_new() {
	int r;
	t_hp2 *hp2 = (t_hp2 *) pd_new(hp2_class);
	r = sm2order_init(&hp2->filter);
	if (r != 0) {
		error("Could not initialize hp2~");
		return NULL;
	}
	hp2->x_f = 0.0f;
	outlet_new(&hp2->o, &s_signal);
	signalinlet_new(&hp2->o, 0.0f); /* freq */
	signalinlet_new(&hp2->o, SM2O_BUTTERWORTH_Q); /* Q */
	return hp2;
}

static void hp2_free(t_hp2 *hp2) {
	sm2order_destroy(&hp2->filter);
}

static t_int *hp2_perform(t_int *w) {
	struct sm2order *filter = (struct sm2order *) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *x = (float *) w[4];
	float *f = (float *) w[5];
	float *Q = (float *) w[6];

	smhighpass2(filter, n, y, x, f, Q);

	return w + 7;
}

static void hp2_dsp(t_hp2 *hp2, t_signal **sp) {
	dsp_add(hp2_perform, 6,
		&hp2->filter, sp[0]->s_n,
		sp[3]->s_vec, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec);
}

void hp2_tilde_setup() {
	hp2_class = class_new(gensym("hp2~"),
			      (t_newmethod) hp2_new,
			      (t_method) hp2_free,
			      sizeof(t_hp2),
			      CLASS_DEFAULT, A_NULL);
	class_addmethod(hp2_class,
			(t_method) hp2_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(hp2_class, t_hp2, x_f); /* x */
}
