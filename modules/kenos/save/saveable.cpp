/*************************************************************************/
/*  saveable.cpp                                                         */
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

#include "saveable.h"
#include "core/script_language.h"

void Saveable::get_property_save_list(List<PropertyInfo> *p_list, bool p_reversed /*= false*/) const {
	const Object *object = dynamic_cast<const Object *>(this);
	if (object && object->get_script_instance() && p_reversed) {
#ifdef TOOLS_ENABLED
		//p_list->push_back(PropertyInfo(Variant::NIL, "Script Variables", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_CATEGORY));
#endif
		//object->get_script_instance()->get_property_list(p_list);
	}

	_get_property_save_listv(p_list, p_reversed);

	if (object && object->get_script_instance() && !p_reversed) {
#ifdef TOOLS_ENABLED
		p_list->push_back(PropertyInfo(Variant::NIL, "Script Variables", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_CATEGORY));
#endif
		//object->get_script_instance()->get_property_save_list(p_list);
	}
}

void Saveable::_get_property_save_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::NIL, "Dummy", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_CATEGORY));
}

Saveable::Saveable() {
}
