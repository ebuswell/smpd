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
} t_gaussian;

static t_class *gaussian_class;

smpdnew(gaussian) {
	smpdnoargs();
	t_gaussian *gaussian = (t_gaussian *) pd_new(gaussian_class);
	outlet_new(&gaussian->o, &s_signal);
	return gaussian;
}

static void gaussian_free(t_gaussian *gaussian __attribute__((unused))) {
	/* Do nothing */
}

static t_int *gaussian_perform(t_int *w) {
	int n = w[1];
	float *y = (float *) w[2];

	smrand_gaussian(n, y);

	return w + 3;
}

static void gaussian_dsp(t_gaussian *gaussian __attribute__((unused)),
			 t_signal **sp) {
	dsp_add(gaussian_perform, 2,
		sp[0]->s_n, sp[0]->s_vec);
}

smpddspclassnoin(gaussian);
