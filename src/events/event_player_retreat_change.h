/*
 * Copyright (C) 2002-2004, 2006, 2008-2009 by the Widelands Development Team
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef EVENT_PLAYER_RETREAT_CHANGE_H
#define EVENT_PLAYER_RETREAT_CHANGE_H

#include "event.h"

#include <set>

namespace Widelands {

struct Tribe_Descr;

/// Abstract base for events involving a player and a boolean option
struct Event_Player_Retreat_Change : public Event {
	Event_Player_Retreat_Change
		(char const * const Name, State const S)
		: Event(Name, S), m_player_number(1), m_percentage(0)
	{}
	Event_Player_Retreat_Change
		(Section &, Editor_Game_Base &, Tribe_Descr const * = 0);

	virtual char const * action_name() const = 0; /// What the event type does.

	int32_t option_menu(Editor_Interactive &) __attribute__ ((noreturn));

	void Write(Section &, Editor_Game_Base &) const;

	Player_Number player_number() const {return m_player_number;}
	void set_player(Player_Number);
	uint32_t const & percentage() const {return m_percentage;}
	uint32_t       & percentage()       {return m_percentage;}
protected:
	Player_Number  m_player_number;
	uint32_t       m_percentage;
};

}

#endif