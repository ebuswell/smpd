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
	struct smkey *key;
} t_key;

static t_class *key_class;

smpdnew(key) {
	float n, root;
	root = smpdfreqarg(1, SMKEYF_C);
	n = smpdnotearg(2, 1, 0.0f);
	t_key *key = (t_key *) pd_new(key_class);
	key->key = SMKEY_EQUAL;
	if (argc > 0) {
		t_symbol *arg = atom_getsymbol(argv+0);
		if (arg == gensym("harmonic")) {
			key->key = SMKEY_HARMONIC;
		} else if (arg == gensym("pythagorean")) {
			key->key = SMKEY_PYTHAGOREAN;
		} else if (arg != gensym("equal")) {
			error("Unknown key specified.");
		}
	}
	outlet_new(&key->o, &s_signal);
	key->x_f = n;
	signalinlet_new(&key->o, root);
	return key;
}

static void key_free(t_key *key __attribute__((unused))) {
	/* Do nothing */
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

smpddspclass(key);
