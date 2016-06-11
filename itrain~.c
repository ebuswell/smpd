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
	struct smosc osc;
} t_itrain;

static t_class *itrain_class;

smpdnew(itrain) {
	int r;
	float f;
	f = smpdfreqarg(0, 0.0f);
	t_itrain *itrain = (t_itrain *) pd_new(itrain_class);
	r = smosc_init(&itrain->osc);
	if (r != 0) {
		error("Could not initialize itrain~");
		return NULL;
	}
	if (argc > 1 && atom_type(argv+1) == A_FLOAT) {
		smosc_set_phase(&itrain->osc, (double) atom_getfloat(argv+1));
	}
	outlet_new(&itrain->o, &s_signal);
	itrain->x_f = f;
	signalinlet_new(&itrain->o, 0.0f);
	return itrain;
}

static void itrain_free(t_itrain *itrain) {
	smosc_destroy(&itrain->osc);
}

static t_int *itrain_perform(t_int *w) {
	struct smosc *osc = (struct smosc *) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *freq = (float *) w[4];
	float *phase = (float *) w[5];

	smitrain(osc, n, y, freq, phase);

	return w + 6;
}

static void itrain_dsp(t_itrain *itrain, t_signal **sp) {
	dsp_add(itrain_perform, 5,
		&itrain->osc, sp[0]->s_n,
		sp[2]->s_vec, sp[0]->s_vec, sp[1]->s_vec);
}

smpddspclass(itrain);
