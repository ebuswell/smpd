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
	struct smlag lag;
} t_lage;

static t_class *lage_class;

smpdnew(lage) {
	int r;
	float t;
	t = smpdtimearg(0, 0.0f);
	t_lage *lage = (t_lage *) pd_new(lage_class);
	r = smlag_init(&lage->lag);
	if (r != 0) {
		error("Could not initialize lage~");
		return NULL;
	}
	outlet_new(&lage->o, &s_signal);
	lage->x_f = 0.0f;
	signalinlet_new(&lage->o, t);
	return lage;
}

static void lage_free(t_lage *lage) {
	smlag_destroy(&lage->lag);
}

static t_int *lage_perform(t_int *w) {
	struct smlag *lag = (struct smlag *) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *x = (float *) w[4];
	float *t = (float *) w[5];

	smlage(lag, n, y, x, t);

	return w + 6;
}

static void lage_dsp(t_lage *lage, t_signal **sp) {
	dsp_add(lage_perform, 5,
		&lage->lag, sp[0]->s_n,
		sp[2]->s_vec, sp[0]->s_vec, sp[1]->s_vec);
}

smpddspclass(lage);
