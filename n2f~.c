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
} t_n2f;

static t_class *n2f_class;

smpdnew(n2f) {
	float root;
	float n;
	root = smpdfreqarg(0, SMKEYF_C);
	n = smpdnotearg(1, 0, 0.0f);
	t_n2f *n2f = (t_n2f *) pd_new(n2f_class);
	outlet_new(&n2f->o, &s_signal);
	n2f->x_f = n;
	signalinlet_new(&n2f->o, root);
	return n2f;
}

static void n2f_free(t_n2f *n2f __attribute__((unused))) {
	/* Do nothing */
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

smpddspclass(n2f);
