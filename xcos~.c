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
#include <sonicmaths/cosine.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	float x_f;
	struct smosc osc;
} t_xcos;

static t_class *xcos_class;

static void *xcos_new(t_symbol *s __attribute__((unused)),
		      int argc, t_atom *argv) {
	int r;
	t_xcos *xcos = (t_xcos *) pd_new(xcos_class);
	r = smosc_init(&xcos->osc);
	if (r != 0) {
		error("Could not initialize xcos~");
		return NULL;
	}
	if (argc > 0) {
		smosc_set_phase(&xcos->osc, (double) atom_getfloat(argv));
		if (argc > 1) {
			error("xcos~ takes only one argument."
			      " Ignoring subsequent arguments.");
		}
	}
	xcos->x_f = 0.0f;
	outlet_new(&xcos->o, &s_signal); /* y */
	signalinlet_new(&xcos->o, 0.0f); /* phase */
	return xcos;
}

static void xcos_free(t_xcos *xcos) {
	smosc_destroy(&xcos->osc);
}

static t_int *xcos_perform(t_int *w) {
	struct smosc *osc = (struct smosc *) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *freq = (float *) w[4];
	float *phase = (float *) w[5];

	smcos(osc, n, y, freq, phase);

	return w + 6;
}

static void xcos_dsp(t_xcos *xcos, t_signal **sp) {
	dsp_add(xcos_perform, 5,
		&xcos->osc, sp[0]->s_n,
		sp[2]->s_vec, sp[0]->s_vec, sp[1]->s_vec);
}

void xcos_tilde_setup() {
	xcos_class = class_new(gensym("xcos~"),
			       (t_newmethod) xcos_new,
			       (t_method) xcos_free,
			       sizeof(t_xcos),
			       CLASS_DEFAULT,
			       A_GIMME, A_NULL);
	class_addmethod(xcos_class, (t_method) xcos_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(xcos_class, t_xcos, x_f); /* freq */
}
