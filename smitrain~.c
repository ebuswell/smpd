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
#include <sonicmaths/impulse-train.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	struct smosc osc;
} t_smitrain;

t_class *smitrain_class;

static void *smitrain_new(t_symbol *s __attribute__((unused)), int argc, t_atom *argv) {
	t_smitrain *x = (t_smitrain *) pd_new(smitrain_class);
	smosc_init(&x->osc);
	if (argc > 0) {
		smosc_set_phase(&x->osc, (double) atom_getfloat(argv));
		if (argc > 1) {
			error("smitrain~ takes only one argument."
			      " Ignoring subsequent arguments.");
		}
	}
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 0.0f); /* phase */
	return x;
}

static void smitrain_free(t_smitrain *x) {
	smosc_destroy(&x->osc);
}

static t_int *smitrain_perform(t_int *w) {
	t_smitrain *x = (t_smitrain *) w[1];
	t_sample *freq = (t_sample *) w[2];
	t_sample *phase = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];

	while (n--) {
		*out++ = smitrain(&x->osc, *freq++, *phase++);
	}

	return w + 6;
}

static void smitrain_dsp(t_smitrain *x, t_signal **sp) {
	dsp_add(smitrain_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smitrain_tilde_setup() {
	smitrain_class = class_new(gensym("smitrain~"),
				(t_newmethod) smitrain_new,
				(t_method) smitrain_free,
				sizeof(t_smitrain),
				CLASS_DEFAULT,
				A_GIMME, A_NULL);
	class_addmethod(smitrain_class,
			(t_method) smitrain_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smitrain_class, t_smitrain, zero); /* freq */
}
