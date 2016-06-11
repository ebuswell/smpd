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
} t_envg;

static t_class *envg_class;

smpdnew(envg) {
	int r;
	float attack_t, attack_a, decay_t, sustain_a, release_t, release_a;
	if (argc >= 6) {
		/* attack_t, attack_a, decay_t, sustain_a, release_t,
		 * release_a */
		if (argc > 6) {
			error("envg~ takes 0, 2 (AR), 3 (ASR), 4 (ADSR), "
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
			error("envg~ takes 0, 2 (AR), 3 (ASR), 4 (ADSR), "
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
			error("envg~ takes 0, 2 (AR), 3 (ASR), 4 (ADSR), "
			      "or 6 (AaDSRr) arguments");
		}
		attack_t  = 0.0f;
		attack_a  = 1.0f;
		decay_t   = 0.0f;
		sustain_a = 1.0f;
		release_t = 0.0f;
		release_a = 0.0f;
	}
	t_envg *envg = (t_envg *) pd_new(envg_class);
	r = smenvg_init(&envg->envg);
	if (r != 0) {
		error("Could not initialize envg~");
		return NULL;
	}
	outlet_new(&envg->o, &s_signal);
	envg->x_f = 0.0f;
	signalinlet_new(&envg->o, attack_t);
	signalinlet_new(&envg->o, attack_a);
	signalinlet_new(&envg->o, decay_t);
	signalinlet_new(&envg->o, sustain_a);
	signalinlet_new(&envg->o, release_t);
	signalinlet_new(&envg->o, release_a);
	return envg;
}

static void envg_free(t_envg *envg) {
	smenvg_destroy(&envg->envg);
}

static t_int *envg_perform(t_int *w) {
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

	smenvg(envg, n, out, ctl, attack_t, attack_a,
	       decay_t, sustain_a, release_t, release_a);

	return w + 11;
}

static void envg_dsp(t_envg *envg, t_signal **sp) {
	dsp_add(envg_perform, 10,
		&envg->envg, sp[0]->s_n, sp[7]->s_vec, sp[0]->s_vec,
		sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec,
		sp[5]->s_vec, sp[6]->s_vec);
}

smpddspclass(envg);
