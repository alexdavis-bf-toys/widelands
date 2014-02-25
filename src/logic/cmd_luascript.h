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

#ifndef CMD_LUASCRIPT_H
#define CMD_LUASCRIPT_H

#include <string>

#include "logic/cmd_queue.h"

namespace Widelands {

struct Cmd_LuaScript : public GameLogicCommand {
	Cmd_LuaScript() : GameLogicCommand(0) {} // For savegame loading
	Cmd_LuaScript
		(int32_t const _duetime, std::string ns, std::string script) :
		GameLogicCommand(_duetime), m_ns(ns), m_script(script) {}

	// Write these commands to a file (for savegames)
	void Write(FileWrite &, Editor_Game_Base &, Map_Map_Object_Saver  &) override;
	void Read (FileRead  &, Editor_Game_Base &, Map_Map_Object_Loader &) override;

	virtual uint8_t id() const override {return QUEUE_CMD_LUASCRIPT;}

	virtual void execute(Game &) override;

private:
	std::string m_ns;
	std::string m_script;
};

}

#endif
