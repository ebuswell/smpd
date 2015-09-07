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
#include <sonicmaths/integrator.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	float x_f;
	struct smintg intg;
} t_intg;

static t_class *intg_class;

static void *intg_new() {
	int r;
	t_intg *intg = (t_intg *) pd_new(intg_class);
	r = smintg_init(&intg->intg);
	if (r != 0) {
		error("Could not initialize intg~");
		return NULL;
	}
	intg->x_f = 0.0f;
	outlet_new(&intg->o, &s_signal); /* y */
	return intg;
}

static void intg_free(t_intg *intg) {
	smintg_destroy(&intg->intg);
}

static t_int *intg_perform(t_int *w) {
	struct smintg *intg = (struct smintg *) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *x = (float *) w[4];

	smintg(intg, n, y, x);

	return w + 5;
}

static void intg_dsp(t_intg *intg, t_signal **sp) {
	dsp_add(intg_perform, 4,
		&intg->intg, sp[0]->s_n, sp[1]->s_vec, sp[0]->s_vec);
}

void intg_tilde_setup() {
	intg_class = class_new(gensym("intg~"),
			       (t_newmethod) intg_new,
			       (t_method) intg_free,
			       sizeof(t_intg),
			       CLASS_DEFAULT, A_NULL);
	class_addmethod(intg_class, (t_method) intg_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(intg_class, t_intg, x_f); /* x */
}
