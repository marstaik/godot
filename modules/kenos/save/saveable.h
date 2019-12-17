/*************************************************************************/
/*  saveable.h                                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           KENOS Project                               */
/*                      https://cosmicblimp.com                          */
/*************************************************************************/
/* Copyright (c) 2019-2020 Marios Staikopoulos, CosmicBlimp.             */
/*                                                                       */
/* Not allowed for public use. Do not distribute.                        */
/*************************************************************************/

/*
 * @author Marios Staikopoulos
 */

#ifndef SAVEABLE_H
#define SAVEABLE_H

#include "core/object.h"

// The following macros define a parallel functionality to that of
// Object's get_property_list
#define SAVECLASS(m_class, m_inherits)                                                                        \
protected:                                                                                                    \
	_FORCE_INLINE_ void (Saveable::*_get_get_property_save_list() const)(List<PropertyInfo> * p_list) const { \
		return (void (Saveable::*)(List<PropertyInfo> *) const) & m_class::_get_property_save_list;           \
	}                                                                                                         \
                                                                                                              \
	virtual void _get_property_save_listv(List<PropertyInfo> *p_list, bool p_reversed) const {                \
		if (!p_reversed) {                                                                                    \
			m_inherits::_get_property_save_listv(p_list, p_reversed);                                         \
		}                                                                                                     \
		if (m_class::_get_get_property_save_list() != m_inherits::_get_get_property_save_list()) {            \
			m_class::_get_property_save_list(p_list);                                                         \
		}                                                                                                     \
		if (p_reversed) {                                                                                     \
			m_inherits::_get_property_save_listv(p_list, p_reversed);                                         \
		}                                                                                                     \
	}                                                                                                         \
                                                                                                              \
private:

#define SAVEBASE(m_class)                                                                                    \
protected:                                                                                                   \
	virtual void _get_property_save_listv(List<PropertyInfo> *p_list, bool p_reversed) const {};             \
                                                                                                             \
	_FORCE_INLINE_ void (m_class::*_get_get_property_save_list() const)(List<PropertyInfo> * p_list) const { \
		return &m_class::_get_property_save_list;                                                            \
	}                                                                                                        \
                                                                                                             \
public:                                                                                                      \
	void get_property_save_list(List<PropertyInfo> *p_list, bool p_reversed = false) const;                  \
                                                                                                             \
private:

// The following is the base class for all Saveable entities,
// it creates a parallel get_property_save_list that dictates what
// properties should be written/loaded from the save file.
class Saveable {

protected:
	virtual void _get_property_save_listv(List<PropertyInfo> *p_list, bool p_reversed) const {};

	void _get_property_save_list(List<PropertyInfo> *p_list) const;

	_FORCE_INLINE_ void (Saveable::*_get_get_property_save_list() const)(List<PropertyInfo> *p_list) const {
		return &Saveable::_get_property_save_list;
	}

public:
	Saveable();

	void get_property_save_list(List<PropertyInfo> *p_list, bool p_reversed = false) const;
};

#endif // SAVEABLE_H
