/* effect.cpp */

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
