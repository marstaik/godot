/*************************************************************************/
/*  effect_handler.h                                                     */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           KENOS Project                               */
/*                      https://cosmicblimp.com                          */
/*************************************************************************/
/* Copyright (c) 2019-2020 Marios Staikopoulos, CosmicBlimp.             */
/*                                                                       */
/* Not allowed for public use. Do not distribute.                        */
/*************************************************************************/

/*
 * @author Marios Staikopoulos
 */

#ifndef EFFECT_HANDLER_H
#define EFFECT_HANDLER_H

#include "effect.h"

class Stats;

class EffectHandler : public Object {
	GDCLASS(EffectHandler, Object);

	Vector<Effect *> effects;

private:
	void remove_effect(const int p_index);

	void set_effect(Effect *p_effect, const int p_index);

	void add_effect(Effect *p_effect, const int p_index);

protected:
	bool _get(const StringName &p_path, Variant &r_ret) const;
	bool _set(const StringName &p_path, const Variant &p_value);
	//void _notification(int p_what);

	void _get_property_list(List<PropertyInfo> *p_list) const;

	static void bind_methods();

public:
	EffectHandler();

	void add_effect(Effect *p_effect);

	void _process(float p_time);

	virtual Stats *get_stats() { return nullptr; }

	void _effect_end(Object *p_effect);
};

#endif // EFFECT_HANDLER
