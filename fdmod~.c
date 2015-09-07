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
#include <sonicmaths/fdmodulator.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	float x_f;
	struct smfdmod mod;
} t_fdmod;

static t_class *fdmod_class;

static void *fdmod_new(t_floatarg maxnbanks) {
	int r;
	t_fdmod *fdmod = (t_fdmod *) pd_new(fdmod_class);
	if (maxnbanks == 0.0f) {
		maxnbanks = (sys_getsr() / 2) / 10.0f;
	} else if (maxnbanks < 2.0f) {
		maxnbanks = 2.0f;
	}
	r = smfdmod_init(&fdmod->mod, maxnbanks);
	if (r != 0) {
		error("Could not initialize fdmod~");
		return NULL;
	}
	fdmod->x_f = 0.0f;
	outlet_new(&fdmod->o, &s_signal); /* y */
	signalinlet_new(&fdmod->o, 0.0f); /* b */
	signalinlet_new(&fdmod->o, smnormfv(sys_getsr(), 80.0f)); /* bankwidth */
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

void fdmod_tilde_setup() {
	fdmod_class = class_new(gensym("fdmod~"),
				(t_newmethod) fdmod_new,
				(t_method) fdmod_free,
				sizeof(t_fdmod),
				CLASS_DEFAULT,
				A_DEFFLOAT, A_NULL);
	class_addmethod(fdmod_class, (t_method) fdmod_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(fdmod_class, t_fdmod, x_f); /* a */
}
