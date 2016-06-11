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
} t_quant;

static t_class *quant_class;

smpdnew(quant) {
	float i;
	i = smpdfloatarg(0, 1.0f);
	t_quant *quant = (t_quant *) pd_new(quant_class);
	outlet_new(&quant->o, &s_signal);
	quant->x_f = 0.0f;
	signalinlet_new(&quant->o, i);
	return quant;
}

static void quant_free(t_quant *quant __attribute__((unused))) {
	/* Do nothing */
}

static t_int *quant_perform(t_int *w) {
	int n = w[1];
	float *y = (float *) w[2];
	float *x = (float *) w[3];
	float *i = (float *) w[4];

	smquant(n, y, x, i);

	return w + 5;
}

static void quant_dsp(t_quant *quant __attribute__((unused)), t_signal **sp) {
	dsp_add(quant_perform, 4,
		sp[0]->s_n, sp[2]->s_vec, sp[0]->s_vec, sp[1]->s_vec);
}

smpddspclass(quant);
