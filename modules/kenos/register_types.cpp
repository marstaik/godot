/* register_types.cpp */

#include "register_types.h"

#include "core/class_db.h"

#include "effects/effect.h"
#include "effects/effect_handler.h"
#include "pawn.h"
#include "save/save_manager.h"
#include "stats/stats.h"

void register_kenos_types() {
	ClassDB::register_class<Pawn>();

	ClassDB::register_class<Effect>();
	ClassDB::register_class<EffectHandler>();

	ClassDB::register_class<Stats>();

	ClassDB::register_class<SaveManager>();
}

void unregister_kenos_types() {
}
