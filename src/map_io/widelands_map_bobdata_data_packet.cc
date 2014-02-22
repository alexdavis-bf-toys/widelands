/*
 * Copyright (C) 2002-2004, 2006-2010 by the Widelands Development Team
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

#include "map_io/widelands_map_bobdata_data_packet.h"

#include "economy/route.h"
#include "economy/transfer.h"
#include "economy/ware_instance.h"
#include "logic/battle.h"
#include "logic/bob.h"
#include "logic/carrier.h"
#include "logic/critter_bob.h"
#include "logic/critter_bob_program.h"
#include "logic/game.h"
#include "logic/game_data_error.h"
#include "logic/map.h"
#include "logic/militarysite.h"
#include "logic/path.h"
#include "logic/player.h"
#include "logic/soldier.h"
#include "logic/tribe.h"
#include "logic/widelands_fileread.h"
#include "logic/widelands_filewrite.h"
#include "logic/worker_program.h"
#include "map_io/widelands_map_map_object_loader.h"
#include "map_io/widelands_map_map_object_saver.h"
#include "upcast.h"

namespace Widelands {

#define CURRENT_PACKET_VERSION 5

// Bob subtype versions
#define CRITTER_BOB_PACKET_VERSION 1
#define WORKER_BOB_PACKET_VERSION 2

// Worker subtype versions
#define SOLDIER_WORKER_BOB_PACKET_VERSION 7
#define CARRIER_WORKER_BOB_PACKET_VERSION 1


void Map_Bobdata_Data_Packet::Read
	(FileSystem            &       fs,
	 Editor_Game_Base      &       egbase,
	 bool                    const skip,
	 Map_Map_Object_Loader &       mol)

{
	if (skip)
		return;

	FileRead fr;
	try {fr.Open(fs, "binary/bob_data");} catch (...) {return;}

	try {
		uint16_t const packet_version = fr.Unsigned16();
		if (1 <= packet_version and packet_version <= CURRENT_PACKET_VERSION) {
			const Map   &       map        = egbase.map();
			Extent        const extent     = map.extent();
			Player_Number const nr_players = map.get_nrplayers();
			for (;;) {
				if (3 <= packet_version and fr.EndOfFile())
					break;
				Serial const serial = fr.Unsigned32();
				if (packet_version < 3 and serial == 0xffffffff) {
					if (not fr.EndOfFile())
						throw game_data_error
							("expected end of file after serial 0xffffffff");
					break;
				}
				try {
					Bob & bob = mol.get<Bob>(serial);
					const Bob::Descr & bob_descr = bob.descr();

					if (Player_Number const read_owner = fr.Player_Number8()) {
						if (nr_players < read_owner)
							throw game_data_error
								("owner number is %u but there are only %u players",
								 read_owner, nr_players);
						if (Player * const owner = egbase.get_player(read_owner))
							bob.set_owner(owner);
						else
							throw game_data_error
								("owning player %u does not exist", read_owner);
					}

					//  basic initialization
					bob.set_position(egbase, fr.Coords32(extent));

					if (packet_version <= 4) {
						fr.Unsigned8(); // used to indicate whether we had a transfer
					}

					bob.m_actid = fr.Unsigned32();

					if (packet_version < 3) {
						bob.m_anim =
							fr.Unsigned8() ? bob_descr.get_animation(fr.CString()) :
							0;
					} else {
						char const * const anim_name = fr.CString();
						bob.m_anim =
							*anim_name ? bob_descr.get_animation(anim_name) : 0;
					}
					bob.m_animstart = fr.Signed32();

					if (packet_version < 3) {
						WalkingDir const walking_dir =
							static_cast<WalkingDir>(fr.Signed32());
						if (6 < walking_dir)
							throw game_data_error
								("walking dir is %u but must be one of {0 (idle), 1 "
								 "(northeast), 2 (east), 3 (southeast), 4 "
								 "(southwest), 5 (west), 6 (northwest)}",
								 walking_dir);
						bob.m_walking = walking_dir;
					} else
						try {
							bob.m_walking =
								static_cast<WalkingDir>(fr.Direction8_allow_null());
						} catch (const StreamRead::direction_invalid & e) {
							throw game_data_error
								("walking dir is %u but must be one of {0 (idle), 1 "
								 "(northeast), 2 (east), 3 (southeast), 4 "
								 "(southwest), 5 (west), 6 (northwest)}",
								 e.direction);
						}
					bob.m_walkstart = fr.Signed32();
					bob.m_walkend   = fr.Signed32();
					if (bob.m_walkend < bob.m_walkstart)
						throw game_data_error
							("walkend (%i) < walkstart (%i)",
							 bob.m_walkend, bob.m_walkstart);

					uint16_t const old_stacksize = bob.m_stack.size();
					uint16_t const new_stacksize = fr.Unsigned16();
					bob.m_stack.resize(new_stacksize);
					for (uint32_t i = 0; i < new_stacksize; ++i) {
						Bob::State & state = bob.m_stack[i];
						try {
							{ //  Task
								const Bob::Task * task;
								char const * const taskname = fr.CString();
								if      (not strcmp(taskname, "idle"))
									task = &Bob::taskIdle;
								else if (not strcmp(taskname, "movepath"))
									task = &Bob::taskMovepath;
								else if
									(packet_version == 1 &&
									 not strcmp(taskname, "forcemove"))
								{
									task = &Bob::taskMove;
								} else if (not strcmp(taskname, "move"))
									task = &Bob::taskMove;
								else if (not strcmp(taskname, "roam"))
									task = &Critter_Bob::taskRoam;
								else if (not strcmp(taskname, "program")) {
									if      (dynamic_cast<Worker      const *>(&bob))
										task = &Worker::taskProgram;
									else if (dynamic_cast<Critter_Bob const *>(&bob))
										task = &Critter_Bob::taskProgram;
									else
										throw;
								} else if (not strcmp(taskname, "transfer"))
									task = &Worker::taskTransfer;
								else if (not strcmp(taskname, "buildingwork"))
									task = &Worker::taskBuildingwork;
								else if (not strcmp(taskname, "return"))
									task = &Worker::taskReturn;
								else if (not strcmp(taskname, "gowarehouse"))
									task = &Worker::taskGowarehouse;
								else if (not strcmp(taskname, "dropoff"))
									task = &Worker::taskReleaserecruit;
								else if (not strcmp(taskname, "releaserecruit"))
									task = &Worker::taskDropoff;
								else if (not strcmp(taskname, "fetchfromflag"))
									task = &Worker::taskFetchfromflag;
								else if (not strcmp(taskname, "waitforcapacity"))
									task = &Worker::taskWaitforcapacity;
								else if (not strcmp(taskname, "leavebuilding"))
									task = &Worker::taskLeavebuilding;
								else if (not strcmp(taskname, "fugitive"))
									task = &Worker::taskFugitive;
								else if (not strcmp(taskname, "geologist"))
									task = &Worker::taskGeologist;
								else if (not strcmp(taskname, "scout"))
									task = &Worker::taskScout;
								else if (not strcmp(taskname, "road"))
									task = &Carrier::taskRoad;
								else if (not strcmp(taskname, "transport"))
									task = &Carrier::taskTransport;
								else if (not strcmp(taskname, "attack"))
									task = &Soldier::taskAttack;
								else if (not strcmp(taskname, "defense"))
									task = &Soldier::taskDefense;
								else if (not strcmp(taskname, "battle"))
									task = &Soldier::taskBattle;
								else if (not strcmp(taskname, "die"))
									task = &Soldier::taskDie;
								else if (not strcmp(taskname, "moveInBattle"))
									task = &Soldier::taskMoveInBattle;
								else if
									(not strcmp(taskname, "moveToBattle") ||
									 not strcmp(taskname, "moveHome"))
									//  Weird hack to support legacy games.
									task = &Worker::taskBuildingwork;
								else if (*taskname == '\0')
									continue; // Skip task
								else
									throw game_data_error
										(_("unknown task type \"%s\""), taskname);

								if (task->unique and bob.get_state(*task))
									throw game_data_error
										(_("task %s is duplicated in stack"),
										 task->name);
								state.task = task;
							}

							state.ivar1 = fr.Signed32();
							state.ivar2 = fr.Signed32();
							state.ivar3 = fr.Signed32();

							if (Serial const objvar1_serial = fr.Unsigned32()) {
								try {
									state.objvar1 =
										&mol.get<Map_Object>(objvar1_serial);
								} catch (const _wexception & e) {
									throw game_data_error
										("objvar1 (%u): %s", objvar1_serial, e.what());
								}
							} else
								state.objvar1 = 0;
							state.svar1 = fr.CString();
							if (packet_version < 3) {
								int32_t const x = fr.Signed32();
								int32_t const y = fr.Signed32();
								state.coords = Coords(x, y);
								if (state.coords and (extent.w <= x or extent.h <= y))
									throw game_data_error
										("invalid coordinates (%i, %i)", x, y);
							} else
								state.coords = fr.Coords32_allow_null(extent);

							if (fr.Unsigned8()) {
								const uint32_t ans[6] = {
									bob_descr.get_animation(fr.CString()),
									bob_descr.get_animation(fr.CString()),
									bob_descr.get_animation(fr.CString()),
									bob_descr.get_animation(fr.CString()),
									bob_descr.get_animation(fr.CString()),
									bob_descr.get_animation(fr.CString())
								};
								state.diranims = DirAnimations(ans[0], ans[1], ans[2], ans[3], ans[4], ans[5]);

								if
									(state.task == &Bob::taskMove and
									 packet_version < 4)
									throw game_data_error
										("savegame created with old broken game engine "
										 "version that pushed task move without "
										 "starting walk first; not fixed; try another "
										 "savegame from a slightly different point in "
										 "time if available; the erroneous state that "
										 "this bob is in only lasts 10ms");
							} else
								state.diranims = DirAnimations::Null();

							uint32_t const pathsteps = fr.Unsigned16();
							if (i < old_stacksize) {
								delete state.path;
								state.path = 0;
							}
							if (pathsteps) {
								try {
									assert(not state.path);
									state.path = new Path(fr.Coords32(extent));
									for (uint32_t step = pathsteps; step; --step)
										try {
											state.path->append(map, fr.Direction8());
										} catch (const _wexception & e) {
											throw game_data_error
												("step #%u: %s",
												 pathsteps - step, e.what());
										}
								} catch (const _wexception & e) {
									throw game_data_error
										(_("reading path: %s"), e.what());
								}
							}

							{
								bool const has_route = fr.Unsigned8();
								if (i < old_stacksize && state.route) {
									if (!has_route) {
										delete state.route;
										// in case we get an exception further down
										state.route = 0;
									} else
										state.route->init(0);
								}

								if (has_route) {
									Route * const route =
										state.route ? state.route : new Route();
									Route::LoadData d;
									route->load(d, fr);
									route->load_pointers(d, mol);
									state.route = route;
								} else
									state.route = 0;
							}

							if (fr.Unsigned8()) {
								std::string progname = fr.CString();
								std::transform
									(progname.begin(), progname.end(), progname.begin(),
									 tolower);
								if      (upcast(Worker      const, wor, &bob))
									state.program = wor->descr().get_program(progname);
								else if (upcast(Critter_Bob const, cri, &bob))
									state.program = cri->descr().get_program(progname);
								else
									throw;
							} else
								state.program = 0;

						} catch (const _wexception & e) {
							throw game_data_error
								("(%s) reading state %u: %s",
								 bob.descr().descname().c_str(), i, e.what());
						}
					}

					//  rest of bob stuff
					if (packet_version == 1) {
						fr.Unsigned8(); // used to be m_stack_dirty
						fr.Unsigned8(); // used to be m_sched_init_task
					}
					bob.m_signal          = fr.CString  ();

					if      (upcast(Critter_Bob, critter_bob, &bob))
						read_critter_bob(fr, egbase, mol, *critter_bob);
					else if (upcast(Worker,      worker,      &bob))
						read_worker_bob (fr, egbase, mol, *worker);
					else
						assert(false);

					mol.mark_object_as_loaded(bob);
				} catch (const _wexception & e) {
					throw game_data_error(_("bob %u: %s"), serial, e.what());
				}
			}
		} else
			throw game_data_error
				(_("unknown/unhandled version %u"), packet_version);
	} catch (const _wexception & e) {
		throw game_data_error(_("bobdata: %s"), e.what());
	}
}

void Map_Bobdata_Data_Packet::read_critter_bob
	(FileRead & fr, Editor_Game_Base &, Map_Map_Object_Loader &, Critter_Bob &)
{
	try {
		uint16_t const packet_version = fr.Unsigned16();
		if (packet_version == CRITTER_BOB_PACKET_VERSION) {
			// No data for critter bob currently
		} else
			throw game_data_error
				(_("unknown/unhandled version %u"), packet_version);
	} catch (const _wexception & e) {
		throw game_data_error(_("critter bob: %s"), e.what());
	}
}

void Map_Bobdata_Data_Packet::read_worker_bob
	(FileRead              & fr,
	 Editor_Game_Base      & egbase,
	 Map_Map_Object_Loader & mol,
	 Worker                & worker)
{
	try {
		uint16_t const packet_version = fr.Unsigned16();
		if (1 <= packet_version && packet_version <= WORKER_BOB_PACKET_VERSION) {
			if (upcast(Soldier, soldier, &worker)) {
				try {
					uint16_t const soldier_worker_bob_packet_version =
						fr.Unsigned16();
					if
						(5 <= soldier_worker_bob_packet_version
						 and
						 soldier_worker_bob_packet_version
						 <=
						 SOLDIER_WORKER_BOB_PACKET_VERSION)
					{
						const Soldier_Descr & descr = soldier->descr();

						soldier->m_hp_current = fr.Unsigned32() * 100; // balance change: multiply times 100

						if (soldier_worker_bob_packet_version <= 6) {
							// no longer used values
							fr.Unsigned32(); // max hp
							fr.Unsigned32(); // min attack
							fr.Unsigned32(); // max attack
							fr.Unsigned32(); // defense
							fr.Unsigned32(); // evade
						}

#define READLEVEL(variable, pn, maxfunction)                                  \
   soldier->variable = fr.Unsigned32();                                       \
   {                                                                          \
      uint32_t const max = descr.maxfunction();                               \
      if (max < soldier->variable) {                                          \
         log                                                                  \
            ("WARNING: %s %s (%u) of player %u has "                          \
             pn                                                               \
             "_level = %u but it can be at most %u, decreasing it to that "   \
             "value\n",                                                       \
             descr.tribe().name().c_str(), descr.descname().c_str(),          \
             soldier->serial(), soldier->owner().player_number(),             \
             soldier->variable, max);                                         \
         soldier->variable = max;                                             \
       }                                                                      \
   }                                                                          \

						READLEVEL(m_hp_level,      "hp",      get_max_hp_level);
						READLEVEL(m_attack_level,  "attack",  get_max_attack_level);
						READLEVEL(m_defense_level, "defense", get_max_defense_level);
						READLEVEL(m_evade_level,   "evade",   get_max_evade_level);

						if (soldier->m_hp_current > soldier->get_max_hitpoints())
							soldier->m_hp_current = soldier->get_max_hitpoints();

						if (Serial const battle = fr.Unsigned32())
							soldier->m_battle = &mol.get<Battle>(battle);

						if (soldier_worker_bob_packet_version >= 6)
						{
							try {
								soldier->m_combat_walking =
									static_cast<CombatWalkingDir>
										(fr.Direction8_allow_null());
							} catch (const StreamRead::direction_invalid & e) {
								throw game_data_error
									("combat walking dir is %u but must be one of {0 "
									 "(none), 1 (combat walk to west), 2 (combat walk"
									 " to east), 3 (fighting at west), 4 (fighting at"
									 " east), 5 (return from west), 6 (return from "
									 "east)}",
									 e.direction);
							}
							soldier->m_combat_walkstart = fr.Signed32();
							soldier->m_combat_walkend   = fr.Signed32();
							if
								(soldier->m_combat_walkend <
								 soldier->m_combat_walkstart)
								throw game_data_error
									("combat_walkend (%i) < combat_walkstart (%i)",
									 soldier->m_combat_walkend,
									 soldier->m_combat_walkstart);
						}
					} else
						throw game_data_error
							(_("unknown/unhandled version %u"),
							 soldier_worker_bob_packet_version);
				} catch (const _wexception & e) {
					throw game_data_error(_("soldier: %s"), e.what());
				}
			} else if (upcast(Carrier, carrier, &worker)) {
				try {
					uint16_t const carrier_worker_bob_packet_version =
						fr.Unsigned16();
					if
						(carrier_worker_bob_packet_version
						 ==
						 CARRIER_WORKER_BOB_PACKET_VERSION)
						carrier->m_promised_pickup_to = fr.Signed32();
					else
						throw game_data_error
							(_("unknown/unhandled version %u"),
							 carrier_worker_bob_packet_version);
				} catch (const _wexception & e) {
					throw game_data_error(_("carrier: %s"), e.what());
				}
			}

			if (uint32_t const location_serial = fr.Unsigned32()) {
				try {
					worker.set_location(&mol.get<PlayerImmovable>(location_serial));
				} catch (const _wexception & e) {
					throw game_data_error
						("location (%u): %s", location_serial, e.what());
				}
			} else
				worker.m_location = 0;

			if (uint32_t const carried_ware_serial = fr.Unsigned32()) {
				try {
					worker.m_carried_ware =
						&mol.get<WareInstance>(carried_ware_serial);
				} catch (const _wexception & e) {
					throw game_data_error
						("carried ware (%u): %s", carried_ware_serial, e.what());
				}
			} else
				worker.m_carried_ware = 0;

			// Skip supply

			if (packet_version == 1)
				fr.Signed32(); // used to be needed_exp
			worker.m_current_exp = fr.Signed32();

			if (worker.m_current_exp >= worker.get_needed_experience()) {
				// avoid inconsistencies in case the game data has changed
				if (worker.get_needed_experience() == -1)
					worker.m_current_exp = -1;
				else
					worker.m_current_exp = worker.get_needed_experience() - 1;
			}

			Economy * economy = 0;
			if (PlayerImmovable * const location = worker.m_location.get(egbase))
				economy = location->get_economy();
			worker.set_economy(economy);
			if
				(WareInstance * const carried_ware =
				 	worker.m_carried_ware.get(egbase))
				carried_ware->set_economy(economy);
		} else
			throw game_data_error
				(_("unknown/unhandled version %u"), packet_version);
	} catch (const _wexception & e) {
		throw game_data_error
			("worker %p (%u): %s", &worker, worker.serial(), e.what());
	}
}


void Map_Bobdata_Data_Packet::Write
	(FileSystem & /* fs */, Editor_Game_Base & /* egbase */, Map_Map_Object_Saver & /* mos */)
{
	throw wexception("bobdata packet is deprecated");
}

}
