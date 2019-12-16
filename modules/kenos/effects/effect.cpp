/*************************************************************************/
/*  effect.cpp                                                           */
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

#include "effect.h"

void Effect::_get_property_list(List<PropertyInfo> *p_list) const {
}

void Effect::_bind_methods() {
	ADD_SIGNAL(MethodInfo("effect_enter"));
	ADD_SIGNAL(MethodInfo("effect_end"));
}

void Effect::_pre_process(float p_time) {

	lifetime += p_time;

	_process(p_time);
}

void Effect::_process(float p_time) {
}
