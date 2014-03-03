/*
 * Copyright (C) 2007-2009 by the Widelands Development Team
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

#ifndef AREAWATCHER_H
#define AREAWATCHER_H

#include "logic/instances.h"
#include "logic/player_area.h"

namespace Widelands {

struct AreaWatcher_Descr;

/**
 * An area watcher is an abstract object that sees an area for a player. It is
 * not abstract as in abstract data type, but in the sense that it has no size
 * and does not interact with other objects. Nor is it visualized with any
 * image. The best analogy would perhaps be a geostationary remote sensing
 * satellite, but it sees as good as a person on the ground. It is used to show
 * an area to a player for a limited time. When act is called, the area watcher
 * is destroyed. Use schedule_act to determine the area watcher's lifetime.
 */
struct AreaWatcher : public Map_Object, private Player_Area<> {
	friend class Player;
	friend struct Map_Players_AreaWatchers_Data_Packet;
	AreaWatcher(const Player_Area<>);
	virtual int32_t get_type() const override {return AREAWATCHER;}
	char const * type_name() const override {return "areawatcher";}
	static AreaWatcher & create(Editor_Game_Base &, const Player_Area<>);
	virtual void act(Game &, uint32_t data) override;
};

}

#endif
