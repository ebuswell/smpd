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
	t_sample zero;
	struct smintg intg;
} t_smintg;

t_class *smintg_class;

static void *smintg_new() {
	t_smintg *x = (t_smintg *) pd_new(smintg_class);
	smintg_init(&x->intg);
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	return x;
}

static void smintg_free(t_smintg *x) {
	smintg_destroy(&x->intg);
}

static t_int *smintg_perform(t_int *w) {
	t_smintg *x = (t_smintg *) w[1];
	t_sample *in = (t_sample *) w[2];
	t_sample *out = (t_sample *) w[3];
	t_int n = w[4];

	while (n--) {
		*out++ = smintg(&x->intg, *in++);
	}

	return w + 5;
}

static void smintg_dsp(t_smintg *x, t_signal **sp) {
	dsp_add(smintg_perform, 4,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

void smintg_tilde_setup() {
	smintg_class = class_new(gensym("smintg~"),
				 (t_newmethod) smintg_new,
				 (t_method) smintg_free,
				 sizeof(t_smintg),
				 CLASS_DEFAULT, A_NULL);
	class_addmethod(smintg_class, (t_method) smintg_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smintg_class, t_smintg, zero); /* input */
}
