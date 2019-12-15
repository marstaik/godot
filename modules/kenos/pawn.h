/* pawn.h */

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
