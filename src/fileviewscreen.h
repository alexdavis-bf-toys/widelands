/*
 * Copyright (C) 2002 by the Widelands Development Team
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

#ifndef __S__FILE_VIEW_SCREEN_H
#define __S__FILE_VIEW_SCREEN_H

class Panel;
struct UniqueWindowRegistry;

void fileview_screen(std::string title, std::string fname);
void textview_screen(std::string title, std::string text);
void fileview_window(Panel* parent, UniqueWindowRegistry* reg, std::string title, std::string fname);
void textview_window(Panel* parent, UniqueWindowRegistry* reg, std::string title, std::string text);

#endif // __S__FILE_VIEW_SCREEN_H
