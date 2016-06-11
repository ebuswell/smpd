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
	struct smsandh sandh;
} t_sandh;

static t_class *sandh_class;

smpdnew(sandh) {
	int r;
	smpdnoargs();
	t_sandh *sandh = (t_sandh *) pd_new(sandh_class);
	r = smsandh_init(&sandh->sandh);
	if (r != 0) {
		error("Could not initialize sandh~");
		return NULL;
	}
	outlet_new(&sandh->o, &s_signal);
	sandh->x_f = 0.0f;
	signalinlet_new(&sandh->o, 0.0f);
	return sandh;
}

static void sandh_free(t_sandh *sandh) {
	smsandh_destroy(&sandh->sandh);
}

static t_int *sandh_perform(t_int *w) {
	struct smsandh *sandh = (struct smsandh *) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *x = (float *) w[4];
	float *ctl = (float *) w[5];

	smsandh(sandh, n, y, x, ctl);

	return w + 6;
}

static void sandh_dsp(t_sandh *sandh, t_signal **sp) {
	dsp_add(sandh_perform, 5,
		&sandh->sandh, sp[0]->s_n,
		sp[2]->s_vec, sp[0]->s_vec, sp[1]->s_vec);
}

smpddspclass(sandh);
