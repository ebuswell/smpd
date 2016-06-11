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
#include <math.h>
#include <sonicmaths.h>
#include "m_pd.h"
#include "common.h"

typedef struct {
	t_object o;
	float x_f;
	struct smverb verb;
} t_verb;

static t_class *verb_class;

smpdnew(verb) {
	int n, r, maxlen;
	float len, tdev, g;
	n = (int) smpdfloatarg(0, 32.0f);
	if (n < 1) {
		n = 1;
	}
	len = smpdtimearg(1, 10.0f);
	tdev = smpdtimearg(2, 0.1f);
	g = smpdfloatarg(3, 0.85f);
	maxlen = ceilf(len + 5.0f * tdev);
       	if (maxlen < 1) {
		maxlen = 1;
	}
	t_verb *verb = (t_verb *) pd_new(verb_class);
	r = smverb_init(&verb->verb, maxlen, (int) n);
	if (r != 0) {
		error("Could not initialize smverb");
		return NULL;
	}
	outlet_new(&verb->o, &s_signal);
	verb->x_f = 0.0f;
	signalinlet_new(&verb->o, len);
	signalinlet_new(&verb->o, tdev);
	signalinlet_new(&verb->o, g);
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

smpddspclass(verb);
