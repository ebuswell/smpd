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
#include <sonicmaths/quantize.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
} t_smquant;

t_class *smquant_class;

static void *smquant_new() {
	t_smquant *x = (t_smquant *) pd_new(smquant_class);
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 2.0f); /* i */
	return x;
}

static t_int *smquant_perform(t_int *w) {
	t_sample *in = (t_sample *) w[1];
	t_sample *i = (t_sample *) w[2];
	t_sample *out = (t_sample *) w[3];
	t_int n = w[4];

	while (n--) {
		*out++ = smquant(*in++, *i++);
	}

	return w + 5;
}

static void smquant_dsp(t_smquant *x __attribute__((unused)), t_signal **sp) {
	dsp_add(smquant_perform, 4,
		sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smquant_tilde_setup() {
	smquant_class = class_new(gensym("smquant~"),
				  (t_newmethod) smquant_new,
				  NULL,
				  sizeof(t_smquant),
				  CLASS_DEFAULT, A_NULL);
	class_addmethod(smquant_class,
			(t_method) smquant_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smquant_class, t_smquant, zero); /* input */
}
