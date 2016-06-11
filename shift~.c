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
	struct smshift shift;
} t_shift;

static t_class *shift_class;

smpdnew(shift) {
	int r;
	float f;
	f = smpdfreqarg(0, 0.0f);
	t_shift *shift = (t_shift *) pd_new(shift_class);
	r = smshift_init(&shift->shift);
	if (r != 0) {
		error("Could not initialize shift~");
		return NULL;
	}
	outlet_new(&shift->o, &s_signal);
	shift->x_f = 0.0f;
	signalinlet_new(&shift->o, f);
	return shift;
}

static void shift_free(t_shift *shift) {
	smshift_destroy(&shift->shift);
}

static t_int *shift_perform(t_int *w) {
	struct smshift *shift = (struct smshift *) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *x = (float *) w[4];
	float *f = (float *) w[5];

	smshift(shift, n, y, x, f);

	return w + 6;
}

static void shift_dsp(t_shift *shift, t_signal **sp) {
	dsp_add(shift_perform, 5,
		&shift->shift, sp[0]->s_n,
		sp[2]->s_vec, sp[0]->s_vec, sp[1]->s_vec);
}

smpddspclass(shift);
