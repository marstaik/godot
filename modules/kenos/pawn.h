/*************************************************************************/
/*  pawn.h                                                               */
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

#ifndef PAWN_H
#define PAWN_H

#include "scene/3d/spatial.h"

class Pawn : public Spatial {
	GDCLASS(Pawn, Spatial);

protected:
	virtual bool _is_saveable() const { return true; }

	void _get_property_list(List<PropertyInfo> *p_list) const;
	void _get_property_save_list(List<PropertyInfo> *p_list) const;
	static void _bind_methods();

public:
	void print_properties() const;

	Pawn();
};

#endif // PAWN_H
