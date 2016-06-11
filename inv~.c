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
} t_inv;

static t_class *inv_class;

smpdnew(inv) {
	smpdnoargs();
	t_inv *inv = (t_inv *) pd_new(inv_class);
	outlet_new(&inv->o, &s_signal);
	inv->x_f = 1.0f;
	return inv;
}

static void inv_free(t_inv *inv __attribute__((unused))) {
	/* Do nothing */
}

static t_int *inv_perform(t_int *w) {
	int n = w[1];
	float *y = (float *) w[2];
	float *x = (float *) w[3];

	while (n--) {
		y[n] = 1.0f / x[n];
	}

	return w + 4;
}

static void inv_dsp(t_inv *inv __attribute__((unused)), t_signal **sp) {
	dsp_add(inv_perform, 3, sp[0]->s_n, sp[1]->s_vec, sp[0]->s_vec);
}

smpddspclass(inv);
