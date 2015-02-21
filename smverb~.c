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
#include <sonicmaths/random.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	struct smverb verb;
} t_smverb;

t_class *smverb_class;

static void *smverb_new(t_floatarg len, t_floatarg n) {
	int r;
	t_smverb *x = (t_smverb *) pd_new(smverb_class);
	if (len == 0.0f) {
		len = 409600.0f;
	} else if(len < 1.0f) {
		len = 1.0f;
	}
	if (n == 0.0f) {
		n = 32.0f;
	} else if(n < 1.0f) {
		n = 1.0f;
	}
	r = smverb_init(&x->verb, (size_t) ceilf(len), (size_t) ceilf(n));
	if (r != 0) {
		error("Could not initialize smverb");
	}
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, len); /* t */
	signalinlet_new(&x->o, 1.0f); /* tdev */
	signalinlet_new(&x->o, n); /* n */
	signalinlet_new(&x->o, 0.5f); /* g */
	return x;
}

static void smverb_free(t_smverb *x) {
	smverb_destroy(&x->verb);
}

static t_int *smverb_perform(t_int *w) {
	t_smverb *x = (t_smverb *) w[1];
	t_sample *in = (t_sample *) w[2];
	t_sample *t = (t_sample *) w[3];
	t_sample *tdev = (t_sample *) w[4];
	t_sample *n = (t_sample *) w[5];
	t_sample *g = (t_sample *) w[6];
	t_sample *out = (t_sample *) w[7];
	t_int nn = w[8];

	while (nn--) {
		*out++ = smverb(&x->verb, *in++, *t++, *tdev++, *n++, *g++);
	}

	return w + 9;
}

static void smverb_dsp(t_smverb *x, t_signal **sp) {
	dsp_add(smverb_perform, 8,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec,
		sp[4]->s_vec, sp[5]->s_vec, sp[0]->s_n);
}

void smverb_tilde_setup() {
	smverb_class = class_new(gensym("smverb~"),
				 (t_newmethod) smverb_new,
				 (t_method) smverb_free,
				 sizeof(t_smverb),
				 CLASS_DEFAULT,
				 A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(smverb_class, (t_method) smverb_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smverb_class, t_smverb, zero); /* input */
}
