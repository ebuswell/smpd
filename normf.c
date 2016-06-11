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
	t_float f;
} t_normf;

static t_class *normf_class;

static void normf_float(t_normf *normf, t_float f);

smpdnew(normf) {
	float f;
	f = smpdfreqarg(0, 0.0f);
	t_normf *normf = (t_normf *) pd_new(normf_class);
	normf->f = f;
	outlet_new(&normf->o, &s_float);
	outlet_float(normf->o.ob_outlet, f);
	return normf;
}

static void normf_float(t_normf *normf, t_float f) {
	outlet_float(normf->o.ob_outlet, (normf->f = smnormfv(sys_getsr(), f)));
}

static void normf_bang(t_normf *normf) {
	outlet_float(normf->o.ob_outlet, normf->f);
}

void normf_setup() {
	normf_class = class_new(gensym("normf"),
				(t_newmethod) normf_new,
				NULL,
				sizeof(t_normf),
				CLASS_DEFAULT,
				A_GIMME, A_NULL);
	class_addbang(normf_class, (t_method) normf_bang);
	class_addfloat(normf_class, (t_method) normf_float);
}
