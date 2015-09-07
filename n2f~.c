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
#include <sonicmaths/math.h>
#include <sonicmaths/key.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	float x_f;
} t_n2f;

static t_class *n2f_class;

static void *n2f_new() {
	t_n2f *n2f = (t_n2f *) pd_new(n2f_class);
	n2f->x_f = 0.0f;
	outlet_new(&n2f->o, &s_signal); /* freq */
	signalinlet_new(&n2f->o, smnormfv(sys_getsr(), SMKEYF_C)); /* root */
	return n2f;
}

static t_int *n2f_perform(t_int *w) {
	int n = w[1];
	float *freq = (float *) w[2];
	float *note = (float *) w[3];
	float *root = (float *) w[4];

	smn2f(n, freq, note, root);

	return w + 5;
}

static void n2f_dsp(t_n2f *n2f __attribute__((unused)),
		    t_signal **sp) {
	dsp_add(n2f_perform, 4,
		sp[0]->s_n, sp[2]->s_vec, sp[0]->s_vec, sp[1]->s_vec);
}

void n2f_tilde_setup() {
	n2f_class = class_new(gensym("n2f~"),
			      (t_newmethod) n2f_new,
			      NULL,
			      sizeof(t_n2f),
			      CLASS_DEFAULT, A_NULL);
	class_addmethod(n2f_class, (t_method) n2f_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(n2f_class, t_n2f, x_f); /* note */
}
