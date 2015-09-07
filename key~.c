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
	float x_f;
	struct smkey *key;
} t_key;

static t_class *key_class;

static void *key_new(t_symbol *s __attribute__((unused)),
		     int argc, t_atom *argv) {
	t_key *key = (t_key *) pd_new(key_class);
	key->key = SMKEY_EQUAL;
	if (argc > 0) {
		t_symbol *arg = atom_getsymbol(argv);
		if (arg == gensym("harmonic")) {
			key->key = SMKEY_HARMONIC;
		} else if (arg == gensym("pythagorean")) {
			key->key = SMKEY_PYTHAGOREAN;
		} else if (arg != gensym("equal")) {
			error("Unknown key specified.");
		}
		if (argc > 1) {
			error("key~ takes only one argument."
			      " Ignoring subsequent arguments.");
		}
	}
	key->x_f = 0.0f;
	outlet_new(&key->o, &s_signal); /* freq */
	signalinlet_new(&key->o, smnormfv(sys_getsr(), SMKEYF_C)); /* root */
	return key;
}

static t_int *key_perform(t_int *w) {
	struct smkey *key = (struct smkey *) w[1];
	int n = w[2];
	float *freq = (float *) w[3];
	float *note = (float *) w[4];
	float *root = (float *) w[5];

	smkey(key, n, freq, note, root);

	return w + 6;
}

static void key_dsp(t_key *key, t_signal **sp) {
	dsp_add(key_perform, 5,
		key->key, sp[0]->s_n,
		sp[2]->s_vec, sp[0]->s_vec, sp[1]->s_vec);
}

void key_tilde_setup() {
	key_class = class_new(gensym("key~"),
			      (t_newmethod) key_new,
			      NULL,
			      sizeof(t_key),
			      CLASS_DEFAULT,
			      A_GIMME, A_NULL);
	class_addmethod(key_class, (t_method) key_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(key_class, t_key, x_f); /* note */
}
