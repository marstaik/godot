/*************************************************************************/
/*  effect_handler.cpp                                                   */
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

#include "effect_handler.h"

bool EffectHandler::_get(const StringName &p_path, Variant &r_ret) const {
	const String path = p_path;

	if (!path.begins_with("effects/"))
		return false;

	const int which = path.get_slicec('/', 1).to_int();

	ERR_FAIL_INDEX_V(which, effects.size(), false);

	r_ret = effects[which];

	return true;
}

bool EffectHandler::_set(const StringName &p_path, const Variant &p_value) {
	const String path = p_path;

	if (!path.begins_with("effects/"))
		return false;

	const int which = path.get_slicec('/', 1).to_int();

	if (which == effects.size()) {
		add_effect(Object::cast_to<Effect>(p_value), which);
		return true;
	}

	ERR_FAIL_INDEX_V(which, effects.size(), false);

	set_effect(Object::cast_to<Effect>(p_value), which);
	return true;
}

void EffectHandler::_get_property_list(List<PropertyInfo> *p_list) const {
	for (int i = 0; i < effects.size(); ++i) {
		const String prep = "effects/" + itos(i) + "/";

		p_list->push_back(PropertyInfo(Variant::OBJECT, prep, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR));
	}
}

void EffectHandler::bind_methods() {

	ClassDB::bind_method(D_METHOD("_effect_end", "effect"), &EffectHandler::_effect_end);
}

EffectHandler::EffectHandler() {
}

void EffectHandler::add_effect(Effect *p_effect) {
	add_effect(p_effect, effects.size());
}

void EffectHandler::add_effect(Effect *p_effect, const int p_index) {
	ERR_FAIL_COND(p_effect->owner != nullptr);
	ERR_FAIL_INDEX(p_index, effects.size() + 1);

	if (p_index == effects.size()) {
		effects.push_back(p_effect);
	} else {
		effects.insert(p_index, p_effect);
	}

	p_effect->owner = this;
	p_effect->connect("effect_end", this, "_effect_end");
}

void EffectHandler::_process(float p_time) {
	for (int i = 0; i < effects.size(); ++i) {
		effects[i]->_pre_process(p_time);
	}
}

void EffectHandler::remove_effect(const int p_index) {

	ERR_FAIL_INDEX(p_index, effects.size());

	Effect *effect = effects[p_index];
	effects.remove(p_index);

	memdelete(effect);
}

void EffectHandler::set_effect(Effect *p_effect, const int p_index) {
	remove_effect(p_index);
	add_effect(p_effect, p_index);
}

void EffectHandler::_effect_end(Object *p_effect) {
	Effect *effect = Object::cast_to<Effect>(p_effect);

	int find = effects.find(effect);
	ERR_FAIL_COND(find == -1);

	remove_effect(find);
}
