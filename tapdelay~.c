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
#include <stdlib.h>
#include <math.h>
#include <sonicmaths.h>
#include "m_pd.h"
#include "common.h"

typedef struct {
	t_object o;
	float x_f;
	int ntaps;
	struct smdelay delay;
	float **y;
	float **t;
} t_tapdelay;

static t_class *tapdelay_class;

smpdnew(tapdelay) {
	int i, r, ntaps;
	float maxlen, len;
	ntaps = (int) smpdfloatarg(0, 1.0f);
	if (argc > (ntaps + 1)) {
		/* maxlen specified separately */
		maxlen = smpdtimearg(1, 10.0f);
		argv += 2;
		argc -= 2;
	} else {
		/* maxlen is max(all the lengths) */
		argv += 1;
		argc -= 1;
		maxlen = 0.0f;
		for (i = 0; i < ntaps; i++) {
			len = smpdtimearg(i, 10.0f);
			if (len > maxlen) {
				maxlen = len;
			}
		}
	}
	maxlen = ceilf(maxlen);
	if (maxlen < 1) {
		maxlen = 1;
	}
	t_tapdelay *tapdelay = (t_tapdelay *) pd_new(tapdelay_class);
	r = smdelay_init(&tapdelay->delay, (int) maxlen);
	if (r != 0) {
		error("Could not initialize tapdelay~");
		return NULL;
	}
	tapdelay->x_f = 0.0f;
	tapdelay->ntaps = ntaps;
	tapdelay->y = malloc(sizeof(float *) * ntaps);
	if (tapdelay->y == NULL) {
		smdelay_destroy(&tapdelay->delay);
		error("Could not initialize tapdelay~: malloc failed");
		return NULL;
	}
	tapdelay->t = malloc(sizeof(float *) * ntaps);
	if (tapdelay->t == NULL) {
		free(tapdelay->y);
		smdelay_destroy(&tapdelay->delay);
		error("Could not initialize tapdelay~: malloc failed");
		return NULL;
	}
	for (i = 0; i < ntaps; i++) {
		outlet_new(&tapdelay->o, &s_signal);
		signalinlet_new(&tapdelay->o, smpdtimearg(i, 10.0f));
	}
	return tapdelay;
}

static void tapdelay_free(t_tapdelay *tapdelay) {
	free(tapdelay->y);
	free(tapdelay->t);
	smdelay_destroy(&tapdelay->delay);
}

static t_int *tapdelay_perform(t_int *w) {
	struct smdelay *delay = (struct smdelay *) w[1];
	float *x = (float *) w[2];
	float **t = (float **) w[3];
	float **y = (float **) w[4];
	t_int n = w[5];
	t_int ntaps = w[6];

	smtapdelay(delay, n, ntaps, y, x, t);

	return w + 7;
}

static void tapdelay_dsp(t_tapdelay *tapdelay, t_signal **sp) {
	int i;
	for (i = 0; i < tapdelay->ntaps; i++) {
		tapdelay->t[i] = sp[i + 1]->s_vec;
		tapdelay->y[i] = sp[2 * tapdelay->ntaps - i]->s_vec;
	}
	dsp_add(tapdelay_perform, 6,
		&tapdelay->delay, sp[0]->s_vec, tapdelay->t, tapdelay->y,
		sp[0]->s_n, tapdelay->ntaps);
}

void tapdelay_tilde_setup() {
	tapdelay_class = class_new(gensym("tapdelay~"),
				   (t_newmethod) tapdelay_new,
				   (t_method) tapdelay_free,
				   sizeof(t_tapdelay),
				   CLASS_DEFAULT,
				   A_GIMME, A_NULL);
	class_addmethod(tapdelay_class, (t_method) tapdelay_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	CLASS_MAINSIGNALIN(tapdelay_class, t_tapdelay, x_f); /* input */
}
