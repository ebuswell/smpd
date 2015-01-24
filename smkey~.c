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
#include <sonicmaths/math.h>
#include <sonicmaths/key.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	struct smkey *key;
} t_smkey;

t_class *smkey_class;

static void *smkey_new(t_symbol *s __attribute__((unused)), int argc, t_atom *argv) {
	t_smkey *x = (t_smkey *) pd_new(smkey_class);
	x->key = SMKEY_EQUAL;
	if (argc > 0) {
		t_symbol *arg = atom_getsymbol(argv);
		if (arg == gensym("major")) {
			x->key = SMKEY_MAJOR;
		} else if (arg == gensym("minor")) {
			x->key = SMKEY_MINOR;
		} else if (arg == gensym("pythagorean")) {
			x->key = SMKEY_PYTHAGOREAN;
		} else if (arg != gensym("equal")) {
			error("Unknown key specified.");
		}
		if (argc > 1) {
			error("smkey~ takes only one argument."
			      " Ignoring subsequent arguments.");
		}
	}
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, smaths_normfreq(sys_getsr(), SMKEY_C)); /* root */
	return x;
}

static t_int *smkey_perform(t_int *w) {
	t_smkey *x = (t_smkey *) w[1];
	t_sample *note = (t_sample *) w[2];
	t_sample *root = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];

	while (n--) {
		*out++ = smkey(x->key, *root++, *note++);
	}

	return w + 6;
}

static void smkey_dsp(t_smkey *x, t_signal **sp) {
	dsp_add(smkey_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smkey_tilde_setup() {
	smkey_class = class_new(gensym("smkey~"),
				(t_newmethod) smkey_new,
				NULL,
				sizeof(t_smkey),
				CLASS_DEFAULT,
				A_GIMME, A_NULL);
	class_addmethod(smkey_class, (t_method) smkey_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smkey_class, t_smkey, zero); /* note */
}
