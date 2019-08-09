/*************************************************************************/
/*  disjoint_set.h                                                       */
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

#ifndef DISJOINT_SET_H
#define DISJOINT_SET_H

#include "core/map.h"
#include "core/vector.h"

/**
	@author Marios Staikopoulos <marios@staik.net>
*/

/* This DisjointSet class uses Find with path compression and Union by rank */
template <typename T, class AL = DefaultAllocator>
class DisjointSet {

	struct Element {
		T object;
		const Element *parent = nullptr;
		int rank = 0;
	};

	typedef Map<T, Element *, AL> MapT;

	MapT elements;

	Element *find(Element *element);

	_FORCE_INLINE_ Element *add(T object);

public:
	_FORCE_INLINE_ void insert(T object) { (void)add(object); }

	void create_union(T a, T b);

	void find_roots(Vector<T> out_roots);
};

/* FUNCTIONS */

template <typename T, class AL>
typename DisjointSet<T, AL>::Element *DisjointSet<T, AL>::find(Element *element) {
	if (element->parent != element) {
		element->parent = find(element->parent);
	}

	return element->parent;
}

template <typename T, class AL>
typename DisjointSet<T, AL>::Element *DisjointSet<T, AL>::add(T object) {

	ERR_FAIL_COND(elements.find(object) != nullptr);

	Element *new_element = memnew_allocator(Element, AL);
	new_element->object = object;
	new_element->parent = new_element;
	elements.insert(object, new_element);

	return new_element;
}

template <typename T, class AL>
void DisjointSet<T, AL>::create_union(T a, T b) {

	MapT::Element *x_map_find = elements.find(a);
	MapT::Element *y_map_find = elements.find(b);

	Element *x, y;

	// Find, or create elements if they do not exist
	if (x_map_find == nullptr) {
		x = add(a);
	} else {
		x = x_map_find->value();
	}

	if (y_map_find == nullptr) {
		y = add(y);
	} else {
		y = y_map_find->value();
	}

	Element *x_root = find(x);
	Element *y_root = find(y);

	// Already in the same set
	if (x_root == y_root)
		return;

	// Not in the same set, merge
	if (x_root->rank < y_root->rank) {
		SWAP(x_root, y_root);
	}

	// Merge y_root into x_root
	y_root->parent = x_root;
	if (x_root->rank == y_root->rank) {
		++x_root->rank;
	}
}

template <typename T, class AL>
void DisjointSet<T, AL>::find_roots(Vector<T> out_roots) {
	for (MapT::Element *map_element = elements.front(); map_element != nullptr; map_element = map_element->next()) {
		Element *element = map_element->value();
		if (element->parent == element) {
			out_roots.push_back(element->object);
		}
	}
}

#endif
