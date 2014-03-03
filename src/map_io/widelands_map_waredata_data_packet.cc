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

#include "map_io/widelands_map_waredata_data_packet.h"

#include "economy/flag.h"
#include "economy/ware_instance.h"
#include "logic/bob.h"
#include "logic/editor_game_base.h"
#include "logic/game.h"
#include "logic/legacy.h"
#include "logic/map.h"
#include "logic/player.h"
#include "logic/tribe.h"
#include "logic/widelands_fileread.h"
#include "logic/worker.h"
#include "map_io/widelands_map_map_object_loader.h"
#include "map_io/widelands_map_map_object_saver.h"
#include "upcast.h"

namespace Widelands {

#define CURRENT_PACKET_VERSION 2


void Map_Waredata_Data_Packet::Read
	(FileSystem            &       fs,
	 Editor_Game_Base      &       egbase,
	 bool                    const skip,
	 Map_Map_Object_Loader &       mol)
{
	if (skip)
		return;

	FileRead fr;
	try {fr.Open(fs, "binary/ware_data");} catch (...) {return;}

	try {
		uint16_t const packet_version = fr.Unsigned16();
		if (1 == packet_version) {
			for (;;) {
				Serial const serial = fr.Unsigned32();
				if (serial == 0xffffffff) {
					if (not fr.EndOfFile())
						throw game_data_error
							("expected end of file after serial 0xffffffff");
					break;
				}
				try {
					WareInstance & ware = mol.get<WareInstance>(serial);
					ware.m_economy = nullptr;
					uint32_t     const location_serial = fr.Unsigned32();
					uint32_t     const index           = fr.Unsigned32();
					try {
						Map_Object & location = mol.get<Map_Object>(location_serial);
						const Tribe_Descr * tribe = nullptr;
						Economy * economy = nullptr;
						std::string explanation;

						if (upcast(PlayerImmovable, player_immovable, &location)) {
							if
								(dynamic_cast<Building const *>(player_immovable)
								 or
								 dynamic_cast<Flag     const *>(player_immovable))
							{
								tribe = &player_immovable->owner().tribe();
								economy = player_immovable->get_economy();
								explanation = player_immovable->descr().descname();
							} else
								throw game_data_error
									("is PlayerImmovable but not Building or Flag");
						} else if (upcast(Worker, worker, &location)) {
							tribe = worker->get_tribe();
							explanation = worker->descname();

							//  The worker sets our economy.
						} else
							throw game_data_error("is not PlayerImmovable or Worker");
						//  Do not touch supply or transfer.

						ware.m_descr_index = Legacy::ware_index
							(*tribe, explanation, "has", index);
						if (!ware.m_descr_index) {
							ware.m_descr_index = Ware_Index::First();
							if (upcast(Game, game, &egbase))
								ware.schedule_destroy(*game);
						}
						ware.m_descr = tribe->get_ware_descr(ware.m_descr_index);

						if (economy)
							ware.set_economy(economy);

						if (uint32_t const nextstep_serial = fr.Unsigned32())
							try {
								ware.m_transfer_nextstep =
									&mol.get<PlayerImmovable>(nextstep_serial);
							} catch (const _wexception & e) {
								throw game_data_error
									("nextstep %1$u: %2$s", nextstep_serial, e.what());
							}
						else
							ware.m_transfer_nextstep =
								static_cast<PlayerImmovable *>(nullptr);

						//  Do some kind of init.
						ware.set_location
							(ref_cast<Game, Editor_Game_Base>(egbase), &location);
					} catch (const _wexception & e) {
						throw game_data_error
							("location %1$u: %2$s", location_serial, e.what());
					}
					mol.mark_object_as_loaded(ware);
				} catch (const _wexception & e) {
					throw game_data_error(_("item %1$u: %2$s"), serial, e.what());
				}
			}
		} else if
			(2 <= packet_version and packet_version <= CURRENT_PACKET_VERSION)
		{
			for (; not fr.EndOfFile();) {
				Serial const serial = fr.Unsigned32();
				try {
					WareInstance & ware = mol.get<WareInstance>(serial);
					ware.m_economy = nullptr;
					uint32_t     const location_serial = fr.Unsigned32();
					char const * const type_name       = fr.CString   ();
					try {
						Map_Object & location = mol.get<Map_Object>(location_serial);

						if (upcast(PlayerImmovable, player_immovable, &location)) {
							if
								(dynamic_cast<Building const *>(player_immovable)
								 or
								 dynamic_cast<Flag     const *>(player_immovable))
							{
								const Tribe_Descr & tribe =
									player_immovable->owner().tribe();
								ware.m_descr_index = tribe.ware_index(type_name);
								if (!ware.m_descr_index) {
									log
										("WARNING: ware type %s does not exist\n",
										 type_name);
									ware.m_descr_index = Ware_Index::First();
									if (upcast(Game, game, &egbase))
										ware.schedule_destroy(*game);
								}
								ware.m_descr = tribe.get_ware_descr(ware.m_descr_index);
								ware.set_economy(player_immovable->get_economy());
							} else
								throw game_data_error
									("is PlayerImmovable but not Building or Flag");
						} else if (upcast(Worker, worker, &location)) {
							const Tribe_Descr & tribe = *worker->get_tribe();
							ware.m_descr =
								tribe.get_ware_descr
									(ware.m_descr_index =
									 	tribe.safe_ware_index(type_name));
							//  The worker sets our economy.
						} else
							throw game_data_error("is not PlayerImmovable or Worker");
						//  Do not touch supply or transfer.

						if (uint32_t const nextstep_serial = fr.Unsigned32())
							try {
								ware.m_transfer_nextstep =
									&mol.get<PlayerImmovable>(nextstep_serial);
							} catch (const _wexception & e) {
								throw game_data_error
									("nextstep %1$u: %2$s", nextstep_serial, e.what());
							}
						else
							ware.m_transfer_nextstep =
								static_cast<PlayerImmovable *>(nullptr);

						//  Do some kind of init.
						ware.set_location
							(ref_cast<Game, Editor_Game_Base>(egbase), &location);
					} catch (const _wexception & e) {
						throw game_data_error
							("location %1$u: %2$s", location_serial, e.what());
					}
					mol.mark_object_as_loaded(ware);
				} catch (const _wexception & e) {
					throw game_data_error(_("item %1$u: %2$s"), serial, e.what());
				}
			}
		} else
			throw game_data_error
				(_("unknown/unhandled version %u"), packet_version);
	} catch (const _wexception & e) {
		throw game_data_error(_("ware data: %s"), e.what());
	}
}


void Map_Waredata_Data_Packet::Write
	(FileSystem & /* fs */, Editor_Game_Base & /* egbase */, Map_Map_Object_Saver & /* mos */)
{
	throw wexception("Map_Waredata_Data_Packet::Write is obsolete");
}

}
