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
	struct smdelay delay;
} t_delay;

static t_class *delay_class;

smpdnew(delay) {
	int r, len;
	float t;
	t = smpdtimearg(0, 10.0f);
	len = (int) ceilf(t);
	if (len < 1) {
		len = 1;
	}
	t_delay *delay = (t_delay *) pd_new(delay_class);
	r = smdelay_init(&delay->delay, len);
	if (r != 0) {
		error("Could not initialize delay~");
		return NULL;
	}
	outlet_new(&delay->o, &s_signal);
	delay->x_f = 0.0f;
	signalinlet_new(&delay->o, t);
	return delay;
}

static void delay_free(t_delay *delay) {
	smdelay_destroy(&delay->delay);
}

static t_int *delay_perform(t_int *w) {
	struct smdelay *delay = (struct smdelay *) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *x = (float *) w[4];
	float *t = (float *) w[5];

	smdelay(delay, n, y, x, t);

	return w + 6;
}

static void delay_dsp(t_delay *delay, t_signal **sp) {
	dsp_add(delay_perform, 5,
		&delay->delay, sp[0]->s_n,
		sp[2]->s_vec, sp[0]->s_vec, sp[1]->s_vec);
}

smpddspclass(delay);
