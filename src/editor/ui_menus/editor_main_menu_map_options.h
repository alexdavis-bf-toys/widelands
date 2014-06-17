/*
 * Copyright (C) 2002-2004, 2006-2011 by the Widelands Development Team
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

#ifndef EDITOR_MAIN_MENU_MAP_OPTIONS_H
#define EDITOR_MAIN_MENU_MAP_OPTIONS_H

#include "ui_basic/button.h"
#include "ui_basic/window.h"

struct Editor_Interactive;
namespace UI {
struct EditBox;
struct Multiline_Editbox;
struct Textarea;
}

/**
 * This is the Main Options Menu. Here, information
 * about the current map are displayed and you can change
 * author, name and description
*/
struct Main_Menu_Map_Options : public UI::Window {
	Main_Menu_Map_Options(Editor_Interactive &);

private:
	Editor_Interactive & eia();
	void changed(int32_t);
	void editbox_changed();
	UI::Multiline_Editbox * m_descr;
	UI::Textarea * m_nrplayers, * m_size;
	UI::EditBox * m_name, * m_author;
	void update();
};

#endif
