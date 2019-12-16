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
	static void _bind_methods();

public:
	Pawn(){};
};

#endif // PAWN_H
