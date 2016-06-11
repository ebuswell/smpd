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
#include <sonicmaths.h>
#include "m_pd.h"
#include "common.h"

typedef struct {
	t_object o;
	float x_f;
	enum smlimit_kind kind;
} t_limit;

static t_class *limit_class;

smpdnew(limit) {
	float sharpness;
       	sharpness = smpdfloatarg(1, 2.0f);
	t_limit *limit = (t_limit *) pd_new(limit_class);
	limit->kind = SMLIMIT_EXP;
	if (argc > 0) {
		t_symbol *arg = atom_getsymbol(argv+0);
		if (arg == gensym("hyp")
		    || arg == gensym("hyperbolic")) {
			limit->kind = SMLIMIT_HYP;
		} else if (arg == gensym("atan")
			   || arg == gensym("arctan")
			   || arg == gensym("arctangent")) {
			limit->kind = SMLIMIT_ATAN;
		} else if (arg != gensym("exp")
			   && arg != gensym("exponential")) {
			error("Unknown limit kind specified.");
		}
	}
	outlet_new(&limit->o, &s_signal);
	limit->x_f = 0.0f;
	signalinlet_new(&limit->o, sharpness);
	return limit;
}

static void limit_free(t_limit *limit __attribute__((unused))) {
	/* Do nothing */
}

static t_int *limit_perform(t_int *w) {
	enum smlimit_kind kind = (enum smlimit_kind) w[1];
	int n = w[2];
	float *y = (float *) w[3];
	float *x = (float *) w[4];
	float *sharpness = (float *) w[5];

	smlimit(kind, n, y, x, sharpness);

	return w + 6;
}

static void limit_dsp(t_limit *limit, t_signal **sp) {
	dsp_add(limit_perform, 5,
		(t_int) limit->kind, sp[0]->s_n,
		sp[2]->s_vec, sp[0]->s_vec, sp[1]->s_vec);
}

smpddspclass(limit);
