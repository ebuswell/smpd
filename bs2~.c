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
#include <sonicmaths/bandstop2.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	float x_f;
	struct sm2order filter;
} t_bs2;

static t_class *bs2_class;

static void *bs2_new() {
	int r;
	t_bs2 *bs2 = (t_bs2 *) pd_new(bs2_class);
	r = sm2order_init(&bs2->filter);
	if (r != 0) {
		error("Could not initialize bs2~");
		return NULL;
	}
	bs2->x_f = 0.0f;
	outlet_new(&bs2->o, &s_signal);
	signalinlet_new(&bs2->o, 0.0f); /* freq */
	signalinlet_new(&bs2->o, SM2O_BUTTERWORTH_Q); /* Q */
	return bs2;
}

static void bs2_free(t_bs2 *bs2) {
	sm2order_destroy(&bs2->filter);
}

static t_int *bs2_perform(t_int *w) {
	struct sm2order *filter = (struct sm2order *) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *x = (float *) w[4];
	float *f = (float *) w[5];
	float *Q = (float *) w[6];

	smbandstop2(filter, n, y, x, f, Q);

	return w + 7;
}

static void bs2_dsp(t_bs2 *bs2, t_signal **sp) {
	dsp_add(bs2_perform, 6,
		&bs2->filter, sp[0]->s_n,
		sp[3]->s_vec, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec);
}

void bs2_tilde_setup() {
	bs2_class = class_new(gensym("bs2~"),
			      (t_newmethod) bs2_new,
			      (t_method) bs2_free,
			      sizeof(t_bs2),
			      CLASS_DEFAULT, A_NULL);
	class_addmethod(bs2_class,
			(t_method) bs2_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(bs2_class, t_bs2, x_f); /* x */
}
