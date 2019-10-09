/*************************************************************************/
/*  skeleton_editor_plugin.cpp                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
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

#include "skeleton_editor_plugin.h"

//#include "scene/3d/collision_shape.h"
//#include "scene/3d/physics_body.h"
//#include "scene/3d/physics_joint.h"
//#include "scene/resources/capsule_shape.h"
//#include "scene/resources/sphere_shape.h"
#include "spatial_editor_plugin.h"

void SkeletonEditor::_joint_tree_selection_changed() {
	TreeItem *selected = joint_tree->get_selected();
	const String path = selected->get_metadata(0);

	if (path.begins_with("bones/")) {
		const int b_idx = path.get_slicec('/', 1).to_int();
		const String bone_name = skeleton->get_bone_name(b_idx);

		rest_editor->set_object_and_property(skeleton, "bones/" + itos(b_idx) + "/rest");
		rest_editor->update_property();
		rest_editor->set_label("Bone Rest: " + bone_name);
	}
}

void SkeletonEditor::add_controls(EditorInspectorPlugin *plugin) {
	plugin->add_custom_control(this);
	plugin->add_property_editor("Skeleton/rest_editor", rest_editor);
}

void SkeletonEditor::update_joint_tree() {
	joint_tree->clear();

	if (skeleton == nullptr)
		return;

	TreeItem *root = joint_tree->create_item();

	Map<int, TreeItem *> items;

	items.insert(-1, root);

	const Vector<int> &joint_porder = skeleton->get_process_order();

	bool icon_exists = has_icon("BoneAttachment", "EditorIcons");
	Ref<Texture> bone_icon = EditorNode::get_singleton()->get_class_icon("BoneAttachment");
	//const Ref<Texture> &bone_icon = get_icon("BoneAttachment", "EditorIcons");

	for (int i = 0; i < joint_porder.size(); ++i) {
		const int b_idx = joint_porder[i];

		const int p_idx = skeleton->get_bone_parent(b_idx);
		TreeItem *p_item = items.find(p_idx)->get();

		TreeItem *joint_item = joint_tree->create_item(p_item);
		items.insert(b_idx, joint_item);

		joint_item->set_text(0, skeleton->get_bone_name(b_idx));
		joint_item->set_icon(0, bone_icon);
		joint_item->set_selectable(0, true);
		joint_item->set_metadata(0, "bones/" + itos(b_idx));
	}
}

void SkeletonEditor::_notification(int p_what) {
	if (p_what == NOTIFICATION_ENTER_TREE) {
		get_tree()->connect("node_removed", this, "_node_removed");
	}
}

void SkeletonEditor::_node_removed(Node *p_node) {

	if (skeleton && p_node == skeleton) {
		skeleton = nullptr;
	}
}

void SkeletonEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_node_removed"), &SkeletonEditor::_node_removed);
	ClassDB::bind_method(D_METHOD("_joint_tree_selection_changed"), &SkeletonEditor::_joint_tree_selection_changed);
}

SkeletonEditor::SkeletonEditor(EditorInspectorPluginSkeleton *e_plugin, Skeleton *sk) :
		editor_plugin(e_plugin),
		skeleton(sk) {

	set_focus_mode(FOCUS_ALL);

	ScrollContainer *s_con = memnew(ScrollContainer);
	s_con->set_h_size_flags(SIZE_EXPAND_FILL);
	s_con->set_custom_minimum_size(Size2(1, 350) * EDSCALE);
	add_child(s_con);

	joint_tree = memnew(Tree);
	joint_tree->set_columns(1);
	joint_tree->set_select_mode(Tree::SELECT_ROW);
	joint_tree->set_hide_root(true);
	joint_tree->set_v_size_flags(SIZE_EXPAND_FILL);
	joint_tree->set_h_size_flags(SIZE_EXPAND_FILL);
	joint_tree->set_allow_rmb_select(true);
	s_con->add_child(joint_tree);

	rest_editor = memnew(EditorPropertyTransform);
	rest_editor->set_h_size_flags(SIZE_EXPAND_FILL);
	rest_editor->setup(-100000, 100000, 0.001, true);

	// Update the tree  at the end
	update_joint_tree();
	joint_tree->connect("item_selected", this, "_joint_tree_selection_changed");
}

SkeletonEditor::~SkeletonEditor() {}

bool EditorInspectorPluginSkeleton::can_handle(Object *p_object) {
	return Object::cast_to<Skeleton>(p_object) != nullptr;
}

void EditorInspectorPluginSkeleton::parse_begin(Object *p_object) {
	Skeleton *skeleton = Object::cast_to<Skeleton>(p_object);
	ERR_FAIL_COND(!skeleton);

	SkeletonEditor *editor = memnew(SkeletonEditor(this, skeleton));
	editor->add_controls(this);
}

SkeletonEditorPlugin::SkeletonEditorPlugin(EditorNode *p_node) {
	Ref<EditorInspectorPluginSkeleton> skeleton_plugin;
	skeleton_plugin.instance();
	EditorInspector::add_inspector_plugin(skeleton_plugin);
}
