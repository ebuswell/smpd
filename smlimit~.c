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
#include <sonicmaths/limiter.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	enum smlimit_kind kind;
} t_smlimit;

t_class *smlimit_class;

static void *smlimit_new(t_symbol *s __attribute__((unused)), int argc, t_atom *argv) {
	t_smlimit *x = (t_smlimit *) pd_new(smlimit_class);
	x->kind = SMLIMIT_EXP;
	if (argc > 0) {
		t_symbol *arg = atom_getsymbol(argv);
		if (arg == gensym("hyp")
			   || arg == gensym("hyperbolic")) {
			x->kind = SMLIMIT_HYP;
		} else if (arg == gensym("atan")
			   || arg == gensym("arctan")
			   || arg == gensym("arctangent")) {
			x->kind = SMLIMIT_ATAN;
		} else if (arg != gensym("exp")
			   && arg != gensym("exponential")) {
			error("Unknown limit kind specified.");
		}
		if (argc > 1) {
			error("smkey~ takes only one argument."
			      " Ignoring subsequent arguments.");
		}
	}
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 2.0f); /* sharpness */
	return x;
}

static t_int *smlimit_perform(t_int *w) {
	t_smlimit *x = (t_smlimit *) w[1];
	t_sample *in = (t_sample *) w[2];
	t_sample *sharpness = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];

	while (n--) {
		*out++ = smlimit(*in++, x->kind, *sharpness++);
	}

	return w + 6;
}

static void smlimit_dsp(t_smlimit *x, t_signal **sp) {
	dsp_add(smlimit_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smlimit_tilde_setup() {
	smlimit_class = class_new(gensym("smlimit~"),
				  (t_newmethod) smlimit_new,
				  NULL,
				  sizeof(t_smlimit),
				  CLASS_DEFAULT,
				  A_GIMME, A_NULL);
	class_addmethod(smlimit_class,
			(t_method) smlimit_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smlimit_class, t_smlimit, zero); /* input */
}
