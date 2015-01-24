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
#include <sonicmaths/reverb.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	struct smverb verb;
} t_smverb;

t_class *smverb_class;

static void *smverb_new(t_float echo_t, t_float echo_dev, t_float ntanks) {
	int r;
	t_smverb *x = (t_smverb *) pd_new(smverb_class);
	r = smverb_init(&x->verb,
			smaths_normtime(echo_t, sys_getsr()),
			smaths_normtime(echo_dev, sys_getsr()),
			(size_t) ntanks);
	if (r != 0) {
		error("Could not initialize smverb");
	}
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 0.0f); /* absorbtion */
	return x;
}

static void smverb_free(t_smverb *x) {
	smverb_destroy(&x->verb);
}

static t_int *smverb_perform(t_int *w) {
	t_smverb *x = (t_smverb *) w[1];
	t_sample *in = (t_sample *) w[2];
	t_sample *absorbtion = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];

	while (n--) {
		*out++ = smverb(&x->verb, *in++, *absorbtion++);
	}

	return w + 6;
}

static void smverb_dsp(t_smverb *x, t_signal **sp) {
	dsp_add(smverb_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smverb_tilde_setup() {
	smverb_class = class_new(gensym("smverb~"),
				(t_newmethod) smverb_new,
				(t_method) smverb_free,
				sizeof(t_smverb),
				CLASS_DEFAULT,
				A_GIMME, A_NULL);
	class_addmethod(smverb_class, (t_method) smverb_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smverb_class, t_smverb, zero); /* input */
}
