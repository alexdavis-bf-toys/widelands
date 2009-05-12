/*
 * Copyright (C) 2002-2009 by the Widelands Development Team
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

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <cassert>
#include <string>
#include <vector>

#include "graphic/graphic.h"
#include "message.h"
#include "ui_basic/button.h"
#include "logic/player.h"

#define NO_NEW_MESSAGES "pics/menu_toggle_oldmessage_menu.png"
#define NEW_MESSAGES "pics/menu_toggle_newmessage_menu.png"

namespace Widelands {
	struct Coords;
	struct GameMessageMenu;

	///the Message manager keeps track of messages
	///based of the objective system

struct MessageQueue {
	void registerButton(UI::Callback_Button<GameMessageMenu>*);

	static std::vector<Message> & get (Player & p)
	{
		return get(p.get_player_number());
	}
	static std::vector<Message> & get (Player_Number const player_number)
	{
		static std::vector<std::vector< Message> > myQueue;
		if (player_number >= myQueue.size())
		{
			myQueue.resize(player_number + 1);
		}
		return myQueue[player_number];
	}

	static void add (Player & p, Message const m)
	{
		add(p.get_player_number(), m);
	}
	static void add (Player_Number player_number, Message m)
	{
		get(player_number).push_back(m);
		m_readall(player_number) = false;
		if (m_button(0))
			m_button(0)->set_pic(g_gr->get_picture(PicMod_Game, NEW_MESSAGES));
	}

	static void read_all (Player & p)
	{
		read_all(p.get_player_number());
	}
	static void read_all (Player_Number player_number)
	{
		m_readall(player_number) = true;
		if (m_button(0))
			m_button(0)->set_pic(g_gr->get_picture(PicMod_Game, NO_NEW_MESSAGES));
	}

	static UI::Callback_Button<Interactive_Player> * m_button
		(UI::Callback_Button<Interactive_Player> * const b)
	{
		static UI::Callback_Button<Interactive_Player> * m_but = 0;
		if (b)
			m_but = b;
		return m_but;
	}

private:
	//use char here instead of bool because bool vectors behave odd
	static char & m_readall(Player_Number const player_number)
	{
		static std::vector<char> m_read;
		if (player_number >= m_read.size()) {
			m_read.resize(player_number + 1);
		}
		return m_read[player_number];
	}



};

};

#endif