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
#include <string.h>
#include <sonicmaths.h>
#include "m_pd.h"
#include "common.h"

#ifdef FILTERARG
#define FILTERARGNEWDEF float A;
#define FILTERARGNEWARG A = smpdfloatarg(1, FILTERARGDEFAULT);
#define FILTERARGNEWSIGNAL(pdsym) signalinlet_new(&pdsym->o, A);
#define FILTERARGPERFORMDEF float *A = (float *) w[6];
#define FILTERARGPERFORMARG , A
#define FILTERARGDSPARG , sp[2]->s_vec
#else
#define FILTERARGNEWDEF
#define FILTERARGNEWARG
#define FILTERARGNEWSIGNAL(pdsym)
#define FILTERARGPERFORMDEF
#define FILTERARGPERFORMARG
#define FILTERARGDSPARG
#endif

#define MAKEFILTERCODE(pdsym, order, ffunc) \
 \
typedef struct { \
	t_object o; \
	float x_f; \
	float u[order]; \
} t_ ## pdsym; \
 \
static t_class *pdsym ## _class; \
 \
smpdnew(pdsym) { \
	float f; \
	FILTERARGNEWDEF \
	f = smpdfreqarg(0, SMKEYF_C); \
	FILTERARGNEWARG \
	t_ ## pdsym *pdsym = (t_ ## pdsym *) pd_new(pdsym ## _class); \
	memset(pdsym->u, 0, sizeof(float) * order); \
	outlet_new(&pdsym->o, &s_signal); \
	pdsym->x_f = 0.0f; \
	signalinlet_new(&pdsym->o, f); \
	FILTERARGNEWSIGNAL(pdsym) \
	return pdsym; \
} \
 \
static void pdsym ## _free(t_ ## pdsym *pdsym __attribute__((unused))) { \
	/* Do nothing */ \
} \
 \
static t_int *pdsym ## _perform(t_int *w) { \
	float *u = (float *) w[1]; \
	int n = w[2]; \
	float *y = (float *) w[3]; \
	float *x = (float *) w[4]; \
	float *f = (float *) w[5]; \
	FILTERARGPERFORMDEF \
 \
	ffunc(u, n, y, x, f FILTERARGPERFORMARG); \
 \
	return w + 7; \
} \
 \
static void pdsym ## _dsp(t_ ## pdsym *pdsym, t_signal **sp) { \
	dsp_add(pdsym ## _perform, 6, \
		&pdsym->u, sp[0]->s_n, \
		sp[3]->s_vec, sp[0]->s_vec, sp[1]->s_vec FILTERARGDSPARG); \
} \
 \
smpddspclass(pdsym);
