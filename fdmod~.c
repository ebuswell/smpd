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
	struct smfdmod mod;
} t_fdmod;

static t_class *fdmod_class;

smpdnew(fdmod) {
	int r;
	float bankwidth;
	bankwidth = smpdtimearg(0, 10.0f);
	t_fdmod *fdmod = (t_fdmod *) pd_new(fdmod_class);
	r = smfdmod_init(&fdmod->mod, (int) ceilf(0.5f / bankwidth));
	if (r != 0) {
		error("Could not initialize fdmod~");
		return NULL;
	}
	outlet_new(&fdmod->o, &s_signal);
	fdmod->x_f = 0.0f;
	signalinlet_new(&fdmod->o, 0.0f);
	signalinlet_new(&fdmod->o, bankwidth);
	return fdmod;
}

static void fdmod_free(t_fdmod *fdmod) {
	smfdmod_destroy(&fdmod->mod);
}

static t_int *fdmod_perform(t_int *w) {
	struct smfdmod *mod = (struct smfdmod *) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *a = (float *) w[4];
	float *b = (float *) w[5];
	float *bankwidth = (float *) w[6];

	smfdmod(mod, n, y, a, b, bankwidth);

	return w + 7;
}

static void fdmod_dsp(t_fdmod *fdmod, t_signal **sp) {
	dsp_add(fdmod_perform, 6,
		&fdmod->mod, sp[0]->s_n,
		sp[3]->s_vec, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec);
}

smpddspclass(fdmod);
