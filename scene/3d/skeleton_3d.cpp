/*************************************************************************/
/*  skeleton_3d.cpp                                                      */
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

#include "skeleton_3d.h"

#include "core/engine.h"
#include "core/message_queue.h"
#include "core/project_settings.h"
#include "core/type_info.h"
#include "scene/3d/physics_body_3d.h"
#include "scene/resources/surface_tool.h"
#include "scene/scene_string_names.h"

void SkinReference::_skin_changed() {
	if (skeleton_node) {
		skeleton_node->dirty = true;
	}
	skeleton_version = 0;
}

void SkinReference::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_skin_changed"), &SkinReference::_skin_changed);
	ClassDB::bind_method(D_METHOD("get_skeleton"), &SkinReference::get_skeleton);
	ClassDB::bind_method(D_METHOD("get_skin"), &SkinReference::get_skin);
}

RID SkinReference::get_skeleton() const {
	return skeleton;
}

Ref<Skin> SkinReference::get_skin() const {
	return skin;
}

SkinReference::~SkinReference() {
	if (skeleton_node) {
		skeleton_node->skin_bindings.erase(this);
	}

	RS::get_singleton()->free(skeleton);
}

///////////////////////////////////////

bool Skeleton3D::_set(const StringName &p_path, const Variant &p_value) {
	String path = p_path;

	if (!path.begins_with("bones/")) {
		return false;
	}

	int which = path.get_slicec('/', 1).to_int();
	String what = path.get_slicec('/', 2);

	if (which == bones.size() && what == "name") {
		add_bone(p_value);
		return true;
	}

	ERR_FAIL_INDEX_V(which, bones.size(), false);

	if (what == "parent") {
		set_bone_parent(which, p_value);
	} else if (what == "rest") {
		set_bone_rest(which, p_value);
	} else if (what == "enabled") {
		set_bone_enabled(which, p_value);
	} else if (what == "pose") {
		set_bone_pose(which, p_value);
	} else if (what == "bound_children") {
		Array children = p_value;

		if (is_inside_tree()) {
			bones.write[which].nodes_bound.clear();

			for (int i = 0; i < children.size(); i++) {
				NodePath npath = children[i];
				ERR_CONTINUE(npath.operator String() == "");
				Node *node = get_node(npath);
				ERR_CONTINUE(!node);
				bind_child_node_to_bone(which, node);
			}
		}
	} else {
		return false;
	}

	return true;
}

bool Skeleton3D::_get(const StringName &p_path, Variant &r_ret) const {
	String path = p_path;

	if (!path.begins_with("bones/")) {
		return false;
	}

	int which = path.get_slicec('/', 1).to_int();
	String what = path.get_slicec('/', 2);

	ERR_FAIL_INDEX_V(which, bones.size(), false);

	if (what == "name") {
		r_ret = get_bone_name(which);
	} else if (what == "parent") {
		r_ret = get_bone_parent(which);
	} else if (what == "rest") {
		r_ret = get_bone_rest(which);
	} else if (what == "enabled") {
		r_ret = is_bone_enabled(which);
	} else if (what == "pose") {
		r_ret = get_bone_pose(which);
	} else if (what == "bound_children") {
		Array children;

		for (const List<ObjectID>::Element *E = bones[which].nodes_bound.front(); E; E = E->next()) {
			Object *obj = ObjectDB::get_instance(E->get());
			ERR_CONTINUE(!obj);
			Node *node = Object::cast_to<Node>(obj);
			ERR_CONTINUE(!node);
			NodePath npath = get_path_to(node);
			children.push_back(npath);
		}

		r_ret = children;
	} else {
		return false;
	}

	return true;
}

void Skeleton3D::_get_property_list(List<PropertyInfo> *p_list) const {
	for (int i = 0; i < bones.size(); i++) {
		String prep = "bones/" + itos(i) + "/";
		p_list->push_back(PropertyInfo(Variant::STRING, prep + "name", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR));
		p_list->push_back(PropertyInfo(Variant::INT, prep + "parent", PROPERTY_HINT_RANGE, "-1," + itos(bones.size() - 1) + ",1", PROPERTY_USAGE_NOEDITOR));
		p_list->push_back(PropertyInfo(Variant::TRANSFORM, prep + "rest", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR));
		p_list->push_back(PropertyInfo(Variant::BOOL, prep + "enabled", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR));
		p_list->push_back(PropertyInfo(Variant::TRANSFORM, prep + "pose", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR));
		p_list->push_back(PropertyInfo(Variant::ARRAY, prep + "bound_children", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR));
	}
}

void Skeleton3D::_update_process_order() {
	if (!process_order_dirty) {
		return;
	}

	Bone *bonesptr = bones.ptrw();
	int len = bones.size();

	process_order.resize(len);
	int *order = process_order.ptrw();
	for (int i = 0; i < len; i++) {
		if (bonesptr[i].parent >= len) {
			//validate this just in case
			ERR_PRINT("Bone " + itos(i) + " has invalid parent: " + itos(bonesptr[i].parent));
			bonesptr[i].parent = -1;
		}
		order[i] = i;
		bonesptr[i].sort_index = i;
	}
	//now check process order
	int pass_count = 0;
	while (pass_count < len * len) {
		//using bubblesort because of simplicity, it won't run every frame though.
		//bublesort worst case is O(n^2), and this may be an infinite loop if cyclic
		bool swapped = false;
		for (int i = 0; i < len; i++) {
			int parent_idx = bonesptr[order[i]].parent;
			if (parent_idx < 0) {
				continue; //do nothing because it has no parent
			}
			//swap indices
			int parent_order = bonesptr[parent_idx].sort_index;
			if (parent_order > i) {
				bonesptr[order[i]].sort_index = parent_order;
				bonesptr[parent_idx].sort_index = i;
				//swap order
				SWAP(order[i], order[parent_order]);
				swapped = true;
			}
		}

		if (!swapped) {
			break;
		}
		pass_count++;
	}

	if (pass_count == len * len) {
		ERR_PRINT("Skeleton3D parenthood graph is cyclic");
	}

	process_order_dirty = false;
}

bool Skeleton3D::update_bone_pose(Bone &p_bone, const Bone *p_bonesptr) const {
	if (p_bone.parent >= 0) {
		p_bone.dirty |= p_bonesptr[p_bone.parent].dirty;
	}

	if (!p_bone.dirty) {
		return false;
	}

	Transform computed_pose;

	// we have a parent, we need its global pose
	if (p_bone.parent >= 0) {
		computed_pose = p_bonesptr[p_bone.parent].pose_global_final;
	}

	if (!p_bone.disable_rest) {
		computed_pose *= p_bone.rest;
	}

	// finalize the global pose
	if (p_bone.enabled) {
		computed_pose *= p_bone.pose;
	}

	p_bone.pose_global_no_override = computed_pose;

	//----------------------------
	// compute the final pose
	//----------------------------

	const real_t override_amount = p_bone.global_pose_override_amount;

	if (p_bone.global_pose_override_reset) {
		p_bone.global_pose_override_amount = 0.0;
	}

	// if the override is near 1, we simply set the final pose to the override
	if (override_amount >= (1.0 - CMP_EPSILON)) {
		p_bone.pose_global_final = p_bone.global_pose_override;
		return true;
	} else if (override_amount >= CMP_EPSILON) {
		p_bone.pose_global_final = computed_pose.interpolate_with(p_bone.global_pose_override, override_amount);
		return true;
	}

	p_bone.pose_global_final = computed_pose;
	return true;
}

void Skeleton3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_INTERNAL_PROCESS: {
			if (!process_order_dirty && !dirty)
				break;

			_update_process_order();

			RenderingServer *rs = RenderingServer::get_singleton();
			Bone *bonesptr = bones.ptrw();
			const int bones_len = bones.size();

			const int *order = process_order.ptr();

			for (int i = 0; i < bones_len; i++) {
				Bone &b = bonesptr[order[i]];

				if (!update_bone_pose(b, bonesptr)) {
					continue;
				}

				for (List<ObjectID>::Element *E = b.nodes_bound.front(); E; E = E->next()) {
					Object *obj = ObjectDB::get_instance(E->get());
					ERR_CONTINUE(!obj);
					Node3D *node_3d = Object::cast_to<Node3D>(obj);
					ERR_CONTINUE(!node_3d);
					node_3d->set_transform(b.pose_global_final);
				}
			}

			//update skins
			for (Set<SkinReference *>::Element *E = skin_bindings.front(); E; E = E->next()) {
				const Skin *skin = E->get()->skin.operator->();
				RID skeleton = E->get()->skeleton;
				uint32_t bind_count = skin->get_bind_count();

				if (E->get()->bind_count != bind_count) {
					rs->skeleton_allocate(skeleton, bind_count);
					E->get()->bind_count = bind_count;
					E->get()->skin_bone_indices.resize(bind_count);
					E->get()->skin_bone_indices_ptrs = E->get()->skin_bone_indices.ptrw();
				}

				bool full_update = false;

				if (E->get()->skeleton_version != version) {
					full_update = true;
					for (uint32_t i = 0; i < bind_count; i++) {
						StringName bind_name = skin->get_bind_name(i);

						if (bind_name != StringName()) {
							//bind name used, use this
							bool found = false;
							for (int j = 0; j < bones_len; j++) {
								if (bonesptr[j].name == bind_name) {
									E->get()->skin_bone_indices_ptrs[i] = j;
									found = true;
									break;
								}
							}

							if (!found) {
								ERR_PRINT("Skin bind #" + itos(i) + " contains named bind '" + String(bind_name) + "' but Skeleton3D has no bone by that name.");
								E->get()->skin_bone_indices_ptrs[i] = 0;
							}
						} else if (skin->get_bind_bone(i) >= 0) {
							int bind_index = skin->get_bind_bone(i);
							if (bind_index >= bones_len) {
								ERR_PRINT("Skin bind #" + itos(i) + " contains bone index bind: " + itos(bind_index) + " , which is greater than the skeleton bone count: " + itos(bones_len) + ".");
								E->get()->skin_bone_indices_ptrs[i] = 0;
							} else {
								E->get()->skin_bone_indices_ptrs[i] = bind_index;
							}
						} else {
							ERR_PRINT("Skin bind #" + itos(i) + " does not contain a name nor a bone index.");
							E->get()->skin_bone_indices_ptrs[i] = 0;
						}
					}

					E->get()->skeleton_version = version;
				}

				for (uint32_t i = 0; i < bind_count; i++) {
					uint32_t bone_index = E->get()->skin_bone_indices_ptrs[i];
					ERR_CONTINUE(bone_index >= (uint32_t)bones_len);
					if (full_update || bonesptr[bone_index].dirty) {
						rs->skeleton_bone_set_transform(skeleton, i, bonesptr[bone_index].pose_global_final * skin->get_bind_pose(i));
					}
				}
			}

			// clear bone dirties
			for (int i = 0; i < bones_len; ++i) {
				bonesptr[i].dirty = false;
			}

			dirty = false;

#ifdef TOOLS_ENABLED
			emit_signal(SceneStringNames::get_singleton()->pose_updated);
#endif

		} break;
	}
}

void Skeleton3D::clear_bones_global_pose_override() {
	for (int i = 0; i < bones.size(); i += 1) {
		bones.write[i].global_pose_override_amount = 0;
	}
	dirty = true;
}

void Skeleton3D::set_bone_global_pose_override(int p_bone, const Transform &p_pose, float p_amount, bool p_persistent) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	bones.write[p_bone].dirty = true;
	bones.write[p_bone].global_pose_override_amount = p_amount;
	bones.write[p_bone].global_pose_override = p_pose;
	bones.write[p_bone].global_pose_override_reset = !p_persistent;
	dirty = true;
}

Transform Skeleton3D::get_bone_global_pose(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), Transform());
	return bones[p_bone].pose_global_final;
}

Transform Skeleton3D::get_bone_global_pose_without_override(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), Transform());
	return bones[p_bone].pose_global_no_override;
}

Transform Skeleton3D::get_bone_global_rest(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), Transform());
	Transform tform;
	do {
		tform *= bones[p_bone].rest.affine_inverse();
		p_bone = get_bone_parent(p_bone);
	} while (p_bone >= 0);

	return tform.affine_inverse();
}

// skeleton creation api
void Skeleton3D::add_bone(const String &p_name) {
	ERR_FAIL_COND(p_name == "" || p_name.find(":") != -1 || p_name.find("/") != -1);

	for (int i = 0; i < bones.size(); i++) {
		ERR_FAIL_COND(bones[i].name == p_name);
	}

	Bone b;
	b.name = p_name;
	bones.push_back(b);
	process_order_dirty = true;
	version++;
	dirty = true;
	update_gizmo();
}

int Skeleton3D::find_bone(const String &p_name) const {
	for (int i = 0; i < bones.size(); i++) {
		if (bones[i].name == p_name) {
			return i;
		}
	}

	return -1;
}

String Skeleton3D::get_bone_name(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), "");

	return bones[p_bone].name;
}

bool Skeleton3D::is_bone_parent_of(int p_bone, int p_parent_bone_id) const {
	int parent_of_bone = get_bone_parent(p_bone);

	if (-1 == parent_of_bone) {
		return false;
	}

	if (parent_of_bone == p_parent_bone_id) {
		return true;
	}

	return is_bone_parent_of(parent_of_bone, p_parent_bone_id);
}

int Skeleton3D::get_bone_count() const {
	return bones.size();
}

void Skeleton3D::set_bone_parent(int p_bone, int p_parent) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	ERR_FAIL_COND(p_parent != -1 && (p_parent < 0));

	Bone &bone = bones.write[p_bone];
	bone.dirty = true;
	bone.parent = p_parent;

	process_order_dirty = true;
	dirty = true;
}

void Skeleton3D::unparent_bone_and_rest(int p_bone) {
	ERR_FAIL_INDEX(p_bone, bones.size());

	_update_process_order();

	int parent = bones[p_bone].parent;
	while (parent >= 0) {
		bones.write[p_bone].rest = bones[parent].rest * bones[p_bone].rest;
		parent = bones[parent].parent;
	}

	bones.write[p_bone].dirty = true;
	bones.write[p_bone].parent = -1;
	process_order_dirty = true;

	dirty = true;
}

void Skeleton3D::set_bone_disable_rest(int p_bone, bool p_disable) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	bones.write[p_bone].dirty = true;
	bones.write[p_bone].disable_rest = p_disable;
	dirty = true;
}

bool Skeleton3D::is_bone_rest_disabled(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), false);
	return bones[p_bone].disable_rest;
}

int Skeleton3D::get_bone_parent(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), -1);

	return bones[p_bone].parent;
}

void Skeleton3D::set_bone_rest(int p_bone, const Transform &p_rest) {
	ERR_FAIL_INDEX(p_bone, bones.size());

	bones.write[p_bone].rest = p_rest;
	bones.write[p_bone].dirty = true;
	dirty = true;
}

Transform Skeleton3D::get_bone_rest(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), Transform());

	return bones[p_bone].rest;
}

void Skeleton3D::set_bone_enabled(int p_bone, bool p_enabled) {
	ERR_FAIL_INDEX(p_bone, bones.size());

	bones.write[p_bone].dirty = true;
	bones.write[p_bone].enabled = p_enabled;
	dirty = true;
}

bool Skeleton3D::is_bone_enabled(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), false);
	return bones[p_bone].enabled;
}

void Skeleton3D::bind_child_node_to_bone(int p_bone, Node *p_node) {
	ERR_FAIL_NULL(p_node);
	ERR_FAIL_INDEX(p_bone, bones.size());

	ObjectID id = p_node->get_instance_id();

	for (const List<ObjectID>::Element *E = bones[p_bone].nodes_bound.front(); E; E = E->next()) {
		if (E->get() == id) {
			return; // already here
		}
	}

	bones.write[p_bone].nodes_bound.push_back(id);
}

void Skeleton3D::unbind_child_node_from_bone(int p_bone, Node *p_node) {
	ERR_FAIL_NULL(p_node);
	ERR_FAIL_INDEX(p_bone, bones.size());

	ObjectID id = p_node->get_instance_id();
	bones.write[p_bone].nodes_bound.erase(id);
}

void Skeleton3D::get_bound_child_nodes_to_bone(int p_bone, List<Node *> *p_bound) const {
	ERR_FAIL_INDEX(p_bone, bones.size());

	for (const List<ObjectID>::Element *E = bones[p_bone].nodes_bound.front(); E; E = E->next()) {
		Object *obj = ObjectDB::get_instance(E->get());
		ERR_CONTINUE(!obj);
		p_bound->push_back(Object::cast_to<Node>(obj));
	}
}

void Skeleton3D::clear_bones() {
	bones.clear();
	process_order_dirty = true;
	dirty = true;
	version++;
}

// posing api

void Skeleton3D::set_bone_pose(int p_bone, const Transform &p_pose) {
	ERR_FAIL_INDEX(p_bone, bones.size());

	bones.write[p_bone].dirty = true;
	bones.write[p_bone].pose = p_pose;
	dirty = true;
}

Transform Skeleton3D::get_bone_pose(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), Transform());
	return bones[p_bone].pose;
}

void Skeleton3D::reset_bone_poses() {
	Skeleton3D::Bone *bones_w = bones.ptrw();
	const int size = bones.size();
	for (int i = 0; i < size; ++i) {
		bones_w[i].dirty = true;
		bones_w[i].pose = Transform();
		bones_w[i].global_pose_override = Transform();
		bones_w[i].global_pose_override_amount = 0.0;
		bones_w[i].global_pose_override_reset = false;
	}

	dirty = true;
}

int Skeleton3D::get_process_order(int p_idx) {
	ERR_FAIL_INDEX_V(p_idx, bones.size(), -1);
	_update_process_order();
	return process_order[p_idx];
}

Vector<int> Skeleton3D::get_bone_process_orders() {
	_update_process_order();
	return process_order;
}

void Skeleton3D::localize_rests() {
	_update_process_order();

	for (int i = bones.size() - 1; i >= 0; i--) {
		int idx = process_order[i];
		if (bones[idx].parent >= 0) {
			set_bone_rest(idx, bones[bones[idx].parent].rest.affine_inverse() * bones[idx].rest);
		}
	}
}

void Skeleton3D::_skin_changed() {
	dirty = true;
}

Ref<SkinReference> Skeleton3D::register_skin(const Ref<Skin> &p_skin) {
	for (Set<SkinReference *>::Element *E = skin_bindings.front(); E; E = E->next()) {
		if (E->get()->skin == p_skin) {
			return Ref<SkinReference>(E->get());
		}
	}

	Ref<Skin> skin = p_skin;

	if (skin.is_null()) {
		//need to create one from existing code, this is for compatibility only
		//when skeletons did not support skins. It is also used by gizmo
		//to display the skeleton.

		skin.instance();
		skin->set_bind_count(bones.size());
		_update_process_order(); //just in case

		// pose changed, rebuild cache of inverses
		const Bone *bonesptr = bones.ptr();
		int len = bones.size();
		const int *order = process_order.ptr();

		// calculate global rests and invert them
		for (int i = 0; i < len; i++) {
			const Bone &b = bonesptr[order[i]];
			if (b.parent >= 0) {
				skin->set_bind_pose(order[i], skin->get_bind_pose(b.parent) * b.rest);
			} else {
				skin->set_bind_pose(order[i], b.rest);
			}
		}

		for (int i = 0; i < len; i++) {
			//the inverse is what is actually required
			skin->set_bind_bone(i, i);
			skin->set_bind_pose(i, skin->get_bind_pose(i).affine_inverse());
		}
	}

	ERR_FAIL_COND_V(skin.is_null(), Ref<SkinReference>());

	Ref<SkinReference> skin_ref;
	skin_ref.instance();

	skin_ref->skeleton_node = this;
	skin_ref->bind_count = 0;
	skin_ref->skeleton = RenderingServer::get_singleton()->skeleton_create();
	skin_ref->skeleton_node = this;
	skin_ref->skin = skin;

	skin_bindings.insert(skin_ref.operator->());

	skin->connect_compat("changed", skin_ref.operator->(), "_skin_changed");

	dirty = true; //skin needs to be updated, so update skeleton

	return skin_ref;
}

// helper functions
Transform Skeleton3D::bone_transform_to_world_transform(Transform p_bone_transform) {
	return get_global_transform() * p_bone_transform;
}

Transform Skeleton3D::world_transform_to_bone_transform(Transform p_world_transform) {
	return get_global_transform().affine_inverse() * p_world_transform;
}

void Skeleton3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_bone_process_orders"), &Skeleton3D::get_bone_process_orders);
	ClassDB::bind_method(D_METHOD("add_bone", "name"), &Skeleton3D::add_bone);
	ClassDB::bind_method(D_METHOD("find_bone", "name"), &Skeleton3D::find_bone);
	ClassDB::bind_method(D_METHOD("get_bone_name", "bone_idx"), &Skeleton3D::get_bone_name);

	ClassDB::bind_method(D_METHOD("get_bone_parent", "bone_idx"), &Skeleton3D::get_bone_parent);
	ClassDB::bind_method(D_METHOD("set_bone_parent", "bone_idx", "parent_idx"), &Skeleton3D::set_bone_parent);

	ClassDB::bind_method(D_METHOD("get_bone_count"), &Skeleton3D::get_bone_count);

	ClassDB::bind_method(D_METHOD("unparent_bone_and_rest", "bone_idx"), &Skeleton3D::unparent_bone_and_rest);

	ClassDB::bind_method(D_METHOD("get_bone_rest", "bone_idx"), &Skeleton3D::get_bone_rest);
	ClassDB::bind_method(D_METHOD("set_bone_rest", "bone_idx", "rest"), &Skeleton3D::set_bone_rest);

	ClassDB::bind_method(D_METHOD("register_skin", "skin"), &Skeleton3D::register_skin);

	ClassDB::bind_method(D_METHOD("localize_rests"), &Skeleton3D::localize_rests);

	ClassDB::bind_method(D_METHOD("set_bone_disable_rest", "bone_idx", "disable"), &Skeleton3D::set_bone_disable_rest);
	ClassDB::bind_method(D_METHOD("is_bone_rest_disabled", "bone_idx"), &Skeleton3D::is_bone_rest_disabled);

	ClassDB::bind_method(D_METHOD("bind_child_node_to_bone", "bone_idx", "node"), &Skeleton3D::bind_child_node_to_bone);
	ClassDB::bind_method(D_METHOD("unbind_child_node_from_bone", "bone_idx", "node"), &Skeleton3D::unbind_child_node_from_bone);
	ClassDB::bind_method(D_METHOD("get_bound_child_nodes_to_bone", "bone_idx"), &Skeleton3D::_get_bound_child_nodes_to_bone);

	ClassDB::bind_method(D_METHOD("clear_bones"), &Skeleton3D::clear_bones);

	ClassDB::bind_method(D_METHOD("get_bone_pose", "bone_idx"), &Skeleton3D::get_bone_pose);
	ClassDB::bind_method(D_METHOD("set_bone_pose", "bone_idx", "pose"), &Skeleton3D::set_bone_pose);

	ClassDB::bind_method(D_METHOD("clear_bones_global_pose_override"), &Skeleton3D::clear_bones_global_pose_override);
	ClassDB::bind_method(D_METHOD("set_bone_global_pose_override", "bone_idx", "pose", "amount", "persistent"), &Skeleton3D::set_bone_global_pose_override, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_bone_global_pose", "bone_idx"), &Skeleton3D::get_bone_global_pose);
	ClassDB::bind_method(D_METHOD("get_bone_global_pose_without_override", "bone_idx"), &Skeleton3D::get_bone_global_pose_without_override);

	ClassDB::bind_method(D_METHOD("bone_transform_to_world_transform", "bone_transform"), &Skeleton3D::bone_transform_to_world_transform);
	ClassDB::bind_method(D_METHOD("world_transform_to_bone_transform", "world_transform"), &Skeleton3D::world_transform_to_bone_transform);

#ifdef TOOLS_ENABLED
	ADD_SIGNAL(MethodInfo("pose_updated"));
#endif
}

Skeleton3D::Skeleton3D() :
		process_order_dirty(1), dirty(true), version(1) {
	set_process_internal(true);
}

Skeleton3D::~Skeleton3D() {
	//some skins may remain bound
	for (Set<SkinReference *>::Element *E = skin_bindings.front(); E; E = E->next()) {
		E->get()->skeleton_node = nullptr;
	}
}
