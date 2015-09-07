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
#include <sonicmaths/differentiator.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	float x_f;
	struct smdiff diff;
} t_diff;

static t_class *diff_class;

static void *diff_new() {
	int r;
	t_diff *diff = (t_diff *) pd_new(diff_class);
	r = smdiff_init(&diff->diff);
	if (r != 0) {
		error("Could not initialize diff~");
		return NULL;
	}
	diff->x_f = 0.0f;
	outlet_new(&diff->o, &s_signal); /* y */
	return diff;
}

static void diff_free(t_diff *diff) {
	smdiff_destroy(&diff->diff);
}

static t_int *diff_perform(t_int *w) {
	struct smdiff *diff = (struct smdiff *) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *x = (float *) w[4];

	smdiff(diff, n, y, x);

	return w + 5;
}

static void diff_dsp(t_diff *diff, t_signal **sp) {
	dsp_add(diff_perform, 4,
		&diff->diff, sp[0]->s_n, sp[1]->s_vec, sp[0]->s_vec);
}

void diff_tilde_setup() {
	diff_class = class_new(gensym("diff~"),
			       (t_newmethod) diff_new,
			       (t_method) diff_free,
			       sizeof(t_diff),
			       CLASS_DEFAULT, A_NULL);
	class_addmethod(diff_class, (t_method) diff_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(diff_class, t_diff, x_f); /* x */
}
