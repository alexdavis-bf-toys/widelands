/*
 * Copyright (C) 2002, 2008 by the Widelands Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef WORLDDATA_H
#define WORLDDATA_H

// NOCOM(#sirver): what is needed in here?
#include <stdint.h>

#ifndef PACK_VERSION
#define PACK_VERSION(major, minor)                                            \
   (static_cast<uint16_t>((static_cast<uint8_t>(major)) << 8) |               \
    ((static_cast<uint8_t>(minor))))
#endif

#ifndef VERSION_MAJOR
#define VERSION_MAJOR(vers) \
   (static_cast<uint16_t>((static_cast<uint16_t>(vers) >> 8) & 0xff))
#endif

#ifndef VERSION_MINOR
#define VERSION_MINOR(vers) \
   (static_cast<uint16_t>((static_cast<uint16_t>(vers)) & 0xff))
#endif

#define WLWF_VERSION PACK_VERSION(1, 0)

enum TerrainType {
	TERRAIN_GREEN = 0,
	TERRAIN_DRY = 1,
	TERRAIN_WATER =  2,
	TERRAIN_ACID = 4,
	TERRAIN_MOUNTAIN = 8,
	TERRAIN_UNPASSABLE = 16,
};

#endif
