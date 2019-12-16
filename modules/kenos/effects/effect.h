/*************************************************************************/
/*  effect.h                                                             */
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

#ifndef EFFECT_H
#define EFFECT_H

#include "core/reference.h"

class EffectHandler;

class Effect : public Object {
	GDCLASS(Effect, Object);

	friend EffectHandler;

	EffectHandler *owner = nullptr;

	float lifetime = 0.0;

protected:
	//bool _get(const StringName &p_path, Variant &r_ret) const;
	//bool _set(const StringName &p_path, const Variant &p_value);
	//void _notification(int p_what);

	void _get_property_list(List<PropertyInfo> *p_list) const;
	static void _bind_methods();

public:
	Effect() {}

	void _pre_process(float p_time);

	virtual void _process(float p_time);
};

#endif // EFFECT_H
