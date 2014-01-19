/*
 * Copyright (C) 2006-2010 by the Widelands Development Team
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

#ifndef LUA_MAP_H
#define LUA_MAP_H

#include <set>

#include "economy/flag.h"
#include "economy/portdock.h"
#include "economy/road.h"
#include "logic/constructionsite.h"
#include "logic/game.h"
#include "logic/militarysite.h"
#include "logic/productionsite.h"
#include "logic/ship.h"
#include "logic/soldier.h"
#include "logic/trainingsite.h"
#include "logic/warehouse.h"
#include "logic/worker.h"
#include "scripting/eris/lua.hpp"
#include "scripting/luna.h"


namespace Widelands {
	struct Soldier_Descr;
	struct Bob;
}

namespace LuaMap {

/*
 * Base class for all classes in wl.map
 */
class L_MapModuleClass : public LunaClass {
	public:
		const char * get_modulename() {return "map";}
};


class L_Map : public L_MapModuleClass {
public:
	LUNA_CLASS_HEAD(L_Map);

	virtual ~L_Map() {}

	L_Map() {}
	L_Map(lua_State * L) {
		report_error(L, "Cannot instantiate a 'Map' directly!");
	}

	virtual void __persist(lua_State * L);
	virtual void __unpersist(lua_State * L);

	/*
	 * Properties
	 */
	int get_width(lua_State *);
	int get_height(lua_State *);
	int get_player_slots(lua_State *);

	/*
	 * Lua methods
	 */
	int place_immovable(lua_State *);
	int get_field(lua_State *);
	int recalculate(lua_State *);

	/*
	 * C methods
	 */
private:
};


#define CASTED_GET(klass) \
Widelands:: klass * get(lua_State * L, Widelands::Editor_Game_Base & egbase) { \
	return static_cast<Widelands:: klass *> \
		(L_MapObject::get(L, egbase, #klass)); \
}

class L_MapObject : public L_MapModuleClass {
	Widelands::Object_Ptr * m_ptr;

public:
	LUNA_CLASS_HEAD(L_MapObject);

	L_MapObject() : m_ptr(0) {}
	L_MapObject(Widelands::Map_Object & mo) {
		m_ptr = new Widelands::Object_Ptr(&mo);
	}
	L_MapObject(lua_State * L) : m_ptr(0) {
		report_error(L, "Cannot instantiate a '%s' directly!", className);
	}
	virtual ~L_MapObject() {
		if (m_ptr) {
			delete m_ptr;
			m_ptr = 0;
		}
	}

	virtual void __persist(lua_State * L);
	virtual void __unpersist(lua_State * L);

	/*
	 * attributes
	 */
	int get___hash(lua_State *);
	int get_serial(lua_State *);
	int get_type(lua_State *);
	int get_name(lua_State *);
	int get_descname(lua_State *);

	/*
	 * Lua Methods
	 */
	int __eq(lua_State * L);
	int remove(lua_State * L);
	int has_attribute(lua_State * L);

	/*
	 * C Methods
	 */
	Widelands::Map_Object * get
		(lua_State *, Widelands::Editor_Game_Base &, std::string = "MapObject");
	Widelands::Map_Object * m_get_or_zero(Widelands::Editor_Game_Base &);
};


class L_BaseImmovable : public L_MapObject {
public:
	LUNA_CLASS_HEAD(L_BaseImmovable);

	L_BaseImmovable() {}
	L_BaseImmovable(Widelands::BaseImmovable & mo) : L_MapObject(mo) {}
	L_BaseImmovable(lua_State * L) : L_MapObject(L) {}
	virtual ~L_BaseImmovable() {}

	/*
	 * Properties
	 */
	int get_size(lua_State * L);
	int get_fields(lua_State * L);

	/*
	 * Lua Methods
	 */

	/*
	 * C Methods
	 */
	CASTED_GET(BaseImmovable);
};

class L_PlayerImmovable : public L_BaseImmovable {
public:
	LUNA_CLASS_HEAD(L_PlayerImmovable);

	L_PlayerImmovable() {}
	L_PlayerImmovable(Widelands::PlayerImmovable & mo) : L_BaseImmovable(mo) {
	}
	L_PlayerImmovable(lua_State * L) : L_BaseImmovable(L) {}
	virtual ~L_PlayerImmovable() {}

	/*
	 * Properties
	 */
	int get_owner(lua_State * L);
	int get_debug_economy(lua_State* L);

	/*
	 * Lua Methods
	 */

	/*
	 * C Methods
	 */
	CASTED_GET(PlayerImmovable);
};

class L_PortDock : public L_PlayerImmovable {
public:
	LUNA_CLASS_HEAD(L_PortDock);

	L_PortDock() {}
	L_PortDock(Widelands::PortDock & mo) : L_PlayerImmovable(mo) {
	}
	L_PortDock(lua_State * L) : L_PlayerImmovable(L) {}
	virtual ~L_PortDock() {}

	/*
	 * Properties
	 */

	/*
	 * Lua methods
	 */

	/*
	 * C methods
	 */
	CASTED_GET(PortDock);
};

class L_Building : public L_PlayerImmovable {
public:
	LUNA_CLASS_HEAD(L_Building);

	L_Building() {}
	L_Building(Widelands::Building & mo) : L_PlayerImmovable(mo) {
	}
	L_Building(lua_State * L) : L_PlayerImmovable(L) {}
	virtual ~L_Building() {}

	/*
	 * Properties
	 */
	int get_building_type(lua_State* L);
	int get_flag(lua_State* L);

	/*
	 * Lua Methods
	 */

	/*
	 * C Methods
	 */
	CASTED_GET(Building);
};

class L_HasWares {
public:
	virtual ~L_HasWares() {}

	virtual int set_wares(lua_State * L) = 0;
	virtual int get_wares(lua_State * L) = 0;

	typedef std::set<Widelands::Ware_Index> WaresSet;
	typedef std::map<Widelands::Ware_Index, uint32_t> WaresMap;
	typedef std::pair<Widelands::Ware_Index, uint32_t> WareAmount;

protected:
	WaresSet m_parse_get_wares_arguments
		(lua_State *, const Widelands::Tribe_Descr &, bool *);
	WaresMap m_parse_set_wares_arguments
		(lua_State *, const Widelands::Tribe_Descr &);
};
class L_HasWorkers {
public:
	virtual ~L_HasWorkers() {}

	virtual int set_workers(lua_State * L) = 0;
	virtual int get_workers(lua_State * L) = 0;

	typedef std::set<Widelands::Ware_Index> WorkersSet;
	typedef std::map<Widelands::Ware_Index, uint32_t> WorkersMap;
	typedef std::pair<Widelands::Ware_Index, uint32_t> WorkerAmount;

protected:
	WorkersSet m_parse_get_workers_arguments
		(lua_State *, const Widelands::Tribe_Descr &, bool *);
	WorkersMap m_parse_set_workers_arguments
		(lua_State *, const Widelands::Tribe_Descr &);
};

class L_HasSoldiers {
public:
	struct SoldierDescr {
		SoldierDescr(uint8_t ghp, uint8_t gat, uint8_t gde, uint8_t gev) :
			hp(ghp), at(gat), de(gde), ev(gev) {}
		SoldierDescr() : hp(0), at(0), de(0), ev(0) {}

		uint8_t hp;
		uint8_t at;
		uint8_t de;
		uint8_t ev;

		bool operator< (const SoldierDescr & ot) const {
			bool hp_eq = hp == ot.hp;
			bool at_eq = at == ot.at;
			bool de_eq = de == ot.de;
			if (hp_eq && at_eq && de_eq)
				return ev < ot.ev;
			if (hp_eq && at_eq)
				return de < ot.de;
			if (hp_eq)
				return at < ot.at;
			return hp < ot.hp;
		}
		bool operator== (const SoldierDescr & ot) const {
			if (hp == ot.hp and at == ot.at and de == ot.de and ev == ot.ev)
				return true;
			return false;
		}
	};

	virtual ~L_HasSoldiers() {}

	virtual int set_soldiers(lua_State * L) = 0;
	virtual int get_soldiers(lua_State * L) = 0;

	typedef std::vector<Widelands::Soldier *> SoldiersList;
	typedef std::map<SoldierDescr, uint32_t> SoldiersMap;
	typedef std::pair<SoldierDescr, uint32_t> SoldierAmount;

protected:
	int m_handle_get_soldiers
		(lua_State *, const Widelands::Soldier_Descr &, const SoldiersList &);
	SoldiersMap m_parse_set_soldiers_arguments
		(lua_State *, const Widelands::Soldier_Descr &);
	int m_get_soldier_levels
		(lua_State *, int, const Widelands::Soldier_Descr &, SoldierDescr &);
};


class L_Flag : public L_PlayerImmovable, public L_HasWares {
public:
	LUNA_CLASS_HEAD(L_Flag);

	L_Flag() {}
	L_Flag(Widelands::Flag & mo) : L_PlayerImmovable(mo) {
	}
	L_Flag(lua_State * L) : L_PlayerImmovable(L) {}
	virtual ~L_Flag() {}

	/*
	 * Properties
	 */

	/*
	 * Lua Methods
	 */
	int set_wares(lua_State *);
	int get_wares(lua_State *);

	/*
	 * C Methods
	 */
	CASTED_GET(Flag);
};

// Small helper class that contains the commonalities between L_Road and
// L_ProductionSite in relation to Worker employment.
class _WorkerEmployer : public L_HasWorkers {
public:
	virtual int get_workers(lua_State * L);
	virtual int set_workers(lua_State * L);

	int get_valid_workers(lua_State * L);

	virtual Widelands::PlayerImmovable * get
		(lua_State *, Widelands::Editor_Game_Base &) = 0;

protected:
	virtual WorkersMap _valid_workers(Widelands::PlayerImmovable &) = 0;
	virtual int _new_worker
		(Widelands::PlayerImmovable &, Widelands::Editor_Game_Base &, const Widelands::Worker_Descr *) = 0;
};

class _SoldierEmployer : public L_HasSoldiers {
public:
	virtual int get_soldiers(lua_State * L);
	virtual int set_soldiers(lua_State * L);

	int get_max_soldiers(lua_State * L);

	virtual Widelands::Building * get
		(lua_State *, Widelands::Editor_Game_Base &) = 0;
	virtual Widelands::SoldierControl * get_sc
		(lua_State *, Widelands::Editor_Game_Base &) = 0;
};

class L_Road : public L_PlayerImmovable, public _WorkerEmployer {
public:
	LUNA_CLASS_HEAD(L_Road);

	L_Road() {}
	L_Road(Widelands::Road & mo) : L_PlayerImmovable(mo) {
	}
	L_Road(lua_State * L) : L_PlayerImmovable(L) {}
	virtual ~L_Road() {}

	/*
	 * Properties
	 */
	int get_length(lua_State * L);
	int get_start_flag(lua_State * L);
	int get_end_flag(lua_State * L);
	int get_road_type(lua_State * L);

	/*
	 * Lua Methods
	 */

	/*
	 * C Methods
	 */
	CASTED_GET(Road);
protected:
	virtual WorkersMap _valid_workers(Widelands::PlayerImmovable &);
	virtual int _new_worker
		(Widelands::PlayerImmovable &,
		 Widelands::Editor_Game_Base &, const Widelands::Worker_Descr *);
};


class L_ConstructionSite : public L_Building
{
public:
	LUNA_CLASS_HEAD(L_ConstructionSite);

	L_ConstructionSite() {}
	L_ConstructionSite(Widelands::ConstructionSite & mo) : L_Building(mo) {
	}
	L_ConstructionSite(lua_State * L) : L_Building(L) {}
	virtual ~L_ConstructionSite() {}

	/*
	 * Properties
	 */
	int get_building(lua_State *);

	/*
	 * Lua Methods
	 */

	/*
	 * C Methods
	 */
	CASTED_GET(ConstructionSite);
};


class L_Warehouse : public L_Building,
	public L_HasWares, public L_HasWorkers, public _SoldierEmployer
{
public:
	LUNA_CLASS_HEAD(L_Warehouse);

	L_Warehouse() {}
	L_Warehouse(Widelands::Warehouse & mo) : L_Building(mo) {
	}
	L_Warehouse(lua_State * L) : L_Building(L) {}
	virtual ~L_Warehouse() {}

	/*
	 * Properties
	 */
	int get_portdock(lua_State* L);

	/*
	 * Lua Methods
	 */
	int set_wares(lua_State *);
	int get_wares(lua_State *);
	int set_workers(lua_State *);
	int get_workers(lua_State *);

	/*
	 * C Methods
	 */
	CASTED_GET(Warehouse);
	Widelands::SoldierControl * get_sc
		(lua_State * L, Widelands::Editor_Game_Base & g)
	{
		return get(L, g);
	}
};


class L_ProductionSite : public L_Building,
	public _WorkerEmployer, public L_HasWares {
public:
	LUNA_CLASS_HEAD(L_ProductionSite);

	L_ProductionSite() {}
	L_ProductionSite(Widelands::ProductionSite & mo) : L_Building(mo) {
	}
	L_ProductionSite(lua_State * L) : L_Building(L) {}
	virtual ~L_ProductionSite() {}

	/*
	 * Properties
	 */
	int get_valid_wares(lua_State * L);

	/*
	 * Lua Methods
	 */
	int set_wares(lua_State * L);
	int get_wares(lua_State * L);

	/*
	 * C Methods
	 */
	CASTED_GET(ProductionSite);

protected:
	virtual WorkersMap _valid_workers(Widelands::PlayerImmovable &);
	virtual int _new_worker
		(Widelands::PlayerImmovable &, Widelands::Editor_Game_Base &,
		 const Widelands::Worker_Descr *);
};

class L_MilitarySite : public L_Building, public _SoldierEmployer {
public:
	LUNA_CLASS_HEAD(L_MilitarySite);

	L_MilitarySite() {}
	L_MilitarySite(Widelands::MilitarySite & mo) : L_Building(mo) {
	}
	L_MilitarySite(lua_State * L) : L_Building(L) {}
	virtual ~L_MilitarySite() {}

	/*
	 * Properties
	 */

	/*
	 * Lua Methods
	 */

	/*
	 * C Methods
	 */
	CASTED_GET(MilitarySite);
	Widelands::SoldierControl * get_sc
		(lua_State * L, Widelands::Editor_Game_Base & g)
	{
		return get(L, g);
	}
};


class L_TrainingSite : public L_ProductionSite, public _SoldierEmployer {
public:
	LUNA_CLASS_HEAD(L_TrainingSite);

	L_TrainingSite() {}
	L_TrainingSite(Widelands::TrainingSite & mo) : L_ProductionSite(mo) {
	}
	L_TrainingSite(lua_State * L) : L_ProductionSite(L) {}
	virtual ~L_TrainingSite() {}

	/*
	 * Properties
	 */

	/*
	 * Lua Methods
	 */

	/*
	 * C Methods
	 */
	CASTED_GET(TrainingSite);
	Widelands::SoldierControl * get_sc
		(lua_State * L, Widelands::Editor_Game_Base & g) {return get(L, g);}
};

class L_Bob : public L_MapObject {
public:
	LUNA_CLASS_HEAD(L_Bob);

	L_Bob() {}
	L_Bob(Widelands::Bob & mo) : L_MapObject(mo) {}
	L_Bob(lua_State * L) : L_MapObject(L) {}
	virtual ~L_Bob() {}

	/*
	 * Properties
	 */
	int has_caps(lua_State *);

	/*
	 * Lua Methods
	 */

	/*
	 * C Methods
	 */
	CASTED_GET(Bob);
};

class L_Worker : public L_Bob {
public:
	LUNA_CLASS_HEAD(L_Worker);

	L_Worker() {}
	L_Worker(Widelands::Worker & w) : L_Bob(w) {}
	L_Worker(lua_State * L) : L_Bob(L) {}
	virtual ~L_Worker() {}

	/*
	 * Properties
	 */
	int get_owner(lua_State * L);
	int get_location(lua_State *);

	/*
	 * Lua methods
	 */

	/*
	 * C methods
	 */
	CASTED_GET(Worker);
};

class L_Soldier : public L_Worker {
public:
	LUNA_CLASS_HEAD(L_Soldier);

	L_Soldier() {}
	L_Soldier(Widelands::Soldier & w) : L_Worker(w) {}
	L_Soldier(lua_State * L) : L_Worker(L) {}
	virtual ~L_Soldier() {}

	/*
	 * Properties
	 */
	int get_attack_level(lua_State *);
	int get_defense_level(lua_State *);
	int get_hp_level(lua_State *);
	int get_evade_level(lua_State *);

	/*
	 * Lua methods
	 */

	/*
	 * C methods
	 */
	CASTED_GET(Soldier);
};

class L_Ship : public L_Bob {
public:
	LUNA_CLASS_HEAD(L_Ship);

	L_Ship() {}
	L_Ship(Widelands::Ship& s) : L_Bob(s) {}
	L_Ship(lua_State* L) : L_Bob(L) {}
	virtual ~L_Ship() {}

	/*
	 * Properties
	 */
	int get_debug_economy(lua_State * L);
	int get_last_portdock(lua_State* L);
	int get_destination(lua_State* L);

	/*
	 * Lua methods
	 */
	int get_wares(lua_State* L);
	int get_workers(lua_State* L);

	/*
	 * C methods
	 */
	CASTED_GET(Ship);
};
#undef CASTED_GET

class L_Field : public L_MapModuleClass {
	Widelands::Coords m_c;
public:
	LUNA_CLASS_HEAD(L_Field);

	L_Field() {}
	L_Field (Widelands::Coordinate x, Widelands::Coordinate y) :
		m_c(Widelands::Coords(x, y)) {}
	L_Field (Widelands::Coords c) : m_c(c) {}
	L_Field(lua_State * L) {
		report_error(L, "Cannot instantiate a 'Field' directly!");
	}
	virtual ~L_Field() {}

	virtual void __persist(lua_State * L);
	virtual void __unpersist(lua_State * L);

	/*
	 * Properties
	 */
	int get___hash(lua_State *);
	int get_x(lua_State * L);
	int get_y(lua_State * L);
	int get_viewpoint_x(lua_State * L);
	int get_viewpoint_y(lua_State * L);
	int get_height(lua_State * L);
	int set_height(lua_State * L);
	int get_raw_height(lua_State * L);
	int set_raw_height(lua_State * L);
	int get_immovable(lua_State * L);
	int get_bobs(lua_State * L);
	int get_terr(lua_State * L);
	int set_terr(lua_State * L);
	int get_terd(lua_State * L);
	int set_terd(lua_State * L);
	int get_rn(lua_State *);
	int get_ln(lua_State *);
	int get_trn(lua_State *);
	int get_tln(lua_State *);
	int get_bln(lua_State *);
	int get_brn(lua_State *);
	int get_resource(lua_State *);
	int set_resource(lua_State *);
	int get_resource_amount(lua_State *);
	int set_resource_amount(lua_State *);
	int get_claimers(lua_State *);
	int get_owner(lua_State *);

	/*
	 * Lua methods
	 */
	int __tostring(lua_State * L);
	int __eq(lua_State * L);
	int region(lua_State * L);
	int has_caps(lua_State *);

	/*
	 * C methods
	 */
	inline const Widelands::Coords & coords() {return m_c;}
	const Widelands::FCoords fcoords(lua_State * L);

private:
	int m_region(lua_State * L, uint32_t radius);
	int m_hollow_region(lua_State * L, uint32_t radius, uint32_t inner_radius);
};

class L_PlayerSlot : public L_MapModuleClass {
	Widelands::Player_Number m_plr;

public:
	LUNA_CLASS_HEAD(L_PlayerSlot);

	L_PlayerSlot() : m_plr(0) {}
	L_PlayerSlot(Widelands::Player_Number plr) : m_plr(plr) {}
	L_PlayerSlot(lua_State * L) : m_plr(0) {
		report_error(L, "Cannot instantiate a 'PlayerSlot' directly!");
	}
	virtual ~L_PlayerSlot() {}

	virtual void __persist(lua_State * L);
	virtual void __unpersist(lua_State * L);

	/*
	 * Properties
	 */
	int get_tribe_name(lua_State *);
	int get_name(lua_State *);
	int get_starting_field(lua_State *);

	/*
	 * Lua methods
	 */

	/*
	 * C methods
	 */
};

int upcasted_immovable_to_lua(lua_State * L, Widelands::BaseImmovable * bi);
int upcasted_bob_to_lua(lua_State * L, Widelands::Bob * mo);

void luaopen_wlmap(lua_State *);


};

#endif
