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
} t_f2n;

static t_class *f2n_class;

static void *f2n_new() {
	t_f2n *f2n = (t_f2n *) pd_new(f2n_class);
	f2n->x_f = 0.0f;
	outlet_new(&f2n->o, &s_signal); /* note */
	signalinlet_new(&f2n->o, smnormfv(sys_getsr(), SMKEYF_C)); /* root */
	return f2n;
}

static t_int *f2n_perform(t_int *w) {
	int n = w[1];
	float *note = (float *) w[2];
	float *freq = (float *) w[3];
	float *root = (float *) w[4];

	smf2n(n, note, freq, root);

	return w + 5;
}

static void f2n_dsp(t_f2n *f2n __attribute__((unused)),
		    t_signal **sp) {
	dsp_add(f2n_perform, 4,
		sp[0]->s_n, sp[2]->s_vec, sp[0]->s_vec, sp[1]->s_vec);
}

void f2n_tilde_setup() {
	f2n_class = class_new(gensym("f2n~"),
			      (t_newmethod) f2n_new,
			      NULL,
			      sizeof(t_f2n),
			      CLASS_DEFAULT, A_NULL);
	class_addmethod(f2n_class, (t_method) f2n_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(f2n_class, t_f2n, x_f); /* freq */
}
