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

#include "map_io/widelands_map_extradata_data_packet.h"

#include <SDL_image.h>

#include "graphic/graphic.h"
#include "graphic/in_memory_image.h"
#include "graphic/surface.h"
#include "io/filewrite.h"
#include "logic/editor_game_base.h"
#include "logic/game_data_error.h"
#include "logic/map.h"
#include "logic/widelands_fileread.h"
#include "logic/widelands_filewrite.h"
#include "profile/profile.h"

namespace Widelands {

#define CURRENT_PACKET_VERSION 1


void Map_Extradata_Data_Packet::Read
	(FileSystem            &       fs,
	 Editor_Game_Base      &       egbase,
	 bool                    const skip,
	 Map_Map_Object_Loader &)
{
	if (skip)
		return;

	Profile prof;
	try {prof.read("extra_data", 0, fs);} catch (...) {return;}

	try {
		int32_t const packet_version =
			prof.get_safe_section("global").get_safe_int("packet_version");
		if (packet_version == CURRENT_PACKET_VERSION) {
			Map & map = egbase.map();
			//  Nothing more. But read all pics.
			if (fs.FileExists("pics") and fs.IsDirectory("pics")) {
				filenameset_t pictures;
				fs.FindFiles("pics", "*", &pictures);
				for
					(filenameset_t::iterator pname = pictures.begin();
					 pname != pictures.end();
					 ++pname)
				{
					if (fs.IsDirectory(pname->c_str())) // Might be a dir, maybe CVS
						continue;

					const std::string hash = std::string("map:") + FileSystem::FS_Filename(pname->c_str());
					const Image* image = nullptr;
					if (!g_gr->images().has(hash)) {
						FileRead fr;

						fr.Open(fs, pname->c_str());
						SDL_Surface * const surf =
							IMG_Load_RW(SDL_RWFromMem(fr.Data(0), fr.GetSize()), 1);
						if (!surf)
							continue; //  Illegal pic. Skip it.
						image = g_gr->images().insert(new_in_memory_image(hash, Surface::create(surf)));
					} else {
						image = g_gr->images().get(hash);
					}
					assert(image);

					//  OK, the pic is now known to the game. But when the game is
					//  saved, this data has to be regenerated.
					Map::Extradata_Info info;
					info.type     = Map::Extradata_Info::PIC;
					info.filename = *pname;
					info.data     = image;
					// replace \ with / in path or pics won't be saved on Windows
					std::replace(info.filename.begin(), info.filename.end(), '\\', '/');
					map.m_extradatainfos.push_back(info);
				}
			}
		} else
			throw game_data_error
				(_("unknown/unhandled version %u"), packet_version);
	} catch (const _wexception & e) {
		throw game_data_error(_("extra data: %s"), e.what());
	}
}


void Map_Extradata_Data_Packet::Write
	(FileSystem & fs, Editor_Game_Base & egbase, Map_Map_Object_Saver &)
{
	Profile prof;
	prof.create_section("global").set_int
		("packet_version", CURRENT_PACKET_VERSION);

	//  Nothing more. All pics in the dir pic are loaded as pictures.
	const Map::Extradata_Infos & extradatainfos =
		egbase.map().m_extradatainfos;
	for (uint32_t i = 0; i < extradatainfos.size(); ++i) {
		const Map::Extradata_Info & edi = extradatainfos[i];
		assert(edi.type == Map::Extradata_Info::PIC);

		fs.EnsureDirectoryExists("pics");
		FileWrite fw;

		g_gr->save_png(edi.data, &fw);

		fw.Write(fs, edi.filename.c_str());
	}

	prof.write("extra_data", false, fs);
}

}
