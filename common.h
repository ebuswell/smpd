#ifndef COMMON_H
#define COMMON_H 1

#include <sonicmaths.h>
#include "m_pd.h"

#define atom_type(atom) \
	((atom)->a_type)

#define atom_getdeffloat(atom, def) \
	((atom)->a_type == A_FLOAT ? (atom)->a_w.w_float : def)

#define atom_getstring(atom) \
	((atom)->a_type == A_SYMBOL ? (atom)->a_w.w_symbol->s_name : NULL)

#define smpdnew(cls)							\
	static void *cls ## _new(t_symbol *s __attribute__((unused)),	\
			         int argc, t_atom *argv)
#define smpdfloatarg(n, def) \
	(argc <= (n) ? (def) : atom_getdeffloat(argv+(n), (def)))

#define smpdfreqarg(n, def)						\
	smnormfv(sys_getsr(),						\
		 argc <= (n) ? (def)					\
		 : atom_type(argv+(n)) == A_SYMBOL ?			\
			smn2fv(smstr2nv(atom_getstring(argv+(n)), "C4"), \
			       SMKEYF_C)				\
		 : atom_getdeffloat(argv+(n), (def)))

#define smpdnotearg(n, rn, def)						\
	 (argc <= (n) ? (def)						\
	  : atom_type(argv+(n)) == A_SYMBOL ?				\
		smstr2nv(atom_getstring(argv+(n)),			\
			 atom_type(argv+(rn)) == A_SYMBOL		\
			 ? atom_getstring(argv+(rn)) : "C4")		\
	  : atom_getdeffloat(argv+(n), (def)))

#define smpdtimearg(n, def)						\
	smnormtv(sys_getsr(),						\
		 argc <= (n) ? (def)					\
		 : atom_getdeffloat(argv+(n), (def)))

#define smpdnoargs()							\
	(void) argc; (void) argv

#define smpddspclass(cls)						\
	void cls ## _tilde_setup() {					\
		cls ## _class = class_new(gensym(#cls "~"),		\
					  (t_newmethod) cls ## _new,	\
					  (t_method) cls ## _free,	\
					  sizeof(t_ ## cls),		\
					  CLASS_DEFAULT,		\
					  A_GIMME, A_NULL);		\
		class_addmethod(cls ## _class,				\
				(t_method) cls ## _dsp, gensym("dsp"),	\
				A_CANT, A_NULL);			\
		CLASS_MAINSIGNALIN(cls ## _class, t_ ## cls, x_f);	\
	}								\
	static t_class *cls ## _class

#define smpddspclassnoin(cls)						\
	void cls ## _tilde_setup() {					\
		cls ## _class = class_new(gensym(#cls "~"),		\
					  (t_newmethod) cls ## _new,	\
					  (t_method) cls ## _free,	\
					  sizeof(t_ ## cls),		\
					  CLASS_NOINLET,		\
					  A_GIMME, A_NULL);		\
		class_addmethod(cls ## _class,				\
				(t_method) cls ## _dsp, gensym("dsp"),	\
				A_CANT, A_NULL);			\
	}								\
	static t_class *cls ## _class

#endif /* !COMMON_H */
