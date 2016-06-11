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
#include <sonicmaths.h>
#include "m_pd.h"
#include "common.h"

typedef struct {
	t_object o;
	float x_f;
} t_f2n;

static t_class *f2n_class;

smpdnew(f2n) {
	float f, root;
	root = smpdfreqarg(0, SMKEYF_C);
	f = smpdfreqarg(1, SMKEYF_C);
	t_f2n *f2n = (t_f2n *) pd_new(f2n_class);
	outlet_new(&f2n->o, &s_signal);
	f2n->x_f = f;
	signalinlet_new(&f2n->o, root);
	return f2n;
}

static void f2n_free(t_f2n *f2n __attribute__((unused))) {
	/* Do nothing */
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

smpddspclass(f2n);
