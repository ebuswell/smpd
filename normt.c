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
	t_float t;
} t_normt;

static t_class *normt_class;

static void normt_float(t_normt *normt, t_float t);

smpdnew(normt) {
	float t;
	t = smpdtimearg(0, 0.0f);
	t_normt *normt = (t_normt *) pd_new(normt_class);
	normt->t = t;
	outlet_new(&normt->o, &s_float);
	outlet_float(normt->o.ob_outlet, t);
	return normt;
}

static void normt_float(t_normt *normt, t_float t) {
	outlet_float(normt->o.ob_outlet, (normt->t = smnormtv(sys_getsr(), t)));
}

static void normt_bang(t_normt *normt) {
	outlet_float(normt->o.ob_outlet, normt->t);
}

void normt_setup() {
	normt_class = class_new(gensym("normt"),
				(t_newmethod) normt_new,
				NULL,
				sizeof(t_normt),
				CLASS_DEFAULT,
				A_GIMME, A_NULL);
	class_addbang(normt_class, (t_method) normt_bang);
	class_addfloat(normt_class, (t_method) normt_float);
}
