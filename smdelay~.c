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
#include <sonicmaths/delay.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	struct smdelay delay;
} t_smdelay;

t_class *smdelay_class;

static void *smdelay_new(t_floatarg len) {
	int r;
	t_smdelay *x = (t_smdelay *) pd_new(smdelay_class);
	r = smdelay_init(&x->delay, (size_t) len);
	if (r != 0) {
		error("Could not initialize smdelay");
	}
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	signalinlet_new(&x->o, 0.0f); /* delay */
	return x;
}

static void smdelay_free(t_smdelay *x) {
	smdelay_destroy(&x->delay);
}

static t_int *smdelay_perform(t_int *w) {
	t_smdelay *x = (t_smdelay *) w[1];
	t_sample *in = (t_sample *) w[2];
	t_sample *delay = (t_sample *) w[3];
	t_sample *out = (t_sample *) w[4];
	t_int n = w[5];

	while (n--) {
		*out++ = smdelay(&x->delay, *in++, *delay++);
	}

	return w + 6;
}

static void smdelay_dsp(t_smdelay *x, t_signal **sp) {
	dsp_add(smdelay_perform, 5,
		x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void smdelay_tilde_setup() {
	smdelay_class = class_new(gensym("smdelay~"),
				(t_newmethod) smdelay_new,
				(t_method) smdelay_free,
				sizeof(t_smdelay),
				CLASS_DEFAULT,
				A_DEFFLOAT, A_NULL);
	class_addmethod(smdelay_class, (t_method) smdelay_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(smdelay_class, t_smdelay, zero); /* input */
}
