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
#include <sonicmaths/envelope-generator.h>
#include "m_pd.h"

typedef struct {
	t_object o;
	t_sample zero;
	bool linear;
	float ctl;
	float attack_t;
	float attack_a;
	float decay_t;
	float sustain_a;
	float release_t;
	float release_a;
	struct smenvg envg;
} t_smenvg;

t_class *smenvg_class;

static void *smenvg_new(t_symbol *s __attribute__((unused)),
			int argc, t_atom *argv) {
	t_smenvg *x = (t_smenvg *) pd_new(smenvg_class);
	smenvg_init(&x->envg);
	x->linear = false;
	x->attack_t = 0.0f;
	x->attack_a = 1.0f;
	x->decay_t = 0.0f;
	x->sustain_a = 1.0f;
	x->release_t = 0.0f;
	x->release_a = 0.0f;
	if (argc > 0) {
		t_symbol *arg = atom_getsymbol(argv);
		if (arg == NULL) {
			error("Argument to smenvg~ should be"
			      " one of \"lin[ear]\" or"
			      " \"exp[onential]\". Ignoring"
			      " argument.");
		} else {
			if (arg == gensym("linear")
			    || arg == gensym("lin")) {
				x->linear = true;
			} else {
				if (arg != gensym("exponential")
				    && arg != gensym("exp")) {
					error("Argument to smenvg~ should be"
					      " one of \"lin[ear]\" or"
					      " \"exp[onential]\". Ignoring"
					      " argument.");
				}
			}
		}
		if (argc > 1) {
			error("smenvg~ takes only one argument."
			      " Ignoring subsequent arguments.");
		}
	}
	x->zero = 0.0f;
	outlet_new(&x->o, &s_signal);
	return x;
}

static void smenvg_free(t_smenvg *x) {
	smenvg_destroy(&x->envg);
}

static t_int *smenvg_perform(t_int *w) {
	t_smenvg *x = (t_smenvg *) w[1];
	t_sample *out = (t_sample *) w[2];
	t_int n = w[3];
	t_sample attack_t, attack_a, decay_t, sustain_a, release_t, release_a;

	attack_t = x->attack_t;
	attack_a = x->attack_a;
	decay_t = x->decay_t;
	sustain_a = x->sustain_a;
	release_t = x->release_t;
	release_a = x->release_a;

	*out++ = smenvg(&x->envg, x->linear, x->ctl,
			attack_t, attack_a,
			decay_t, sustain_a,
			release_t, release_a);
	x->ctl = 0.0f;
	if (x->linear) {
		while (--n) {
			*out++ = smenvg(&x->envg, true, 0.0f,
					attack_t, attack_a,
					decay_t, sustain_a,
					release_t, release_a);
		}
	} else {
		while (--n) {
			*out++ = smenvg(&x->envg, false, 0.0f,
					attack_t, attack_a,
					decay_t, sustain_a,
					release_t, release_a);
		}
	}		

	return w + 4;
}

static void smenvg_dsp(t_smenvg *x, t_signal **sp) {
	dsp_add(smenvg_perform, 3,
		x, sp[0]->s_vec, sp[0]->s_n);
}

static void smenvg_ctl(t_smenvg *x, t_float ctl) {
	x->ctl = ctl;
}

static void smenvg_attack_a(t_smenvg *x, t_float attack_a) {
	x->attack_a = attack_a;
}

static void smenvg_attack_t(t_smenvg *x, t_float attack_t) {
	x->attack_t = smaths_normtime(sys_getsr(), attack_t);
}

static void smenvg_sustain_a(t_smenvg *x, t_float sustain_a) {
	x->sustain_a = sustain_a;
}

static void smenvg_decay_t(t_smenvg *x, t_float decay_t) {
	x->decay_t = smaths_normtime(sys_getsr(), decay_t);
}

static void smenvg_release_a(t_smenvg *x, t_float release_a) {
	x->release_a = release_a;
}

static void smenvg_release_t(t_smenvg *x, t_float release_t) {
	x->release_t = smaths_normtime(sys_getsr(), release_t);
}

void smenvg_tilde_setup() {
	smenvg_class = class_new(gensym("smenvg~"),
				(t_newmethod) smenvg_new,
				(t_method) smenvg_free,
				sizeof(t_smenvg),
				CLASS_DEFAULT,
				A_GIMME, A_NULL);
	class_addmethod(smenvg_class, (t_method) smenvg_dsp, gensym("dsp"),
			A_CANT, A_NULL);
	class_addmethod(smenvg_class,
			(t_method) smenvg_ctl, gensym("ctl"),
			A_FLOAT, A_NULL);
	class_addmethod(smenvg_class,
			(t_method) smenvg_attack_a, gensym("attack_a"),
			A_FLOAT, A_NULL);
	class_addmethod(smenvg_class,
			(t_method) smenvg_attack_t, gensym("attack_t"),
			A_FLOAT, A_NULL);
	class_addmethod(smenvg_class,
			(t_method) smenvg_sustain_a, gensym("sustain_a"),
			A_FLOAT, A_NULL);
	class_addmethod(smenvg_class,
			(t_method) smenvg_decay_t, gensym("decay_t"),
			A_FLOAT, A_NULL);
	class_addmethod(smenvg_class,
			(t_method) smenvg_release_a, gensym("release_a"),
			A_FLOAT, A_NULL);
	class_addmethod(smenvg_class,
			(t_method) smenvg_release_t, gensym("release_t"),
			A_FLOAT, A_NULL);
}
