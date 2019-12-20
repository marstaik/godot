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
#include "thirdparty/misc/guid.hpp"

/*
 * @author Marios Staikopoulos
 */

class Guid {
	xg::Guid id;

	Guid(xg::Guid &&p_id) noexcept : id(std::move(p_id)) {}

public:
	Guid() {}

	_FORCE_INLINE_ void init() { id = xg::newGuid(); }

	_FORCE_INLINE_ void operator=(const Guid &guid) { id = guid.id; }
	_FORCE_INLINE_ bool operator==(const Guid &guid) const { return id == guid.id; }
	_FORCE_INLINE_ bool operator!=(const Guid &guid) const { return id != guid.id; }

	_FORCE_INLINE_ uint64_t hash() const { return std::hash<xg::Guid>{}(id); }

	_FORCE_INLINE_ static Guid New() { return Guid(xg::newGuid()); }
};

#endif // GUID_H
