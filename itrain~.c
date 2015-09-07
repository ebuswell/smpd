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
#include <sonicmaths/oscillator.h>
#include <sonicmaths/impulse-train.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	float x_f;
	struct smosc osc;
} t_itrain;

static t_class *itrain_class;

static void *itrain_new(t_symbol *s __attribute__((unused)),
			int argc, t_atom *argv) {
	int r;
	t_itrain *itrain = (t_itrain *) pd_new(itrain_class);
	r = smosc_init(&itrain->osc);
	if (r != 0) {
		error("Could not initialize itrain~");
		return NULL;
	}
	if (argc > 0) {
		smosc_set_phase(&itrain->osc, (double) atom_getfloat(argv));
		if (argc > 1) {
			error("itrain~ takes only one argument."
			      " Ignoring subsequent arguments.");
		}
	}
	itrain->x_f = 0.0f;
	outlet_new(&itrain->o, &s_signal); /* y */
	signalinlet_new(&itrain->o, 0.0f); /* phase */
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

void itrain_tilde_setup() {
	itrain_class = class_new(gensym("itrain~"),
				 (t_newmethod) itrain_new,
				 (t_method) itrain_free,
				 sizeof(t_itrain),
				 CLASS_DEFAULT,
				 A_GIMME, A_NULL);
	class_addmethod(itrain_class,
			(t_method) itrain_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(itrain_class, t_itrain, x_f); /* freq */
}
