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
#include "core/print_string.h"

void Pawn::_get_property_list(List<PropertyInfo> *p_list) const {
}

void Pawn::_bind_methods() {
	ClassDB::bind_method(D_METHOD("print_properties"), &Pawn::print_properties);
}

void Pawn::print_properties() const {

	List<PropertyInfo> list;
	get_property_list(&list);

	for (int i = 0; i < list.size(); ++i) {
		if ((list[i].usage & PROPERTY_USAGE_SAVE) != 0)
			print_line(list[i].name.c_str());
	}
}

Pawn::Pawn() {
	set_process(true);
}
