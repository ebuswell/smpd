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
#include <sonicmaths/reverb.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	float x_f;
	struct smverb verb;
} t_verb;

static t_class *verb_class;

static void *verb_new(t_floatarg len, t_floatarg n) {
	int r;
	t_verb *verb = (t_verb *) pd_new(verb_class);
	if (len == 0.0f) {
		len = 409600.0f;
	} else if(len < 1.0f) {
		len = 1.0f;
	}
	if (n == 0.0f) {
		n = 32.0f;
	} else if (n < 1.0f) {
		n = 1.0f;
	}
	r = smverb_init(&verb->verb, (int) len, (int) n);
	if (r != 0) {
		error("Could not initialize smverb");
		return NULL;
	}
	verb->x_f = 0.0f;
	outlet_new(&verb->o, &s_signal); /* y */
	signalinlet_new(&verb->o, len / 2); /* t */
	signalinlet_new(&verb->o, len / 16); /* tdev */
	signalinlet_new(&verb->o, 0.5f); /* g */
	return verb;
}

static void verb_free(t_verb *verb) {
	smverb_destroy(&verb->verb);
}

static t_int *verb_perform(t_int *w) {
	struct smverb *verb = (struct smverb *) w[1];
	t_int n = w[2];
	float *y = (float *) w[3];
	float *x = (float *) w[4];
	float *t = (float *) w[5];
	float *tdev = (float *) w[6];
	float *g = (float *) w[7];

	smverb(verb, n, y, x, t, tdev, g);

	return w + 8;
}

static void verb_dsp(t_verb *verb, t_signal **sp) {
	dsp_add(verb_perform, 7,
		&verb->verb, sp[0]->s_n,
		sp[4]->s_vec, sp[0]->s_vec, sp[1]->s_vec,
		sp[2]->s_vec, sp[3]->s_vec);
}

void verb_tilde_setup() {
	verb_class = class_new(gensym("verb~"),
			       (t_newmethod) verb_new,
			       (t_method) verb_free,
			       sizeof(t_verb),
			       CLASS_DEFAULT,
			       A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(verb_class, (t_method) verb_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(verb_class, t_verb, x_f); /* x */
}
