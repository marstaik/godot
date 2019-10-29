/*************************************************************************/
/*  skeleton_editor_plugin.h                                             */
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

#ifndef SKELETON_EDITOR_PLUGIN_H
#define SKELETON_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "scene/3d/skeleton.h"

class SkeletonEditorPlugin;
class EditorInspectorPluginSkeleton;

class TransformEditor : public VBoxContainer {
	GDCLASS(TransformEditor, VBoxContainer);

	EditorSpinSlider *translation[3];
	EditorSpinSlider *rotation[3];
	EditorSpinSlider *scale[3];

	EditorSpinSlider *transform[12];

	Object *object;
	String property;
	bool disabled;
	String label;

	void create_editors();
	void setup_spinner(EditorSpinSlider *spinner);

protected:
	void _notification(int p_what);

public:
	TransformEditor(Object *p_object);

	void set_object_and_target(Object *p_object, const String &p_prop);
	void _update_properties();
	void set_label(const String &p_label) { label = p_label; }
};

class BoneEditor : public VBoxContainer {
	GDCLASS(BoneEditor, VBoxContainer);

	Skeleton *skeleton;
	BoneId bone_id;

	TransformEditor *rest;
	TransformEditor *pose;

	bool rest_disabled;

public:
	BoneEditor(Skeleton *p_skeleton);

	void set_bone(const BoneId p_bone_id);

	void set_rest_disabled(const bool p_disabled);
	void _update_properties();
};

class SkeletonEditor : public VBoxContainer {

	GDCLASS(SkeletonEditor, VBoxContainer);

	EditorNode *editor;
	EditorInspectorPluginSkeleton *editor_plugin;

	Skeleton *skeleton;

	Tree *joint_tree;
	BoneEditor *bone_editor;
	EditorPropertyResource *skeleton_def_editor;

	UndoRedo *undo_redo;

	void update_joint_tree();
	void update_editors();

	void create_editors();

protected:
	void _notification(int p_what);
	void _node_removed(Node *p_node);
	static void _bind_methods();

public:
	Skeleton *get_skeleton() const { return skeleton; };

	void set_undo_redo(UndoRedo *p_undo_redo) { undo_redo = p_undo_redo; }

	void _joint_tree_selection_changed();

	void _update_properties();

	SkeletonEditor(EditorInspectorPluginSkeleton *e_plugin, EditorNode *p_editor, Skeleton *skeleton);
	~SkeletonEditor();
};

class EditorInspectorPluginSkeleton : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginSkeleton, EditorInspectorPlugin);

	friend class SkeletonEditorPlugin;

	EditorNode *editor;

public:
	virtual bool can_handle(Object *p_object);
	virtual void parse_begin(Object *p_object);
};

class SkeletonEditorPlugin : public EditorPlugin {
	GDCLASS(SkeletonEditorPlugin, EditorPlugin);

	EditorNode *editor;

public:
	SkeletonEditorPlugin(EditorNode *p_node);

	virtual String get_name() const { return "Skeleton"; }
};

#endif // SKELETON_EDITOR_PLUGIN_H
