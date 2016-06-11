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
	struct smenvg envg;
} t_envgl;

static t_class *envgl_class;

smpdnew(envgl) {
	int r;
	float attack_t, attack_a, decay_t, sustain_a, release_t, release_a;
	if (argc >= 6) {
		/* attack_t, attack_a, decay_t, sustain_a, release_t,
		 * release_a */
		if (argc > 6) {
			error("envgl~ takes 0, 2 (AR), 3 (ASR), 4 (ADSR), "
			      "or 6 (AaDSRr) arguments");
		}
		attack_t  = smpdtimearg(0, 0.0f);
		attack_a  = smpdfloatarg(1, 1.0f);
		decay_t   = smpdtimearg(2, 0.0f);
		sustain_a = smpdfloatarg(3, 1.0f);
		release_t = smpdtimearg(4, 0.0f);
		release_a = smpdfloatarg(5, 0.0f);
	} else if (argc >= 4) {
		/* Attack, decay, sustain, release */
		if (argc > 4) {
			error("envgl~ takes 0, 2 (AR), 3 (ASR), 4 (ADSR), "
			      "or 6 (AaDSRr) arguments");
		}
		attack_t  = smpdtimearg(0, 0.0f);
		attack_a  = 1.0f;
		decay_t   = smpdtimearg(1, 0.0f);
		sustain_a = smpdfloatarg(2, 1.0f);
		release_t = smpdtimearg(3, 0.0f);
		release_a = 0.0f;
	} else if (argc >= 3) {
		/* Attack, decay/release, sustain */
		attack_t  = smpdtimearg(0, 0.0f);
		attack_a  = 1.0f;
		decay_t   = smpdtimearg(1, 0.0f);
		sustain_a = smpdfloatarg(2, 1.0f);
		release_t = decay_t;
		release_a = 0.0f;
	} else if (argc >= 2) {
		/* Attack, release */
		attack_t  = smpdtimearg(0, 0.0f);
		attack_a  = 1.0f;
		decay_t   = 0.0f;
		sustain_a = 1.0f;
		release_t = smpdtimearg(1, 0.0f);
		release_a = 0.0f;
	} else {
		if (argc > 0) {
			error("envgl~ takes 0, 2 (AR), 3 (ASR), 4 (ADSR), "
			      "or 6 (AaDSRr) arguments");
		}
		attack_t  = 0.0f;
		attack_a  = 1.0f;
		decay_t   = 0.0f;
		sustain_a = 1.0f;
		release_t = 0.0f;
		release_a = 0.0f;
	}
	t_envgl *envgl = (t_envgl *) pd_new(envgl_class);
	r = smenvg_init(&envgl->envg);
	if (r != 0) {
		error("Could not initialize envgl~");
		return NULL;
	}
	outlet_new(&envgl->o, &s_signal);
	envgl->x_f = 0.0f;
	signalinlet_new(&envgl->o, attack_t);
	signalinlet_new(&envgl->o, attack_a);
	signalinlet_new(&envgl->o, decay_t);
	signalinlet_new(&envgl->o, sustain_a);
	signalinlet_new(&envgl->o, release_t);
	signalinlet_new(&envgl->o, release_a);
	return envgl;
}

static void envgl_free(t_envgl *envgl) {
	smenvg_destroy(&envgl->envg);
}

static t_int *envgl_perform(t_int *w) {
	struct smenvg *envg = (struct smenvg *) w[1];
	int n = w[2];
	float *out = (float *) w[3];
	float *ctl = (float *) w[4];
	float *attack_t = (float *) w[5];
	float *attack_a = (float *) w[6];
	float *decay_t = (float *) w[7];
	float *sustain_a = (float *) w[8];
	float *release_t = (float *) w[9];
	float *release_a = (float *) w[10];

	smenvgl(envg, n, out, ctl, attack_t, attack_a,
	       decay_t, sustain_a, release_t, release_a);

	return w + 11;
}

static void envgl_dsp(t_envgl *envgl, t_signal **sp) {
	dsp_add(envgl_perform, 10,
		&envgl->envg, sp[0]->s_n, sp[7]->s_vec, sp[0]->s_vec,
		sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec,
		sp[5]->s_vec, sp[6]->s_vec);
}

smpddspclass(envgl);
