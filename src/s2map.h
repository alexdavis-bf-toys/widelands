/*
 * Copyright (C) 2002, 2008, 2013 by the Widelands Development Team
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

#ifndef S2MAP_H
#define S2MAP_H

#include <string>

#include "map_io/map_loader.h"

struct S2_Map_Loader : public Widelands::Map_Loader {
	enum WorldType {
		GREENLAND = 0,
		BLACKLAND = 1,
		WINTERLAND = 2,
	};

	S2_Map_Loader(const char *, Widelands::Map &);

	virtual int32_t preload_map(bool) override;
	virtual int32_t load_map_complete
		(Widelands::Editor_Game_Base &, bool scenario) override;

private:
	const std::string m_filename;
	WorldType m_worldtype;

	void load_s2mf_header(FileRead&);
	void load_s2mf(Widelands::Editor_Game_Base &);
	void postload_fix_conversion(Widelands::Editor_Game_Base &);
};


#endif
