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
#include <sonicmaths/cosine.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	struct smosc osc;
} t_smcos;

t_class *smcos_class;

static void *smcos_new(t_symbol *s __attribute__((unused)), int argc, t_atom *argv) {
	t_smcos *x = (t_smcos *) pd_new(smcos_class);
	smosc_init(&x->osc);
	if (argc > 0) {
		smosc_set_phase(&x->osc, (double) atom_getfloat(argv));
		if (argc > 1) {
			error("smcos~ takes only one argument."
			      " Ignoring subsequent arguments.");
		}
	}
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 0.0f); /* phase */
	return x;
}

static t_int *smcos_perform(t_int *w) {
	t_smcos *x = (t_smcos *) w[1];
	t_sample *freq = (t_sample *) w[2];
	t_sample *phase = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];

	while (n--) {
		*out++ = smcos(&x->osc, *freq++, *phase++);
	}

	return w + 6;
}

static void smcos_dsp(t_smcos *x, t_signal **sp) {
	dsp_add(smcos_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smcos_tilde_setup() {
	smcos_class = class_new(gensym("smcos~"),
				(t_newmethod) smcos_new,
				NULL,
				sizeof(t_smcos),
				CLASS_DEFAULT,
				A_GIMME, A_NULL);
	class_addmethod(smcos_class, (t_method) smcos_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smcos_class, t_smcos, zero); /* freq */
}
