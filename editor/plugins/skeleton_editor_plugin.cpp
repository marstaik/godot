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
#include "scene/resources/skeleton_definition.h"
#include "spatial_editor_plugin.h"

void TransformEditor::create_editors() {
	const Color section_color = get_color("prop_subsection", "Editor");
	const Color subsection_color = Color(section_color.r, section_color.g, section_color.b, section_color.a / 2);

	EditorInspectorSection *s1 = memnew(EditorInspectorSection);
	s1->setup("trf_properties", label, object, section_color, true);
	add_child(s1);

	Label *l1 = memnew(Label("Translation"));
	s1->get_vbox()->add_child(l1);

	GridContainer *c1 = memnew(GridContainer());
	c1->set_columns(3);
	s1->get_vbox()->add_child(c1);

	Label *l2 = memnew(Label("Rotation"));
	s1->get_vbox()->add_child(l2);

	GridContainer *c2 = memnew(GridContainer());
	c2->set_columns(3);
	s1->get_vbox()->add_child(c2);

	Label *l3 = memnew(Label("Scale"));
	s1->get_vbox()->add_child(l3);

	GridContainer *c3 = memnew(GridContainer());
	c3->set_columns(3);
	s1->get_vbox()->add_child(c3);

	Label *l4 = memnew(Label("Transform"));
	s1->get_vbox()->add_child(l4);

	GridContainer *c4 = memnew(GridContainer());
	c4->set_columns(3);
	s1->get_vbox()->add_child(c4);

	static const char *desc1[3] = { "x", "y", "z" };

	for (int i = 0; i < 3; ++i) {
		translation[i] = memnew(EditorSpinSlider());
		translation[i]->set_label(desc1[i]);
		setup_spinner(translation[i]);
		c1->add_child(translation[i]);

		rotation[i] = memnew(EditorSpinSlider());
		rotation[i]->set_label(desc1[i]);
		setup_spinner(rotation[i]);
		c2->add_child(rotation[i]);

		scale[i] = memnew(EditorSpinSlider());
		scale[i]->set_label(desc1[i]);
		setup_spinner(scale[i]);
		c3->add_child(scale[i]);
	}

	static const char *desc2[12] = { "x", "y", "z", "x", "y", "z", "x", "y", "z", "x", "y", "z" };
	for (int i = 0; i < 12; ++i) {
		transform[i] = memnew(EditorSpinSlider());
		transform[i]->set_label(desc2[i]);
		setup_spinner(transform[i]);
		c4->add_child(transform[i]);
	}
}

void TransformEditor::setup_spinner(EditorSpinSlider *spinner) {
	spinner->set_flat(true);
	spinner->set_min(-10000);
	spinner->set_max(10000);
	spinner->set_step(0.001);
	spinner->set_hide_slider(true);
	spinner->set_allow_greater(true);
	spinner->set_allow_lesser(true);
	spinner->set_h_size_flags(SIZE_EXPAND_FILL);
}

void TransformEditor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			create_editors();
			// fallthrough
		}
		case NOTIFICATION_THEME_CHANGED: {
			const Color base = get_color("accent_color", "Editor");
			const Color bg_color = get_color("property_color", "Editor");
			const Color bg_lbl_color(bg_color.r, bg_color.g, bg_color.b, 0.5);

			for (int i = 0; i < 12; i++) {
				Color c = base;
				c.set_hsv(float(i % 3) / 3.0 + 0.05, c.get_s() * 0.75, c.get_v());

				if (i < 3) {
					if (translation[i] != nullptr)
						translation[i]->set_custom_label_color(true, c);
					if (rotation[i] != nullptr)
						rotation[i]->set_custom_label_color(true, c);
					if (scale[i] != nullptr)
						scale[i]->set_custom_label_color(true, c);
				}

				if (transform[i] != nullptr)
					transform[i]->set_custom_label_color(true, c);
			}
		}
	}
}

void TransformEditor::_update_properties() {
	if (object == nullptr)
		return;

	Transform tform = object->get(property);

	Vector3 rot = tform.get_basis().get_rotation_euler();
	Vector3 sc = tform.get_basis().get_scale();
	Vector3 tr = tform.get_origin();

	for (int i = 0; i < 3; ++i) {
		translation[i]->set_value(tr[i]);
		rotation[i]->set_value(rot[i]);
		scale[i]->set_value(sc[i]);

		transform[9 + i]->set_value(tr[i]);
	}

	for (int i = 0; i < 9; ++i) {
		transform[i]->set_value(tform.get_basis()[i / 3][i % 3]);
	}
}

TransformEditor::TransformEditor(Object *p_object) :
		object(p_object),
		disabled(false),
		translation(),
		rotation(),
		scale(),
		transform() {
}

void TransformEditor::set_object_and_target(Object *p_object, const String &p_prop) {
	object = p_object;
	property = p_prop;
}

BoneEditor::BoneEditor(Skeleton *p_skeleton) :
		skeleton(p_skeleton) {

	add_constant_override("separation", 0);

	rest = memnew(TransformEditor(skeleton));
	rest->set_label("Bone Rest");
	add_child(rest);

	pose = memnew(TransformEditor(skeleton));
	pose->set_label("Bone Pose");
	add_child(pose);
}

void BoneEditor::set_bone(const BoneId p_bone_id) {
	bone_id = p_bone_id;
	rest->set_object_and_target(skeleton, "bones/" + itos(p_bone_id) + "/rest");
	pose->set_object_and_target(skeleton, "bones/" + itos(p_bone_id) + "/pose");
}

void BoneEditor::set_rest_disabled(const bool p_disabled) {
	rest_disabled = p_disabled;
}

void BoneEditor::_update_properties() {
	rest->_update_properties();
	pose->_update_properties();
}

void SkeletonEditor::_joint_tree_selection_changed() {
	TreeItem *selected = joint_tree->get_selected();
	const String path = selected->get_metadata(0);

	if (path.begins_with("bones/")) {
		const int b_idx = path.get_slicec('/', 1).to_int();
		const String bone_name = skeleton->get_bone_name(b_idx);

		bone_editor->set_bone(b_idx);
	}

	_update_properties();
}

void SkeletonEditor::_update_properties() {
	bone_editor->_update_properties();
}

void SkeletonEditor::update_joint_tree() {
	joint_tree->clear();

	if (skeleton == nullptr)
		return;

	TreeItem *root = joint_tree->create_item();

	Map<int, TreeItem *> items;

	items.insert(-1, root);

	const Vector<int> &joint_porder = skeleton->get_process_order();

	Ref<Texture> bone_icon = get_icon("BoneAttachment", "EditorIcons");

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

void SkeletonEditor::update_editors() {
	skeleton_def_editor->set_object_and_property(skeleton, "skeleton_definition");
}

void SkeletonEditor::create_editors() {
	set_h_size_flags(SIZE_EXPAND_FILL);
	add_constant_override("separation", 0);

	set_focus_mode(FOCUS_ALL);

	Ref<Texture> skel_icon = get_icon("Skeleton", "EditorIcons");
	Color category_color = get_color("prop_category", "Editor");
	Color section_color = get_color("prop_subsection", "Editor");
	Color subsection_color = Color(section_color.r, section_color.g, section_color.b, section_color.a / 2);

	EditorInspectorCategory *category = memnew(EditorInspectorCategory);
	category->setup("Skeleton", skel_icon, category_color);
	add_child(category);

	EditorInspectorSection *skeleton_def_section = memnew(EditorInspectorSection);
	skeleton_def_section->setup("skeleton_definition", "Skeleton Definition", skeleton, section_color, true);
	add_child(skeleton_def_section);

	skeleton_def_editor = memnew(EditorPropertyResource);
	skeleton_def_editor->set_h_size_flags(SIZE_EXPAND_FILL);
	skeleton_def_editor->setup("SkeletonDefinition");
	skeleton_def_section->get_vbox()->add_child(skeleton_def_editor);

	EditorInspectorSection *bones_section = memnew(EditorInspectorSection);
	add_child(bones_section);
	bones_section->setup("bones", "Bones", skeleton, section_color, true);

	ScrollContainer *s_con = memnew(ScrollContainer);
	bones_section->get_vbox()->add_child(s_con);
	s_con->set_h_size_flags(SIZE_EXPAND_FILL);
	s_con->set_custom_minimum_size(Size2(1, 350) * EDSCALE);

	joint_tree = memnew(Tree);
	s_con->add_child(joint_tree);
	joint_tree->set_columns(1);
	joint_tree->set_select_mode(Tree::SELECT_ROW);
	joint_tree->set_hide_root(true);
	joint_tree->set_v_size_flags(SIZE_EXPAND_FILL);
	joint_tree->set_h_size_flags(SIZE_EXPAND_FILL);
	joint_tree->set_allow_rmb_select(true);
	joint_tree->connect("item_selected", this, "_joint_tree_selection_changed");

	bone_editor = memnew(BoneEditor(skeleton));
	add_child(bone_editor);

#ifdef TOOLS_ENABLED
	skeleton->connect("pose_updated", this, "_update_properties");
#endif // TOOLS_ENABLED
}

void SkeletonEditor::_notification(int p_what) {

	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			create_editors();
			update_joint_tree();
			update_editors();

			get_tree()->connect("node_removed", this, "_node_removed");
			break;
		}
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
	ClassDB::bind_method(D_METHOD("_update_properties"), &SkeletonEditor::_update_properties);
}

SkeletonEditor::SkeletonEditor(EditorInspectorPluginSkeleton *e_plugin, EditorNode *p_editor, Skeleton *p_skeleton) :
		skeleton(p_skeleton),
		editor(p_editor),
		editor_plugin(e_plugin) {
}

SkeletonEditor::~SkeletonEditor() {}

bool EditorInspectorPluginSkeleton::can_handle(Object *p_object) {
	return Object::cast_to<Skeleton>(p_object) != nullptr;
}

void EditorInspectorPluginSkeleton::parse_begin(Object *p_object) {
	Skeleton *skeleton = Object::cast_to<Skeleton>(p_object);
	ERR_FAIL_COND(!skeleton);

	SkeletonEditor *skel_editor = memnew(SkeletonEditor(this, editor, skeleton));
	add_custom_control(skel_editor);
}

SkeletonEditorPlugin::SkeletonEditorPlugin(EditorNode *p_node) {
	editor = p_node;

	Ref<EditorInspectorPluginSkeleton> skeleton_plugin;
	skeleton_plugin.instance();
	skeleton_plugin->editor = editor;

	EditorInspector::add_inspector_plugin(skeleton_plugin);
}
