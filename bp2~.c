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
#include <sonicmaths/bandpass2.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	float x_f;
	struct sm2order filter;
} t_bp2;

static t_class *bp2_class;

static void *bp2_new() {
	int r;
	t_bp2 *bp2 = (t_bp2 *) pd_new(bp2_class);
	r = sm2order_init(&bp2->filter);
	if (r != 0) {
		error("Could not initialize bp2~");
		return NULL;
	}
	bp2->x_f = 0.0f;
	outlet_new(&bp2->o, &s_signal);
	signalinlet_new(&bp2->o, 0.0f); /* freq */
	signalinlet_new(&bp2->o, SM2O_BUTTERWORTH_Q); /* Q */
	return bp2;
}

static void bp2_free(t_bp2 *bp2) {
	sm2order_destroy(&bp2->filter);
}

static t_int *bp2_perform(t_int *w) {
	struct sm2order *filter = (struct sm2order *) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *x = (float *) w[4];
	float *f = (float *) w[5];
	float *Q = (float *) w[6];

	smbandpass2(filter, n, y, x, f, Q);

	return w + 7;
}

static void bp2_dsp(t_bp2 *bp2, t_signal **sp) {
	dsp_add(bp2_perform, 6,
		&bp2->filter, sp[0]->s_n,
		sp[3]->s_vec, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec);
}

void bp2_tilde_setup() {
	bp2_class = class_new(gensym("bp2~"),
			      (t_newmethod) bp2_new,
			      (t_method) bp2_free,
			      sizeof(t_bp2),
			      CLASS_DEFAULT, A_NULL);
	class_addmethod(bp2_class,
			(t_method) bp2_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(bp2_class, t_bp2, x_f); /* x */
}
