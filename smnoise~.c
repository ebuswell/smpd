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
#include <sonicmaths/random.h>
#include "m_pd.h"

typedef struct {
	t_object o;
} t_smnoise;

t_class *smnoise_class;

static void *smnoise_new() {
	t_smnoise *x = (t_smnoise *) pd_new(smnoise_class);
	outlet_new(&x->o, &s_signal);
	return x;
}

static t_int *smnoise_perform(t_int *w) {
	t_sample *out = (t_sample *) w[1];
	t_int n = w[2];

	while (n--) {
		*out++ = smrand_gaussian();
	}

	return w + 3;
}

static void smnoise_dsp(t_smnoise *x __attribute__((unused)), t_signal **sp) {
	dsp_add(smnoise_perform, 2,
		sp[0]->s_vec, sp[0]->s_n);
}

void smnoise_tilde_setup() {
	smnoise_class = class_new(gensym("smnoise~"),
				  (t_newmethod) smnoise_new,
				  NULL,
				  sizeof(t_smnoise),
				  CLASS_DEFAULT, A_NULL);
	class_addmethod(smnoise_class, (t_method) smnoise_dsp, gensym("dsp"),
			A_CANT, A_NULL);
}
