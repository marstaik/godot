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

#include "scene/3d/collision_shape.h"
#include "scene/3d/physics_body.h"
#include "scene/3d/physics_joint.h"
#include "scene/resources/capsule_shape.h"
#include "scene/resources/sphere_shape.h"
#include "spatial_editor_plugin.h"

SkeletonEditor *SkeletonEditor::singleton = nullptr;

void SkeletonEditor::edit(Skeleton *p_node) {

	skeleton = p_node;

	update_tree();
}

void SkeletonEditor::update_tree() {
	joint_tree->clear();

	if (skeleton == nullptr)
		return;

	TreeItem* root = joint_tree->create_item();

	Map<int, TreeItem*> items;

	items.insert(-1, root);

	const Vector<int>& joint_porder = skeleton->get_process_order();

	Ref<Texture> bone_icon = get_icon("BoneAttachment", "EditorIcons");

	for (int i = 0; i < joint_porder.size(); ++i) {
		const int b_idx = joint_porder[i];

		const int p_idx = skeleton->get_bone_parent(b_idx);
		TreeItem* p_item = items.find(p_idx)->get();

		TreeItem* joint_item = joint_tree->create_item(p_item);
		items.insert(b_idx, joint_item);

		joint_item->set_text(0, skeleton->get_bone_name(b_idx));
		joint_item->set_icon(0, bone_icon);
		joint_item->set_selectable(0, true);
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
}

SkeletonEditor::SkeletonEditor(EditorNode *p_editor, SkeletonEditorPlugin *p_plugin) :
		editor(p_editor),
		plugin(p_plugin) {

	singleton = this;
	skeleton = nullptr;

	set_focus_mode(FOCUS_ALL);

	VBoxContainer *root = this;

	HBoxContainer* hbox = memnew(HBoxContainer);
	hbox->set_v_size_flags(SIZE_EXPAND_FILL);
	hbox->set_h_size_flags(SIZE_EXPAND_FILL);
	root->add_child(hbox);

	joint_tree = memnew(Tree);
	joint_tree->set_columns(1);
	joint_tree->set_select_mode(Tree::SELECT_ROW);
	joint_tree->set_hide_root(true);
	joint_tree->set_v_size_flags(SIZE_EXPAND_FILL);
	joint_tree->set_h_size_flags(SIZE_EXPAND_FILL);
	joint_tree->set_allow_rmb_select(true);
	hbox->add_child(joint_tree);

	/*tform_editor = memnew(EditorPropertyTransform);
	tform_editor->set_object_and_property(skeleton, "/bone/0/pose");
	tform_editor->set_h_size_flags(SIZE_EXPAND);
	tform_editor->set_v_size_flags(SIZE_EXPAND_FILL);
	hbox->add_child(tform_editor);*/
}

SkeletonEditor::~SkeletonEditor() {}

void SkeletonEditorPlugin::edit(Object *p_object) {
	skeleton_editor->set_undo_redo(&get_undo_redo());
	if (!p_object)
		return;
	skeleton_editor->edit(Object::cast_to<Skeleton>(p_object));
}

bool SkeletonEditorPlugin::handles(Object *p_object) const {
	return p_object->is_class("Skeleton");
}

void SkeletonEditorPlugin::make_visible(bool p_visible) {
	if (p_visible) {

		editor->make_bottom_panel_item_visible(skeleton_editor);
		skeleton_editor->set_process(true);
	}
}

SkeletonEditorPlugin::SkeletonEditorPlugin(EditorNode *p_node) {
	editor = p_node;
	skeleton_editor = memnew(SkeletonEditor(editor, this));
	skeleton_editor->set_undo_redo(EditorNode::get_undo_redo());
	editor->add_bottom_panel_item(TTR("Skeleton"), skeleton_editor);
}

SkeletonEditorPlugin::~SkeletonEditorPlugin() {}
