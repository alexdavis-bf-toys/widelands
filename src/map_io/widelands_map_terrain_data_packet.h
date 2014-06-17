/*
 * Copyright (C) 2002-2004, 2006-2008, 2010 by the Widelands Development Team
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

#ifndef WIDELANDS_MAP_TERRAIN_DATA_PACKET_H
#define WIDELANDS_MAP_TERRAIN_DATA_PACKET_H

#include <string>

class OneWorldLegacyLookupTable;
class FileSystem;

namespace Widelands {

class Editor_Game_Base;

class Map_Terrain_Data_Packet {
public:
	void Read(FileSystem&, Editor_Game_Base&, const OneWorldLegacyLookupTable& lookup_table);
	void Write(FileSystem&, Editor_Game_Base&);
};

}

#endif
