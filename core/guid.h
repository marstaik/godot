/*************************************************************************/
/*  guid.h                                                               */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           KENOS Project                               */
/*                      https://cosmicblimp.com                          */
/*************************************************************************/
/* Copyright (c) 2019-2020 Marios Staikopoulos, CosmicBlimp.             */
/*                                                                       */
/* Not allowed for public use. Do not distribute.                        */
/*************************************************************************/

#ifndef GUID_H
#define GUID_H

#include "core/typedefs.h"
#include "core/ustring.h"
#include "thirdparty/misc/guid.hpp"

class String;

/*
 * @author Marios Staikopoulos
 */

class Guid {
	xg::Guid id;

	Guid(xg::Guid &&p_id) noexcept : id(std::move(p_id)) {}

public:
	Guid() {}

	Guid(const String &p_str);

	Guid(const std::array<uint8_t, 16> &p_arr) noexcept : id(p_arr) {}

	_FORCE_INLINE_ void init() { id = xg::newGuid(); }

	_FORCE_INLINE_ void operator=(const Guid &p_guid) { id = p_guid.id; }
	_FORCE_INLINE_ bool operator==(const Guid &p_guid) const { return id == p_guid.id; }
	_FORCE_INLINE_ bool operator!=(const Guid &p_guid) const { return id != p_guid.id; }

	_FORCE_INLINE_ uint8_t operator[](size_t i) const { return id.bytes()[i]; }
	_FORCE_INLINE_ uint8_t &operator[](size_t i) { return id.bytes()[i]; }

	operator String() const;

	_FORCE_INLINE_ uint64_t hash() const { return std::hash<xg::Guid>{}(id); }

	_FORCE_INLINE_ static Guid create() { return Guid(xg::newGuid()); }

	_FORCE_INLINE_ const uint8_t *data() { return reinterpret_cast<const uint8_t *>(&id.bytes()); }
};

#endif // GUID_H
