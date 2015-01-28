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
#include <stdbool.h>
#include <sonicmaths/math.h>
#include <sonicmaths/lag.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	bool linear;
	struct smlag lag;
} t_smlag;

t_class *smlag_class;

static void *smlag_new(t_symbol *s __attribute__((unused)),
		       int argc, t_atom *argv) {
	t_smlag *x = (t_smlag *) pd_new(smlag_class);
	smlag_init(&x->lag);
	x->linear = false;
	if (argc > 0) {
		t_symbol *arg = atom_getsymbol(argv);
		if (arg == NULL) {
			error("Argument to smlag~ should be"
			      " one of \"lin[ear]\" or"
			      " \"exp[onential]\". Ignoring"
			      " argument.");
		} else {
			if (arg == gensym("linear")
			    || arg == gensym("lin")) {
				x->linear = true;
			} else {
				if (arg != gensym("exponential")
				    && arg != gensym("exp")) {
					error("Argument to smlag~ should be"
					      " one of \"lin[ear]\" or"
					      " \"exp[onential]\". Ignoring"
					      " argument.");
				}
			}
		}
		if (argc > 1) {
			error("smlag~ takes only one argument."
			      " Ignoring subsequent arguments.");
		}
	}
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 0.0f); /* lag */
	return x;
}

static void smlag_free(t_smlag *x) {
	smlag_destroy(&x->lag);
}

static t_int *smlag_perform(t_int *w) {
	t_smlag *x = (t_smlag *) w[1];
	t_sample *in = (t_sample *) w[2];
	t_sample *lag = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];

	if (x->linear) {
		while (n--) {
			*out++ = smlag(&x->lag, *in++, *lag++, true);
		}
	} else {
		while (n--) {
			*out++ = smlag(&x->lag, *in++, *lag++, false);
		}
	}

	return w + 6;
}

static void smlag_dsp(t_smlag *x, t_signal **sp) {
	dsp_add(smlag_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smlag_tilde_setup() {
	smlag_class = class_new(gensym("smlag~"),
				(t_newmethod) smlag_new,
				(t_method) smlag_free,
				sizeof(t_smlag),
				CLASS_DEFAULT,
				A_GIMME, A_NULL);
	class_addmethod(smlag_class, (t_method) smlag_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smlag_class, t_smlag, zero); /* input */
}
