/*************************************************************************/
/*  pawn.cpp                                                             */
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

#include "pawn.h"

void Pawn::_get_property_list(List<PropertyInfo> *p_list) const {
}

void Pawn::_get_property_save_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::NIL, "Dummy2", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_CATEGORY));
}

void Pawn::_bind_methods() {
	ClassDB::bind_method(D_METHOD("print_properties"), &Pawn::print_properties);
}

void Pawn::print_properties() const {
	List<PropertyInfo> list;
	get_property_save_list(&list);

	for (int i = 0; i < list.size(); ++i) {
		print_line(list[i].name.c_str());
	}
}
