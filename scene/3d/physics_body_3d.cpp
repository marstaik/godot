/*************************************************************************/
/*  physics_body_3d.cpp                                                  */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "physics_body_3d.h"

#include "core/core_string_names.h"
#include "core/engine.h"
#include "core/list.h"
#include "core/method_bind_ext.gen.inc"
#include "core/object.h"
#include "core/rid.h"
#include "scene/3d/collision_shape_3d.h"
#include "scene/scene_string_names.h"
#include "servers/navigation_server_3d.h"

#ifdef TOOLS_ENABLED
#include "editor/plugins/node_3d_editor_plugin.h"
#endif

Vector3 PhysicsBody3D::get_linear_velocity() const {
	return Vector3();
}

Vector3 PhysicsBody3D::get_angular_velocity() const {
	return Vector3();
}

float PhysicsBody3D::get_inverse_mass() const {
	return 0;
}

void PhysicsBody3D::set_collision_layer(uint32_t p_layer) {
	collision_layer = p_layer;
	PhysicsServer3D::get_singleton()->body_set_collision_layer(get_rid(), p_layer);
}

uint32_t PhysicsBody3D::get_collision_layer() const {
	return collision_layer;
}

void PhysicsBody3D::set_collision_mask(uint32_t p_mask) {
	collision_mask = p_mask;
	PhysicsServer3D::get_singleton()->body_set_collision_mask(get_rid(), p_mask);
}

uint32_t PhysicsBody3D::get_collision_mask() const {
	return collision_mask;
}

void PhysicsBody3D::set_collision_mask_bit(int p_bit, bool p_value) {
	uint32_t mask = get_collision_mask();
	if (p_value) {
		mask |= 1 << p_bit;
	} else {
		mask &= ~(1 << p_bit);
	}
	set_collision_mask(mask);
}

bool PhysicsBody3D::get_collision_mask_bit(int p_bit) const {
	return get_collision_mask() & (1 << p_bit);
}

void PhysicsBody3D::set_collision_layer_bit(int p_bit, bool p_value) {
	uint32_t mask = get_collision_layer();
	if (p_value) {
		mask |= 1 << p_bit;
	} else {
		mask &= ~(1 << p_bit);
	}
	set_collision_layer(mask);
}

bool PhysicsBody3D::get_collision_layer_bit(int p_bit) const {
	return get_collision_layer() & (1 << p_bit);
}

TypedArray<PhysicsBody3D> PhysicsBody3D::get_collision_exceptions() {
	List<RID> exceptions;
	PhysicsServer3D::get_singleton()->body_get_collision_exceptions(get_rid(), &exceptions);
	Array ret;
	for (List<RID>::Element *E = exceptions.front(); E; E = E->next()) {
		RID body = E->get();
		ObjectID instance_id = PhysicsServer3D::get_singleton()->body_get_object_instance_id(body);
		Object *obj = ObjectDB::get_instance(instance_id);
		PhysicsBody3D *physics_body = Object::cast_to<PhysicsBody3D>(obj);
		ret.append(physics_body);
	}
	return ret;
}

void PhysicsBody3D::add_collision_exception_with(Node *p_node) {
	ERR_FAIL_NULL(p_node);
	CollisionObject3D *collision_object = Object::cast_to<CollisionObject3D>(p_node);
	ERR_FAIL_COND_MSG(!collision_object, "Collision exception only works between two CollisionObject3Ds.");
	PhysicsServer3D::get_singleton()->body_add_collision_exception(get_rid(), collision_object->get_rid());
}

void PhysicsBody3D::remove_collision_exception_with(Node *p_node) {
	ERR_FAIL_NULL(p_node);
	CollisionObject3D *collision_object = Object::cast_to<CollisionObject3D>(p_node);
	ERR_FAIL_COND_MSG(!collision_object, "Collision exception only works between two CollisionObject3Ds.");
	PhysicsServer3D::get_singleton()->body_remove_collision_exception(get_rid(), collision_object->get_rid());
}

void PhysicsBody3D::_set_layers(uint32_t p_mask) {
	set_collision_layer(p_mask);
	set_collision_mask(p_mask);
}

uint32_t PhysicsBody3D::_get_layers() const {
	return get_collision_layer();
}

void PhysicsBody3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_collision_layer", "layer"), &PhysicsBody3D::set_collision_layer);
	ClassDB::bind_method(D_METHOD("get_collision_layer"), &PhysicsBody3D::get_collision_layer);

	ClassDB::bind_method(D_METHOD("set_collision_mask", "mask"), &PhysicsBody3D::set_collision_mask);
	ClassDB::bind_method(D_METHOD("get_collision_mask"), &PhysicsBody3D::get_collision_mask);

	ClassDB::bind_method(D_METHOD("set_collision_mask_bit", "bit", "value"), &PhysicsBody3D::set_collision_mask_bit);
	ClassDB::bind_method(D_METHOD("get_collision_mask_bit", "bit"), &PhysicsBody3D::get_collision_mask_bit);

	ClassDB::bind_method(D_METHOD("set_collision_layer_bit", "bit", "value"), &PhysicsBody3D::set_collision_layer_bit);
	ClassDB::bind_method(D_METHOD("get_collision_layer_bit", "bit"), &PhysicsBody3D::get_collision_layer_bit);

	ClassDB::bind_method(D_METHOD("_set_layers", "mask"), &PhysicsBody3D::_set_layers);
	ClassDB::bind_method(D_METHOD("_get_layers"), &PhysicsBody3D::_get_layers);

	ADD_GROUP("Collision", "collision_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_layer", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collision_layer", "get_collision_layer");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collision_mask", "get_collision_mask");
}

PhysicsBody3D::PhysicsBody3D(PhysicsServer3D::BodyMode p_mode) :
		CollisionObject3D(PhysicsServer3D::get_singleton()->body_create(p_mode), false) {
	collision_layer = 1;
	collision_mask = 1;
}

void StaticBody3D::set_physics_material_override(const Ref<PhysicsMaterial> &p_physics_material_override) {
	if (physics_material_override.is_valid()) {
		if (physics_material_override->is_connected(CoreStringNames::get_singleton()->changed, callable_mp(this, &StaticBody3D::_reload_physics_characteristics))) {
			physics_material_override->disconnect(CoreStringNames::get_singleton()->changed, callable_mp(this, &StaticBody3D::_reload_physics_characteristics));
		}
	}

	physics_material_override = p_physics_material_override;

	if (physics_material_override.is_valid()) {
		physics_material_override->connect(CoreStringNames::get_singleton()->changed, callable_mp(this, &StaticBody3D::_reload_physics_characteristics));
	}
	_reload_physics_characteristics();
}

Ref<PhysicsMaterial> StaticBody3D::get_physics_material_override() const {
	return physics_material_override;
}

void StaticBody3D::set_constant_linear_velocity(const Vector3 &p_vel) {
	constant_linear_velocity = p_vel;
	PhysicsServer3D::get_singleton()->body_set_state(get_rid(), PhysicsServer3D::BODY_STATE_LINEAR_VELOCITY, constant_linear_velocity);
}

void StaticBody3D::set_constant_angular_velocity(const Vector3 &p_vel) {
	constant_angular_velocity = p_vel;
	PhysicsServer3D::get_singleton()->body_set_state(get_rid(), PhysicsServer3D::BODY_STATE_ANGULAR_VELOCITY, constant_angular_velocity);
}

Vector3 StaticBody3D::get_constant_linear_velocity() const {
	return constant_linear_velocity;
}

Vector3 StaticBody3D::get_constant_angular_velocity() const {
	return constant_angular_velocity;
}

void StaticBody3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_constant_linear_velocity", "vel"), &StaticBody3D::set_constant_linear_velocity);
	ClassDB::bind_method(D_METHOD("set_constant_angular_velocity", "vel"), &StaticBody3D::set_constant_angular_velocity);
	ClassDB::bind_method(D_METHOD("get_constant_linear_velocity"), &StaticBody3D::get_constant_linear_velocity);
	ClassDB::bind_method(D_METHOD("get_constant_angular_velocity"), &StaticBody3D::get_constant_angular_velocity);

	ClassDB::bind_method(D_METHOD("set_physics_material_override", "physics_material_override"), &StaticBody3D::set_physics_material_override);
	ClassDB::bind_method(D_METHOD("get_physics_material_override"), &StaticBody3D::get_physics_material_override);

	ClassDB::bind_method(D_METHOD("get_collision_exceptions"), &PhysicsBody3D::get_collision_exceptions);
	ClassDB::bind_method(D_METHOD("add_collision_exception_with", "body"), &PhysicsBody3D::add_collision_exception_with);
	ClassDB::bind_method(D_METHOD("remove_collision_exception_with", "body"), &PhysicsBody3D::remove_collision_exception_with);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "physics_material_override", PROPERTY_HINT_RESOURCE_TYPE, "PhysicsMaterial"), "set_physics_material_override", "get_physics_material_override");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "constant_linear_velocity"), "set_constant_linear_velocity", "get_constant_linear_velocity");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "constant_angular_velocity"), "set_constant_angular_velocity", "get_constant_angular_velocity");
}

StaticBody3D::StaticBody3D() :
		PhysicsBody3D(PhysicsServer3D::BODY_MODE_STATIC) {
}

StaticBody3D::~StaticBody3D() {}

void StaticBody3D::_reload_physics_characteristics() {
	if (physics_material_override.is_null()) {
		PhysicsServer3D::get_singleton()->body_set_param(get_rid(), PhysicsServer3D::BODY_PARAM_BOUNCE, 0);
		PhysicsServer3D::get_singleton()->body_set_param(get_rid(), PhysicsServer3D::BODY_PARAM_FRICTION, 1);
	} else {
		PhysicsServer3D::get_singleton()->body_set_param(get_rid(), PhysicsServer3D::BODY_PARAM_BOUNCE, physics_material_override->computed_bounce());
		PhysicsServer3D::get_singleton()->body_set_param(get_rid(), PhysicsServer3D::BODY_PARAM_FRICTION, physics_material_override->computed_friction());
	}
}

void RigidBody3D::_body_enter_tree(ObjectID p_id) {
	Object *obj = ObjectDB::get_instance(p_id);
	Node *node = Object::cast_to<Node>(obj);
	ERR_FAIL_COND(!node);

	ERR_FAIL_COND(!contact_monitor);
	Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.find(p_id);
	ERR_FAIL_COND(!E);
	ERR_FAIL_COND(E->get().in_tree);

	E->get().in_tree = true;

	contact_monitor->locked = true;

	emit_signal(SceneStringNames::get_singleton()->body_entered, node);

	for (int i = 0; i < E->get().shapes.size(); i++) {
		emit_signal(SceneStringNames::get_singleton()->body_shape_entered, p_id, node, E->get().shapes[i].body_shape, E->get().shapes[i].local_shape);
	}

	contact_monitor->locked = false;
}

void RigidBody3D::_body_exit_tree(ObjectID p_id) {
	Object *obj = ObjectDB::get_instance(p_id);
	Node *node = Object::cast_to<Node>(obj);
	ERR_FAIL_COND(!node);
	ERR_FAIL_COND(!contact_monitor);
	Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.find(p_id);
	ERR_FAIL_COND(!E);
	ERR_FAIL_COND(!E->get().in_tree);
	E->get().in_tree = false;

	contact_monitor->locked = true;

	emit_signal(SceneStringNames::get_singleton()->body_exited, node);

	for (int i = 0; i < E->get().shapes.size(); i++) {
		emit_signal(SceneStringNames::get_singleton()->body_shape_exited, p_id, node, E->get().shapes[i].body_shape, E->get().shapes[i].local_shape);
	}

	contact_monitor->locked = false;
}

void RigidBody3D::_body_inout(int p_status, ObjectID p_instance, int p_body_shape, int p_local_shape) {
	bool body_in = p_status == 1;
	ObjectID objid = p_instance;

	Object *obj = ObjectDB::get_instance(objid);
	Node *node = Object::cast_to<Node>(obj);

	ERR_FAIL_COND(!contact_monitor);
	Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.find(objid);

	ERR_FAIL_COND(!body_in && !E);

	if (body_in) {
		if (!E) {
			E = contact_monitor->body_map.insert(objid, BodyState());
			//E->get().rc=0;
			E->get().in_tree = node && node->is_inside_tree();
			if (node) {
				node->connect(SceneStringNames::get_singleton()->tree_entered, callable_mp(this, &RigidBody3D::_body_enter_tree), make_binds(objid));
				node->connect(SceneStringNames::get_singleton()->tree_exiting, callable_mp(this, &RigidBody3D::_body_exit_tree), make_binds(objid));
				if (E->get().in_tree) {
					emit_signal(SceneStringNames::get_singleton()->body_entered, node);
				}
			}
		}
		//E->get().rc++;
		if (node) {
			E->get().shapes.insert(ShapePair(p_body_shape, p_local_shape));
		}

		if (E->get().in_tree) {
			emit_signal(SceneStringNames::get_singleton()->body_shape_entered, objid, node, p_body_shape, p_local_shape);
		}

	} else {
		//E->get().rc--;

		if (node) {
			E->get().shapes.erase(ShapePair(p_body_shape, p_local_shape));
		}

		bool in_tree = E->get().in_tree;

		if (E->get().shapes.empty()) {
			if (node) {
				node->disconnect(SceneStringNames::get_singleton()->tree_entered, callable_mp(this, &RigidBody3D::_body_enter_tree));
				node->disconnect(SceneStringNames::get_singleton()->tree_exiting, callable_mp(this, &RigidBody3D::_body_exit_tree));
				if (in_tree) {
					emit_signal(SceneStringNames::get_singleton()->body_exited, node);
				}
			}

			contact_monitor->body_map.erase(E);
		}
		if (node && in_tree) {
			emit_signal(SceneStringNames::get_singleton()->body_shape_exited, objid, obj, p_body_shape, p_local_shape);
		}
	}
}

struct _RigidBodyInOut {
	ObjectID id;
	int shape;
	int local_shape;
};

void RigidBody3D::_direct_state_changed(Object *p_state) {
#ifdef DEBUG_ENABLED
	state = Object::cast_to<PhysicsDirectBodyState3D>(p_state);
#else
	state = (PhysicsDirectBodyState3D *)p_state; //trust it
#endif

	set_ignore_transform_notification(true);
	set_global_transform(state->get_transform());
	linear_velocity = state->get_linear_velocity();
	angular_velocity = state->get_angular_velocity();
	inverse_inertia_tensor = state->get_inverse_inertia_tensor();
	if (sleeping != state->is_sleeping()) {
		sleeping = state->is_sleeping();
		emit_signal(SceneStringNames::get_singleton()->sleeping_state_changed);
	}
	if (get_script_instance()) {
		get_script_instance()->call("_integrate_forces", state);
	}
	set_ignore_transform_notification(false);

	if (contact_monitor) {
		contact_monitor->locked = true;

		//untag all
		int rc = 0;
		for (Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.front(); E; E = E->next()) {
			for (int i = 0; i < E->get().shapes.size(); i++) {
				E->get().shapes[i].tagged = false;
				rc++;
			}
		}

		_RigidBodyInOut *toadd = (_RigidBodyInOut *)alloca(state->get_contact_count() * sizeof(_RigidBodyInOut));
		int toadd_count = 0; //state->get_contact_count();
		RigidBody3D_RemoveAction *toremove = (RigidBody3D_RemoveAction *)alloca(rc * sizeof(RigidBody3D_RemoveAction));
		int toremove_count = 0;

		//put the ones to add

		for (int i = 0; i < state->get_contact_count(); i++) {
			ObjectID obj = state->get_contact_collider_id(i);
			int local_shape = state->get_contact_local_shape(i);
			int shape = state->get_contact_collider_shape(i);

			//bool found=false;

			Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.find(obj);
			if (!E) {
				toadd[toadd_count].local_shape = local_shape;
				toadd[toadd_count].id = obj;
				toadd[toadd_count].shape = shape;
				toadd_count++;
				continue;
			}

			ShapePair sp(shape, local_shape);
			int idx = E->get().shapes.find(sp);
			if (idx == -1) {
				toadd[toadd_count].local_shape = local_shape;
				toadd[toadd_count].id = obj;
				toadd[toadd_count].shape = shape;
				toadd_count++;
				continue;
			}

			E->get().shapes[idx].tagged = true;
		}

		//put the ones to remove

		for (Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.front(); E; E = E->next()) {
			for (int i = 0; i < E->get().shapes.size(); i++) {
				if (!E->get().shapes[i].tagged) {
					toremove[toremove_count].body_id = E->key();
					toremove[toremove_count].pair = E->get().shapes[i];
					toremove_count++;
				}
			}
		}

		//process remotions

		for (int i = 0; i < toremove_count; i++) {
			_body_inout(0, toremove[i].body_id, toremove[i].pair.body_shape, toremove[i].pair.local_shape);
		}

		//process aditions

		for (int i = 0; i < toadd_count; i++) {
			_body_inout(1, toadd[i].id, toadd[i].shape, toadd[i].local_shape);
		}

		contact_monitor->locked = false;
	}

	state = nullptr;
}

void RigidBody3D::_notification(int p_what) {
#ifdef TOOLS_ENABLED
	if (p_what == NOTIFICATION_ENTER_TREE) {
		if (Engine::get_singleton()->is_editor_hint()) {
			set_notify_local_transform(true); //used for warnings and only in editor
		}
	}

	if (p_what == NOTIFICATION_LOCAL_TRANSFORM_CHANGED) {
		if (Engine::get_singleton()->is_editor_hint()) {
			update_configuration_warning();
		}
	}

#endif
}

void RigidBody3D::set_mode(Mode p_mode) {
	mode = p_mode;
	switch (p_mode) {
		case MODE_RIGID: {
			PhysicsServer3D::get_singleton()->body_set_mode(get_rid(), PhysicsServer3D::BODY_MODE_RIGID);
		} break;
		case MODE_STATIC: {
			PhysicsServer3D::get_singleton()->body_set_mode(get_rid(), PhysicsServer3D::BODY_MODE_STATIC);

		} break;
		case MODE_CHARACTER: {
			PhysicsServer3D::get_singleton()->body_set_mode(get_rid(), PhysicsServer3D::BODY_MODE_CHARACTER);

		} break;
		case MODE_KINEMATIC: {
			PhysicsServer3D::get_singleton()->body_set_mode(get_rid(), PhysicsServer3D::BODY_MODE_KINEMATIC);
		} break;
	}
	update_configuration_warning();
}

RigidBody3D::Mode RigidBody3D::get_mode() const {
	return mode;
}

void RigidBody3D::set_mass(real_t p_mass) {
	ERR_FAIL_COND(p_mass <= 0);
	mass = p_mass;
	_change_notify("mass");
	_change_notify("weight");
	PhysicsServer3D::get_singleton()->body_set_param(get_rid(), PhysicsServer3D::BODY_PARAM_MASS, mass);
}

real_t RigidBody3D::get_mass() const {
	return mass;
}

void RigidBody3D::set_weight(real_t p_weight) {
	set_mass(p_weight / real_t(GLOBAL_DEF("physics/3d/default_gravity", 9.8)));
}

real_t RigidBody3D::get_weight() const {
	return mass * real_t(GLOBAL_DEF("physics/3d/default_gravity", 9.8));
}

void RigidBody3D::set_physics_material_override(const Ref<PhysicsMaterial> &p_physics_material_override) {
	if (physics_material_override.is_valid()) {
		if (physics_material_override->is_connected(CoreStringNames::get_singleton()->changed, callable_mp(this, &RigidBody3D::_reload_physics_characteristics))) {
			physics_material_override->disconnect(CoreStringNames::get_singleton()->changed, callable_mp(this, &RigidBody3D::_reload_physics_characteristics));
		}
	}

	physics_material_override = p_physics_material_override;

	if (physics_material_override.is_valid()) {
		physics_material_override->connect(CoreStringNames::get_singleton()->changed, callable_mp(this, &RigidBody3D::_reload_physics_characteristics));
	}
	_reload_physics_characteristics();
}

Ref<PhysicsMaterial> RigidBody3D::get_physics_material_override() const {
	return physics_material_override;
}

void RigidBody3D::set_gravity_scale(real_t p_gravity_scale) {
	gravity_scale = p_gravity_scale;
	PhysicsServer3D::get_singleton()->body_set_param(get_rid(), PhysicsServer3D::BODY_PARAM_GRAVITY_SCALE, gravity_scale);
}

real_t RigidBody3D::get_gravity_scale() const {
	return gravity_scale;
}

void RigidBody3D::set_linear_damp(real_t p_linear_damp) {
	ERR_FAIL_COND(p_linear_damp < -1);
	linear_damp = p_linear_damp;
	PhysicsServer3D::get_singleton()->body_set_param(get_rid(), PhysicsServer3D::BODY_PARAM_LINEAR_DAMP, linear_damp);
}

real_t RigidBody3D::get_linear_damp() const {
	return linear_damp;
}

void RigidBody3D::set_angular_damp(real_t p_angular_damp) {
	ERR_FAIL_COND(p_angular_damp < -1);
	angular_damp = p_angular_damp;
	PhysicsServer3D::get_singleton()->body_set_param(get_rid(), PhysicsServer3D::BODY_PARAM_ANGULAR_DAMP, angular_damp);
}

real_t RigidBody3D::get_angular_damp() const {
	return angular_damp;
}

void RigidBody3D::set_axis_velocity(const Vector3 &p_axis) {
	Vector3 v = state ? state->get_linear_velocity() : linear_velocity;
	Vector3 axis = p_axis.normalized();
	v -= axis * axis.dot(v);
	v += p_axis;
	if (state) {
		set_linear_velocity(v);
	} else {
		PhysicsServer3D::get_singleton()->body_set_axis_velocity(get_rid(), p_axis);
		linear_velocity = v;
	}
}

void RigidBody3D::set_linear_velocity(const Vector3 &p_velocity) {
	linear_velocity = p_velocity;
	if (state) {
		state->set_linear_velocity(linear_velocity);
	} else {
		PhysicsServer3D::get_singleton()->body_set_state(get_rid(), PhysicsServer3D::BODY_STATE_LINEAR_VELOCITY, linear_velocity);
	}
}

Vector3 RigidBody3D::get_linear_velocity() const {
	return linear_velocity;
}

void RigidBody3D::set_angular_velocity(const Vector3 &p_velocity) {
	angular_velocity = p_velocity;
	if (state) {
		state->set_angular_velocity(angular_velocity);
	} else {
		PhysicsServer3D::get_singleton()->body_set_state(get_rid(), PhysicsServer3D::BODY_STATE_ANGULAR_VELOCITY, angular_velocity);
	}
}

Vector3 RigidBody3D::get_angular_velocity() const {
	return angular_velocity;
}

Basis RigidBody3D::get_inverse_inertia_tensor() {
	return inverse_inertia_tensor;
}

void RigidBody3D::set_use_custom_integrator(bool p_enable) {
	if (custom_integrator == p_enable) {
		return;
	}

	custom_integrator = p_enable;
	PhysicsServer3D::get_singleton()->body_set_omit_force_integration(get_rid(), p_enable);
}

bool RigidBody3D::is_using_custom_integrator() {
	return custom_integrator;
}

void RigidBody3D::set_sleeping(bool p_sleeping) {
	sleeping = p_sleeping;
	PhysicsServer3D::get_singleton()->body_set_state(get_rid(), PhysicsServer3D::BODY_STATE_SLEEPING, sleeping);
}

void RigidBody3D::set_can_sleep(bool p_active) {
	can_sleep = p_active;
	PhysicsServer3D::get_singleton()->body_set_state(get_rid(), PhysicsServer3D::BODY_STATE_CAN_SLEEP, p_active);
}

bool RigidBody3D::is_able_to_sleep() const {
	return can_sleep;
}

bool RigidBody3D::is_sleeping() const {
	return sleeping;
}

void RigidBody3D::set_max_contacts_reported(int p_amount) {
	max_contacts_reported = p_amount;
	PhysicsServer3D::get_singleton()->body_set_max_contacts_reported(get_rid(), p_amount);
}

int RigidBody3D::get_max_contacts_reported() const {
	return max_contacts_reported;
}

void RigidBody3D::add_central_force(const Vector3 &p_force) {
	PhysicsServer3D::get_singleton()->body_add_central_force(get_rid(), p_force);
}

void RigidBody3D::add_force(const Vector3 &p_force, const Vector3 &p_position) {
	PhysicsServer3D *singleton = PhysicsServer3D::get_singleton();
	singleton->body_add_force(get_rid(), p_force, p_position);
}

void RigidBody3D::add_torque(const Vector3 &p_torque) {
	PhysicsServer3D::get_singleton()->body_add_torque(get_rid(), p_torque);
}

void RigidBody3D::apply_central_impulse(const Vector3 &p_impulse) {
	PhysicsServer3D::get_singleton()->body_apply_central_impulse(get_rid(), p_impulse);
}

void RigidBody3D::apply_impulse(const Vector3 &p_impulse, const Vector3 &p_position) {
	PhysicsServer3D *singleton = PhysicsServer3D::get_singleton();
	singleton->body_apply_impulse(get_rid(), p_impulse, p_position);
}

void RigidBody3D::apply_torque_impulse(const Vector3 &p_impulse) {
	PhysicsServer3D::get_singleton()->body_apply_torque_impulse(get_rid(), p_impulse);
}

void RigidBody3D::set_use_continuous_collision_detection(bool p_enable) {
	ccd = p_enable;
	PhysicsServer3D::get_singleton()->body_set_enable_continuous_collision_detection(get_rid(), p_enable);
}

bool RigidBody3D::is_using_continuous_collision_detection() const {
	return ccd;
}

void RigidBody3D::set_contact_monitor(bool p_enabled) {
	if (p_enabled == is_contact_monitor_enabled()) {
		return;
	}

	if (!p_enabled) {
		ERR_FAIL_COND_MSG(contact_monitor->locked, "Can't disable contact monitoring during in/out callback. Use call_deferred(\"set_contact_monitor\", false) instead.");

		for (Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.front(); E; E = E->next()) {
			//clean up mess
			Object *obj = ObjectDB::get_instance(E->key());
			Node *node = Object::cast_to<Node>(obj);

			if (node) {
				node->disconnect(SceneStringNames::get_singleton()->tree_entered, callable_mp(this, &RigidBody3D::_body_enter_tree));
				node->disconnect(SceneStringNames::get_singleton()->tree_exiting, callable_mp(this, &RigidBody3D::_body_exit_tree));
			}
		}

		memdelete(contact_monitor);
		contact_monitor = nullptr;
	} else {
		contact_monitor = memnew(ContactMonitor);
		contact_monitor->locked = false;
	}
}

bool RigidBody3D::is_contact_monitor_enabled() const {
	return contact_monitor != nullptr;
}

void RigidBody3D::set_axis_lock(PhysicsServer3D::BodyAxis p_axis, bool p_lock) {
	PhysicsServer3D::get_singleton()->body_set_axis_lock(get_rid(), p_axis, p_lock);
}

bool RigidBody3D::get_axis_lock(PhysicsServer3D::BodyAxis p_axis) const {
	return PhysicsServer3D::get_singleton()->body_is_axis_locked(get_rid(), p_axis);
}

Array RigidBody3D::get_colliding_bodies() const {
	ERR_FAIL_COND_V(!contact_monitor, Array());

	Array ret;
	ret.resize(contact_monitor->body_map.size());
	int idx = 0;
	for (const Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.front(); E; E = E->next()) {
		Object *obj = ObjectDB::get_instance(E->key());
		if (!obj) {
			ret.resize(ret.size() - 1); //ops
		} else {
			ret[idx++] = obj;
		}
	}

	return ret;
}

String RigidBody3D::get_configuration_warning() const {
	Transform t = get_transform();

	String warning = CollisionObject3D::get_configuration_warning();

	if ((get_mode() == MODE_RIGID || get_mode() == MODE_CHARACTER) && (ABS(t.basis.get_axis(0).length() - 1.0) > 0.05 || ABS(t.basis.get_axis(1).length() - 1.0) > 0.05 || ABS(t.basis.get_axis(2).length() - 1.0) > 0.05)) {
		if (warning != String()) {
			warning += "\n\n";
		}
		warning += TTR("Size changes to RigidBody3D (in character or rigid modes) will be overridden by the physics engine when running.\nChange the size in children collision shapes instead.");
	}

	return warning;
}

void RigidBody3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_mode", "mode"), &RigidBody3D::set_mode);
	ClassDB::bind_method(D_METHOD("get_mode"), &RigidBody3D::get_mode);

	ClassDB::bind_method(D_METHOD("set_mass", "mass"), &RigidBody3D::set_mass);
	ClassDB::bind_method(D_METHOD("get_mass"), &RigidBody3D::get_mass);

	ClassDB::bind_method(D_METHOD("set_weight", "weight"), &RigidBody3D::set_weight);
	ClassDB::bind_method(D_METHOD("get_weight"), &RigidBody3D::get_weight);

	ClassDB::bind_method(D_METHOD("set_physics_material_override", "physics_material_override"), &RigidBody3D::set_physics_material_override);
	ClassDB::bind_method(D_METHOD("get_physics_material_override"), &RigidBody3D::get_physics_material_override);

	ClassDB::bind_method(D_METHOD("set_linear_velocity", "linear_velocity"), &RigidBody3D::set_linear_velocity);
	ClassDB::bind_method(D_METHOD("get_linear_velocity"), &RigidBody3D::get_linear_velocity);

	ClassDB::bind_method(D_METHOD("set_angular_velocity", "angular_velocity"), &RigidBody3D::set_angular_velocity);
	ClassDB::bind_method(D_METHOD("get_angular_velocity"), &RigidBody3D::get_angular_velocity);

	ClassDB::bind_method(D_METHOD("get_inverse_inertia_tensor"), &RigidBody3D::get_inverse_inertia_tensor);

	ClassDB::bind_method(D_METHOD("set_gravity_scale", "gravity_scale"), &RigidBody3D::set_gravity_scale);
	ClassDB::bind_method(D_METHOD("get_gravity_scale"), &RigidBody3D::get_gravity_scale);

	ClassDB::bind_method(D_METHOD("set_linear_damp", "linear_damp"), &RigidBody3D::set_linear_damp);
	ClassDB::bind_method(D_METHOD("get_linear_damp"), &RigidBody3D::get_linear_damp);

	ClassDB::bind_method(D_METHOD("set_angular_damp", "angular_damp"), &RigidBody3D::set_angular_damp);
	ClassDB::bind_method(D_METHOD("get_angular_damp"), &RigidBody3D::get_angular_damp);

	ClassDB::bind_method(D_METHOD("set_max_contacts_reported", "amount"), &RigidBody3D::set_max_contacts_reported);
	ClassDB::bind_method(D_METHOD("get_max_contacts_reported"), &RigidBody3D::get_max_contacts_reported);

	ClassDB::bind_method(D_METHOD("set_use_custom_integrator", "enable"), &RigidBody3D::set_use_custom_integrator);
	ClassDB::bind_method(D_METHOD("is_using_custom_integrator"), &RigidBody3D::is_using_custom_integrator);

	ClassDB::bind_method(D_METHOD("set_contact_monitor", "enabled"), &RigidBody3D::set_contact_monitor);
	ClassDB::bind_method(D_METHOD("is_contact_monitor_enabled"), &RigidBody3D::is_contact_monitor_enabled);

	ClassDB::bind_method(D_METHOD("set_use_continuous_collision_detection", "enable"), &RigidBody3D::set_use_continuous_collision_detection);
	ClassDB::bind_method(D_METHOD("is_using_continuous_collision_detection"), &RigidBody3D::is_using_continuous_collision_detection);

	ClassDB::bind_method(D_METHOD("set_axis_velocity", "axis_velocity"), &RigidBody3D::set_axis_velocity);

	ClassDB::bind_method(D_METHOD("add_central_force", "force"), &RigidBody3D::add_central_force);
	ClassDB::bind_method(D_METHOD("add_force", "force", "position"), &RigidBody3D::add_force, Vector3());
	ClassDB::bind_method(D_METHOD("add_torque", "torque"), &RigidBody3D::add_torque);

	ClassDB::bind_method(D_METHOD("apply_central_impulse", "impulse"), &RigidBody3D::apply_central_impulse);
	ClassDB::bind_method(D_METHOD("apply_impulse", "impulse", "position"), &RigidBody3D::apply_impulse, Vector3());
	ClassDB::bind_method(D_METHOD("apply_torque_impulse", "impulse"), &RigidBody3D::apply_torque_impulse);

	ClassDB::bind_method(D_METHOD("set_sleeping", "sleeping"), &RigidBody3D::set_sleeping);
	ClassDB::bind_method(D_METHOD("is_sleeping"), &RigidBody3D::is_sleeping);

	ClassDB::bind_method(D_METHOD("set_can_sleep", "able_to_sleep"), &RigidBody3D::set_can_sleep);
	ClassDB::bind_method(D_METHOD("is_able_to_sleep"), &RigidBody3D::is_able_to_sleep);

	ClassDB::bind_method(D_METHOD("_direct_state_changed"), &RigidBody3D::_direct_state_changed);

	ClassDB::bind_method(D_METHOD("set_axis_lock", "axis", "lock"), &RigidBody3D::set_axis_lock);
	ClassDB::bind_method(D_METHOD("get_axis_lock", "axis"), &RigidBody3D::get_axis_lock);

	ClassDB::bind_method(D_METHOD("get_colliding_bodies"), &RigidBody3D::get_colliding_bodies);

	BIND_VMETHOD(MethodInfo("_integrate_forces", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "PhysicsDirectBodyState3D")));

	ADD_PROPERTY(PropertyInfo(Variant::INT, "mode", PROPERTY_HINT_ENUM, "Rigid,Static,Character,Kinematic"), "set_mode", "get_mode");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mass", PROPERTY_HINT_EXP_RANGE, "0.01,65535,0.01"), "set_mass", "get_mass");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "weight", PROPERTY_HINT_EXP_RANGE, "0.01,65535,0.01", PROPERTY_USAGE_EDITOR), "set_weight", "get_weight");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "physics_material_override", PROPERTY_HINT_RESOURCE_TYPE, "PhysicsMaterial"), "set_physics_material_override", "get_physics_material_override");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity_scale", PROPERTY_HINT_RANGE, "-128,128,0.01"), "set_gravity_scale", "get_gravity_scale");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "custom_integrator"), "set_use_custom_integrator", "is_using_custom_integrator");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "continuous_cd"), "set_use_continuous_collision_detection", "is_using_continuous_collision_detection");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "contacts_reported", PROPERTY_HINT_RANGE, "0,64,1,or_greater"), "set_max_contacts_reported", "get_max_contacts_reported");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "contact_monitor"), "set_contact_monitor", "is_contact_monitor_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "sleeping"), "set_sleeping", "is_sleeping");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "can_sleep"), "set_can_sleep", "is_able_to_sleep");
	ADD_GROUP("Axis Lock", "axis_lock_");
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "axis_lock_linear_x"), "set_axis_lock", "get_axis_lock", PhysicsServer3D::BODY_AXIS_LINEAR_X);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "axis_lock_linear_y"), "set_axis_lock", "get_axis_lock", PhysicsServer3D::BODY_AXIS_LINEAR_Y);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "axis_lock_linear_z"), "set_axis_lock", "get_axis_lock", PhysicsServer3D::BODY_AXIS_LINEAR_Z);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "axis_lock_angular_x"), "set_axis_lock", "get_axis_lock", PhysicsServer3D::BODY_AXIS_ANGULAR_X);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "axis_lock_angular_y"), "set_axis_lock", "get_axis_lock", PhysicsServer3D::BODY_AXIS_ANGULAR_Y);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "axis_lock_angular_z"), "set_axis_lock", "get_axis_lock", PhysicsServer3D::BODY_AXIS_ANGULAR_Z);
	ADD_GROUP("Linear", "linear_");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "linear_velocity"), "set_linear_velocity", "get_linear_velocity");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "linear_damp", PROPERTY_HINT_RANGE, "-1,100,0.001,or_greater"), "set_linear_damp", "get_linear_damp");
	ADD_GROUP("Angular", "angular_");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "angular_velocity"), "set_angular_velocity", "get_angular_velocity");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "angular_damp", PROPERTY_HINT_RANGE, "-1,100,0.001,or_greater"), "set_angular_damp", "get_angular_damp");

	ADD_SIGNAL(MethodInfo("body_shape_entered", PropertyInfo(Variant::INT, "body_id"), PropertyInfo(Variant::OBJECT, "body", PROPERTY_HINT_RESOURCE_TYPE, "Node"), PropertyInfo(Variant::INT, "body_shape"), PropertyInfo(Variant::INT, "local_shape")));
	ADD_SIGNAL(MethodInfo("body_shape_exited", PropertyInfo(Variant::INT, "body_id"), PropertyInfo(Variant::OBJECT, "body", PROPERTY_HINT_RESOURCE_TYPE, "Node"), PropertyInfo(Variant::INT, "body_shape"), PropertyInfo(Variant::INT, "local_shape")));
	ADD_SIGNAL(MethodInfo("body_entered", PropertyInfo(Variant::OBJECT, "body", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
	ADD_SIGNAL(MethodInfo("body_exited", PropertyInfo(Variant::OBJECT, "body", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
	ADD_SIGNAL(MethodInfo("sleeping_state_changed"));

	BIND_ENUM_CONSTANT(MODE_RIGID);
	BIND_ENUM_CONSTANT(MODE_STATIC);
	BIND_ENUM_CONSTANT(MODE_CHARACTER);
	BIND_ENUM_CONSTANT(MODE_KINEMATIC);
}

RigidBody3D::RigidBody3D() :
		PhysicsBody3D(PhysicsServer3D::BODY_MODE_RIGID) {
	mode = MODE_RIGID;

	mass = 1;
	max_contacts_reported = 0;
	state = nullptr;

	gravity_scale = 1;
	linear_damp = -1;
	angular_damp = -1;

	//angular_velocity=0;
	sleeping = false;
	ccd = false;

	custom_integrator = false;
	contact_monitor = nullptr;
	can_sleep = true;

	PhysicsServer3D::get_singleton()->body_set_force_integration_callback(get_rid(), this, "_direct_state_changed");
}

RigidBody3D::~RigidBody3D() {
	if (contact_monitor) {
		memdelete(contact_monitor);
	}
}

void RigidBody3D::_reload_physics_characteristics() {
	if (physics_material_override.is_null()) {
		PhysicsServer3D::get_singleton()->body_set_param(get_rid(), PhysicsServer3D::BODY_PARAM_BOUNCE, 0);
		PhysicsServer3D::get_singleton()->body_set_param(get_rid(), PhysicsServer3D::BODY_PARAM_FRICTION, 1);
	} else {
		PhysicsServer3D::get_singleton()->body_set_param(get_rid(), PhysicsServer3D::BODY_PARAM_BOUNCE, physics_material_override->computed_bounce());
		PhysicsServer3D::get_singleton()->body_set_param(get_rid(), PhysicsServer3D::BODY_PARAM_FRICTION, physics_material_override->computed_friction());
	}
}

//////////////////////////////////////////////////////
//////////////////////////

Ref<KinematicCollision3D> KinematicBody3D::_move(const Vector3 &p_motion, bool p_infinite_inertia, bool p_exclude_raycast_shapes, bool p_test_only) {
	Collision col;
	if (move_and_collide(p_motion, p_infinite_inertia, col, p_exclude_raycast_shapes, p_test_only)) {
		if (motion_cache.is_null()) {
			motion_cache.instance();
			motion_cache->owner = this;
		}

		motion_cache->collision = col;

		return motion_cache;
	}

	return Ref<KinematicCollision3D>();
}

Vector3 KinematicBody3D::get_linear_velocity() const {
	return linear_velocity;
}

Vector3 KinematicBody3D::get_angular_velocity() const {
	return angular_velocity;
}

bool KinematicBody3D::move_and_collide(const Vector3 &p_motion, bool p_infinite_inertia, Collision &r_collision, bool p_exclude_raycast_shapes, bool p_test_only) {
	Transform gt = get_global_transform();
	PhysicsServer3D::MotionResult result;
	bool colliding = PhysicsServer3D::get_singleton()->body_test_motion(get_rid(), gt, p_motion, p_infinite_inertia, &result, p_exclude_raycast_shapes);

	if (colliding) {
		r_collision.collider_metadata = result.collider_metadata;
		r_collision.collider_shape = result.collider_shape;
		r_collision.collider_vel = result.collider_velocity;
		r_collision.collision = result.collision_point;
		r_collision.normal = result.collision_normal;
		r_collision.collider = result.collider_id;
		r_collision.collider_rid = result.collider;
		r_collision.travel = result.motion;
		r_collision.remainder = result.remainder;
		r_collision.local_shape = result.collision_local_shape;
	}

	for (int i = 0; i < 3; i++) {
		if (locked_axis & (1 << i)) {
			result.motion[i] = 0;
		}
	}

	if (!p_test_only) {
		gt.origin += result.motion;
		set_global_transform(gt);
	}

	return colliding;
}

//so, if you pass 45 as limit, avoid numerical precision errors when angle is 45.
#define FLOOR_ANGLE_THRESHOLD 0.01

Vector3 KinematicBody3D::move_and_slide(const Vector3 &p_linear_velocity, const Vector3 &p_up_direction, bool p_stop_on_slope, int p_max_slides, float p_floor_max_angle, bool p_infinite_inertia) {
	Vector3 body_velocity = p_linear_velocity;
	Vector3 body_velocity_normal = body_velocity.normalized();
	Vector3 up_direction = p_up_direction.normalized();

	for (int i = 0; i < 3; i++) {
		if (locked_axis & (1 << i)) {
			body_velocity[i] = 0;
		}
	}

	// Hack in order to work with calling from _process as well as from _physics_process; calling from thread is risky
	Vector3 motion = (floor_velocity + body_velocity) * (Engine::get_singleton()->is_in_physics_frame() ? get_physics_process_delta_time() : get_process_delta_time());

	on_floor = false;
	on_floor_body = RID();
	on_ceiling = false;
	on_wall = false;
	colliders.clear();
	floor_normal = Vector3();
	floor_velocity = Vector3();

	while (p_max_slides) {
		Collision collision;
		bool found_collision = false;

		for (int i = 0; i < 2; ++i) {
			bool collided;
			if (i == 0) { //collide
				collided = move_and_collide(motion, p_infinite_inertia, collision);
				if (!collided) {
					motion = Vector3(); //clear because no collision happened and motion completed
				}
			} else { //separate raycasts (if any)
				collided = separate_raycast_shapes(p_infinite_inertia, collision);
				if (collided) {
					collision.remainder = motion; //keep
					collision.travel = Vector3();
				}
			}

			if (collided) {
				found_collision = true;

				colliders.push_back(collision);
				motion = collision.remainder;

				if (up_direction == Vector3()) {
					//all is a wall
					on_wall = true;
				} else {
					if (Math::acos(collision.normal.dot(up_direction)) <= p_floor_max_angle + FLOOR_ANGLE_THRESHOLD) { //floor

						on_floor = true;
						floor_normal = collision.normal;
						on_floor_body = collision.collider_rid;
						floor_velocity = collision.collider_vel;

						if (p_stop_on_slope) {
							if ((body_velocity_normal + up_direction).length() < 0.01 && collision.travel.length() < 1) {
								Transform gt = get_global_transform();
								gt.origin -= collision.travel.slide(up_direction);
								set_global_transform(gt);
								return Vector3();
							}
						}
					} else if (Math::acos(collision.normal.dot(-up_direction)) <= p_floor_max_angle + FLOOR_ANGLE_THRESHOLD) { //ceiling
						on_ceiling = true;
					} else {
						on_wall = true;
					}
				}

				motion = motion.slide(collision.normal);
				body_velocity = body_velocity.slide(collision.normal);

				for (int j = 0; j < 3; j++) {
					if (locked_axis & (1 << j)) {
						body_velocity[j] = 0;
					}
				}
			}
		}

		if (!found_collision || motion == Vector3()) {
			break;
		}

		--p_max_slides;
	}

	return body_velocity;
}

Vector3 KinematicBody3D::move_and_slide_with_snap(const Vector3 &p_linear_velocity, const Vector3 &p_snap, const Vector3 &p_up_direction, bool p_stop_on_slope, int p_max_slides, float p_floor_max_angle, bool p_infinite_inertia) {
	Vector3 up_direction = p_up_direction.normalized();
	bool was_on_floor = on_floor;

	Vector3 ret = move_and_slide(p_linear_velocity, up_direction, p_stop_on_slope, p_max_slides, p_floor_max_angle, p_infinite_inertia);
	if (!was_on_floor || p_snap == Vector3()) {
		return ret;
	}

	Collision col;
	Transform gt = get_global_transform();

	if (move_and_collide(p_snap, p_infinite_inertia, col, false, true)) {
		bool apply = true;
		if (up_direction != Vector3()) {
			if (Math::acos(col.normal.dot(up_direction)) <= p_floor_max_angle + FLOOR_ANGLE_THRESHOLD) {
				on_floor = true;
				floor_normal = col.normal;
				on_floor_body = col.collider_rid;
				floor_velocity = col.collider_vel;
				if (p_stop_on_slope) {
					// move and collide may stray the object a bit because of pre un-stucking,
					// so only ensure that motion happens on floor direction in this case.
					col.travel = col.travel.project(up_direction);
				}
			} else {
				apply = false; //snapped with floor direction, but did not snap to a floor, do not snap.
			}
		}
		if (apply) {
			gt.origin += col.travel;
			set_global_transform(gt);
		}
	}

	return ret;
}

bool KinematicBody3D::is_on_floor() const {
	return on_floor;
}

bool KinematicBody3D::is_on_wall() const {
	return on_wall;
}

bool KinematicBody3D::is_on_ceiling() const {
	return on_ceiling;
}

Vector3 KinematicBody3D::get_floor_normal() const {
	return floor_normal;
}

Vector3 KinematicBody3D::get_floor_velocity() const {
	return floor_velocity;
}

bool KinematicBody3D::test_move(const Transform &p_from, const Vector3 &p_motion, bool p_infinite_inertia) {
	ERR_FAIL_COND_V(!is_inside_tree(), false);

	return PhysicsServer3D::get_singleton()->body_test_motion(get_rid(), p_from, p_motion, p_infinite_inertia);
}

bool KinematicBody3D::separate_raycast_shapes(bool p_infinite_inertia, Collision &r_collision) {
	PhysicsServer3D::SeparationResult sep_res[8]; //max 8 rays

	Transform gt = get_global_transform();

	Vector3 recover;
	int hits = PhysicsServer3D::get_singleton()->body_test_ray_separation(get_rid(), gt, p_infinite_inertia, recover, sep_res, 8, margin);
	int deepest = -1;
	float deepest_depth;
	for (int i = 0; i < hits; i++) {
		if (deepest == -1 || sep_res[i].collision_depth > deepest_depth) {
			deepest = i;
			deepest_depth = sep_res[i].collision_depth;
		}
	}

	gt.origin += recover;
	set_global_transform(gt);

	if (deepest != -1) {
		r_collision.collider = sep_res[deepest].collider_id;
		r_collision.collider_metadata = sep_res[deepest].collider_metadata;
		r_collision.collider_shape = sep_res[deepest].collider_shape;
		r_collision.collider_vel = sep_res[deepest].collider_velocity;
		r_collision.collision = sep_res[deepest].collision_point;
		r_collision.normal = sep_res[deepest].collision_normal;
		r_collision.local_shape = sep_res[deepest].collision_local_shape;
		r_collision.travel = recover;
		r_collision.remainder = Vector3();

		return true;
	} else {
		return false;
	}
}

void KinematicBody3D::set_axis_lock(PhysicsServer3D::BodyAxis p_axis, bool p_lock) {
	if (p_lock) {
		locked_axis |= p_axis;
	} else {
		locked_axis &= (~p_axis);
	}
	PhysicsServer3D::get_singleton()->body_set_axis_lock(get_rid(), p_axis, p_lock);
}

bool KinematicBody3D::get_axis_lock(PhysicsServer3D::BodyAxis p_axis) const {
	return PhysicsServer3D::get_singleton()->body_is_axis_locked(get_rid(), p_axis);
}

void KinematicBody3D::set_safe_margin(float p_margin) {
	margin = p_margin;
	PhysicsServer3D::get_singleton()->body_set_kinematic_safe_margin(get_rid(), margin);
}

float KinematicBody3D::get_safe_margin() const {
	return margin;
}

int KinematicBody3D::get_slide_count() const {
	return colliders.size();
}

KinematicBody3D::Collision KinematicBody3D::get_slide_collision(int p_bounce) const {
	ERR_FAIL_INDEX_V(p_bounce, colliders.size(), Collision());
	return colliders[p_bounce];
}

Ref<KinematicCollision3D> KinematicBody3D::_get_slide_collision(int p_bounce) {
	ERR_FAIL_INDEX_V(p_bounce, colliders.size(), Ref<KinematicCollision3D>());
	if (p_bounce >= slide_colliders.size()) {
		slide_colliders.resize(p_bounce + 1);
	}

	if (slide_colliders[p_bounce].is_null()) {
		slide_colliders.write[p_bounce].instance();
		slide_colliders.write[p_bounce]->owner = this;
	}

	slide_colliders.write[p_bounce]->collision = colliders[p_bounce];
	return slide_colliders[p_bounce];
}

void KinematicBody3D::_notification(int p_what) {
	if (p_what == NOTIFICATION_ENTER_TREE) {
		// Reset move_and_slide() data.
		on_floor = false;
		on_floor_body = RID();
		on_ceiling = false;
		on_wall = false;
		colliders.clear();
		floor_velocity = Vector3();
	}
}

void KinematicBody3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_direct_state_changed"), &KinematicBody3D::_direct_state_changed);

	ClassDB::bind_method(D_METHOD("move_and_collide", "rel_vec", "infinite_inertia", "exclude_raycast_shapes", "test_only"), &KinematicBody3D::_move, DEFVAL(true), DEFVAL(true), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("move_and_slide", "linear_velocity", "up_direction", "stop_on_slope", "max_slides", "floor_max_angle", "infinite_inertia"), &KinematicBody3D::move_and_slide, DEFVAL(Vector3(0, 0, 0)), DEFVAL(false), DEFVAL(4), DEFVAL(Math::deg2rad((float)45)), DEFVAL(true));
	ClassDB::bind_method(D_METHOD("move_and_slide_with_snap", "linear_velocity", "snap", "up_direction", "stop_on_slope", "max_slides", "floor_max_angle", "infinite_inertia"), &KinematicBody3D::move_and_slide_with_snap, DEFVAL(Vector3(0, 0, 0)), DEFVAL(false), DEFVAL(4), DEFVAL(Math::deg2rad((float)45)), DEFVAL(true));

	ClassDB::bind_method(D_METHOD("test_move", "from", "rel_vec", "infinite_inertia"), &KinematicBody3D::test_move, DEFVAL(true));

	ClassDB::bind_method(D_METHOD("is_on_floor"), &KinematicBody3D::is_on_floor);
	ClassDB::bind_method(D_METHOD("is_on_ceiling"), &KinematicBody3D::is_on_ceiling);
	ClassDB::bind_method(D_METHOD("is_on_wall"), &KinematicBody3D::is_on_wall);
	ClassDB::bind_method(D_METHOD("get_floor_normal"), &KinematicBody3D::get_floor_normal);
	ClassDB::bind_method(D_METHOD("get_floor_velocity"), &KinematicBody3D::get_floor_velocity);

	ClassDB::bind_method(D_METHOD("set_axis_lock", "axis", "lock"), &KinematicBody3D::set_axis_lock);
	ClassDB::bind_method(D_METHOD("get_axis_lock", "axis"), &KinematicBody3D::get_axis_lock);

	ClassDB::bind_method(D_METHOD("set_safe_margin", "pixels"), &KinematicBody3D::set_safe_margin);
	ClassDB::bind_method(D_METHOD("get_safe_margin"), &KinematicBody3D::get_safe_margin);

	ClassDB::bind_method(D_METHOD("get_slide_count"), &KinematicBody3D::get_slide_count);
	ClassDB::bind_method(D_METHOD("get_slide_collision", "slide_idx"), &KinematicBody3D::_get_slide_collision);

	ADD_GROUP("Axis Lock", "axis_lock_");
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "axis_lock_motion_x"), "set_axis_lock", "get_axis_lock", PhysicsServer3D::BODY_AXIS_LINEAR_X);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "axis_lock_motion_y"), "set_axis_lock", "get_axis_lock", PhysicsServer3D::BODY_AXIS_LINEAR_Y);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "axis_lock_motion_z"), "set_axis_lock", "get_axis_lock", PhysicsServer3D::BODY_AXIS_LINEAR_Z);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "collision/safe_margin", PROPERTY_HINT_RANGE, "0.001,256,0.001"), "set_safe_margin", "get_safe_margin");
}

void KinematicBody3D::_direct_state_changed(Object *p_state) {
#ifdef DEBUG_ENABLED
	PhysicsDirectBodyState3D *state = Object::cast_to<PhysicsDirectBodyState3D>(p_state);
#else
	PhysicsDirectBodyState3D *state = (PhysicsDirectBodyState3D *)p_state; //trust it
#endif

	linear_velocity = state->get_linear_velocity();
	angular_velocity = state->get_angular_velocity();
}

KinematicBody3D::KinematicBody3D() :
		PhysicsBody3D(PhysicsServer3D::BODY_MODE_KINEMATIC) {
	locked_axis = 0;
	on_floor = false;
	on_ceiling = false;
	on_wall = false;

	set_safe_margin(0.001);
	PhysicsServer3D::get_singleton()->body_set_force_integration_callback(get_rid(), this, "_direct_state_changed");
}

KinematicBody3D::~KinematicBody3D() {
	if (motion_cache.is_valid()) {
		motion_cache->owner = nullptr;
	}

	for (int i = 0; i < slide_colliders.size(); i++) {
		if (slide_colliders[i].is_valid()) {
			slide_colliders.write[i]->owner = nullptr;
		}
	}
}

///////////////////////////////////////

Vector3 KinematicCollision3D::get_position() const {
	return collision.collision;
}

Vector3 KinematicCollision3D::get_normal() const {
	return collision.normal;
}

Vector3 KinematicCollision3D::get_travel() const {
	return collision.travel;
}

Vector3 KinematicCollision3D::get_remainder() const {
	return collision.remainder;
}

Object *KinematicCollision3D::get_local_shape() const {
	if (!owner) {
		return nullptr;
	}
	uint32_t ownerid = owner->shape_find_owner(collision.local_shape);
	return owner->shape_owner_get_owner(ownerid);
}

Object *KinematicCollision3D::get_collider() const {
	if (collision.collider.is_valid()) {
		return ObjectDB::get_instance(collision.collider);
	}

	return nullptr;
}

ObjectID KinematicCollision3D::get_collider_id() const {
	return collision.collider;
}

Object *KinematicCollision3D::get_collider_shape() const {
	Object *collider = get_collider();
	if (collider) {
		CollisionObject3D *obj2d = Object::cast_to<CollisionObject3D>(collider);
		if (obj2d) {
			uint32_t ownerid = obj2d->shape_find_owner(collision.collider_shape);
			return obj2d->shape_owner_get_owner(ownerid);
		}
	}

	return nullptr;
}

int KinematicCollision3D::get_collider_shape_index() const {
	return collision.collider_shape;
}

Vector3 KinematicCollision3D::get_collider_velocity() const {
	return collision.collider_vel;
}

Variant KinematicCollision3D::get_collider_metadata() const {
	return Variant();
}

void KinematicCollision3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_position"), &KinematicCollision3D::get_position);
	ClassDB::bind_method(D_METHOD("get_normal"), &KinematicCollision3D::get_normal);
	ClassDB::bind_method(D_METHOD("get_travel"), &KinematicCollision3D::get_travel);
	ClassDB::bind_method(D_METHOD("get_remainder"), &KinematicCollision3D::get_remainder);
	ClassDB::bind_method(D_METHOD("get_local_shape"), &KinematicCollision3D::get_local_shape);
	ClassDB::bind_method(D_METHOD("get_collider"), &KinematicCollision3D::get_collider);
	ClassDB::bind_method(D_METHOD("get_collider_id"), &KinematicCollision3D::get_collider_id);
	ClassDB::bind_method(D_METHOD("get_collider_shape"), &KinematicCollision3D::get_collider_shape);
	ClassDB::bind_method(D_METHOD("get_collider_shape_index"), &KinematicCollision3D::get_collider_shape_index);
	ClassDB::bind_method(D_METHOD("get_collider_velocity"), &KinematicCollision3D::get_collider_velocity);
	ClassDB::bind_method(D_METHOD("get_collider_metadata"), &KinematicCollision3D::get_collider_metadata);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "position"), "", "get_position");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "normal"), "", "get_normal");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "travel"), "", "get_travel");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "remainder"), "", "get_remainder");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "local_shape"), "", "get_local_shape");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collider"), "", "get_collider");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collider_id"), "", "get_collider_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collider_shape"), "", "get_collider_shape");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collider_shape_index"), "", "get_collider_shape_index");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "collider_velocity"), "", "get_collider_velocity");
	ADD_PROPERTY(PropertyInfo(Variant::NIL, "collider_metadata", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), "", "get_collider_metadata");
}

KinematicCollision3D::KinematicCollision3D() {
	collision.collider_shape = 0;
	collision.local_shape = 0;
	owner = nullptr;
}
