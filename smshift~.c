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
#include <sonicmaths/shifter.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	struct smshift shift;
} t_smshift;

t_class *smshift_class;

static void *smshift_new() {
	t_smshift *x = (t_smshift *) pd_new(smshift_class);
	smshift_init(&x->shift);
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 0.0f); /* shift */
	return x;
}

static void smshift_free(t_smshift *x) {
	smshift_destroy(&x->shift);
}

static t_int *smshift_perform(t_int *w) {
	t_smshift *x = (t_smshift *) w[1];
	t_sample *in = (t_sample *) w[2];
	t_sample *shift = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];

	while (n--) {
		*out++ = smshift(&x->shift, *in++, *shift++);
	}

	return w + 6;
}

static void smshift_dsp(t_smshift *x, t_signal **sp) {
	dsp_add(smshift_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smshift_tilde_setup() {
	smshift_class = class_new(gensym("smshift~"),
				 (t_newmethod) smshift_new,
				 (t_method) smshift_free,
				 sizeof(t_smshift),
				 CLASS_DEFAULT, A_NULL);
	class_addmethod(smshift_class, (t_method) smshift_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smshift_class, t_smshift, zero); /* input */
}
