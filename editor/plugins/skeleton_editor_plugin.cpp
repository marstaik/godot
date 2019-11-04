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

void BoneTransformEditor::create_editors() {
	const Color section_color = get_color("prop_subsection", "Editor");
	const Color subsection_color = Color(section_color.r, section_color.g, section_color.b, section_color.a / 2);

	EditorInspectorSection *s1 = memnew(EditorInspectorSection);
	s1->setup("trf_properties", label, skeleton, section_color, true);
	add_child(s1);

	Label *l1 = memnew(Label("Translation"));
	s1->get_vbox()->add_child(l1);

	translation_grid = memnew(GridContainer());
	translation_grid->set_columns(3);
	s1->get_vbox()->add_child(translation_grid);

	Label *l2 = memnew(Label("Rotation"));
	s1->get_vbox()->add_child(l2);

	rotation_grid = memnew(GridContainer());
	rotation_grid->set_columns(3);
	s1->get_vbox()->add_child(rotation_grid);

	Label *l3 = memnew(Label("Scale"));
	s1->get_vbox()->add_child(l3);

	scale_grid = memnew(GridContainer());
	scale_grid->set_columns(3);
	s1->get_vbox()->add_child(scale_grid);

	Label *l4 = memnew(Label("Transform"));
	s1->get_vbox()->add_child(l4);

	transform_grid = memnew(GridContainer());
	transform_grid->set_columns(3);
	s1->get_vbox()->add_child(transform_grid);

	static const char *desc[12] = { "x", "y", "z", "x", "y", "z", "x", "y", "z", "x", "y", "z" };

	for (int i = 0; i < 3; ++i) {
		translation[i] = memnew(EditorSpinSlider());
		translation[i]->set_label(desc[i]);
		setup_spinner(translation[i], false);
		translation_grid->add_child(translation[i]);

		rotation[i] = memnew(EditorSpinSlider());
		rotation[i]->set_label(desc[i]);
		setup_spinner(rotation[i], false);
		rotation_grid->add_child(rotation[i]);

		scale[i] = memnew(EditorSpinSlider());
		scale[i]->set_label(desc[i]);
		setup_spinner(scale[i], false);
		scale_grid->add_child(scale[i]);
	}

	for (int i = 0; i < 12; ++i) {
		transform[i] = memnew(EditorSpinSlider());
		transform[i]->set_label(desc[i]);
		setup_spinner(transform[i], true);
		transform_grid->add_child(transform[i]);
	}
}

void BoneTransformEditor::setup_spinner(EditorSpinSlider *spinner, const bool is_transform_spinner) {
	spinner->set_flat(true);
	spinner->set_min(-10000);
	spinner->set_max(10000);
	spinner->set_step(0.001);
	spinner->set_hide_slider(true);
	spinner->set_allow_greater(true);
	spinner->set_allow_lesser(true);
	spinner->set_h_size_flags(SIZE_EXPAND_FILL);

	spinner->connect("value_changed", this, "_value_changed", varray(is_transform_spinner));
}

void BoneTransformEditor::_notification(int p_what) {
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

				if (transform[i] != nullptr) {
					transform[i]->set_custom_label_color(true, c);
				}
			}
			break;
		}
		case NOTIFICATION_SORT_CHILDREN: {
			const Ref<Font> font = get_font("font", "Tree");

			Point2 buffer;
			buffer.x += get_constant("inspector_margin", "Editor");
			buffer.y += font->get_height();
			buffer.y += get_constant("vseparation", "Tree");

			const float vector_height = translation_grid->get_size().y;
			const float transform_height = transform_grid->get_size().y;

			const float width = get_size().x - get_constant("inspector_margin", "Editor");

			background_rects[0] = Rect2(translation_grid->get_position() + buffer, Size2(width, vector_height));
			background_rects[1] = Rect2(rotation_grid->get_position() + buffer, Size2(width, vector_height));
			background_rects[2] = Rect2(scale_grid->get_position() + buffer, Size2(width, vector_height));
			background_rects[3] = Rect2(transform_grid->get_position() + buffer, Size2(width, transform_height));

			update();
			break;
		}
		case NOTIFICATION_DRAW: {
			const Color dark_color = get_color("dark_color_2", "Editor");

			for (int i = 0; i < 4; ++i) {
				draw_rect(background_rects[i], dark_color);
			}

			break;
		}
	}
}

void BoneTransformEditor::_value_changed(const double p_value, const bool p_from_transform) {
	if (updating)
		return;

	const Transform tform = compute_transform(p_from_transform);

	undo_redo->create_action(TTR("Set Bone Transform"), UndoRedo::MERGE_ENDS);
	undo_redo->add_undo_property(skeleton, property, skeleton->get(property));
	undo_redo->add_do_property(skeleton, property, tform);
	undo_redo->commit_action();
	//skeleton->set(property, compute_transform(slider));

	//_update_properties();
}

Transform BoneTransformEditor::compute_transform(const bool p_from_transform) const {

	// Last modified was a raw transform column...
	if (p_from_transform) {
		Transform tform;

		for (int i = 0; i < 9; ++i) {
			tform.basis[i / 3][i % 3] = transform[i]->get_value();
		}

		for (int i = 0; i < 3; ++i) {
			tform.origin[i] = transform[i + 9]->get_value();
		}

		return tform;
	}

	return Transform(
			Basis(Vector3(rotation[0]->get_value(), rotation[1]->get_value(), rotation[2]->get_value()),
					Vector3(scale[0]->get_value(), scale[1]->get_value(), scale[2]->get_value())),
			Vector3(translation[0]->get_value(), translation[1]->get_value(), translation[2]->get_value()));
}

void BoneTransformEditor::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_value_changed", "value"), &BoneTransformEditor::_value_changed);
}

void BoneTransformEditor::_update_properties() {
	if (updating)
		return;

	if (skeleton == nullptr)
		return;

	updating = true;

	Transform tform = skeleton->get(property);

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

	updating = false;
}

BoneTransformEditor::BoneTransformEditor(Skeleton *p_skeleton) :
		skeleton(p_skeleton),
		disabled(false),
		updating(false),
		translation(),
		rotation(),
		scale(),
		transform() {

	undo_redo = EditorNode::get_undo_redo();
}

void BoneTransformEditor::set_target(const String &p_prop) {
	property = p_prop;
}

void SkeletonEditor::_joint_tree_selection_changed() {
	TreeItem *selected = joint_tree->get_selected();
	const String path = selected->get_metadata(0);

	if (path.begins_with("bones/")) {
		const int b_idx = path.get_slicec('/', 1).to_int();
		const String bone_path = "bones/" + itos(b_idx) + "/";

		pose_editor->set_target(bone_path + "pose");
		rest_editor->set_target(bone_path + "rest");
	}

	_update_properties();
}

void SkeletonEditor::_update_properties() {
	if (rest_editor)
		rest_editor->_update_properties();
	if (pose_editor)
		pose_editor->_update_properties();
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

	pose_editor = memnew(BoneTransformEditor(skeleton));
	pose_editor->set_label("Bone Pose");
	add_child(pose_editor);

	rest_editor = memnew(BoneTransformEditor(skeleton));
	rest_editor->set_label("Bone Rest");
	add_child(rest_editor);

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
