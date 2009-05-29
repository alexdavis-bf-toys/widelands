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

#include "multiplayer.h"

#if HAVE_GGZ

#include "constants.h"
#include "i18n.h"

Fullscreen_Menu_MultiPlayer::Fullscreen_Menu_MultiPlayer() :
Fullscreen_Menu_Base("singleplmenu.jpg"),

// Values for alignment and size
m_butw
	(m_xres * 7 / 20),
m_buth
	(m_yres * 19 / 400),
m_butx
	((m_xres - m_butw) / 2),
m_fs
	(fs_small()),
m_fn
	(ui_fn()),

// Title
title
	(this,
	 m_xres / 2, m_yres * 3 / 40,
	 _("Choose game type"), Align_HCenter),

// Buttons
metaserver
	(this,
	 m_butx, m_yres * 6 / 25, m_butw, m_buth,
	 1,
	 &Fullscreen_Menu_MultiPlayer::end_modal, *this, Metaserver,
	 _("Internet game"), std::string(), true, false,
	 m_fn, m_fs),
lan
	(this,
	 m_butx, m_yres * 61 / 200, m_butw, m_buth,
	 1,
	 &Fullscreen_Menu_MultiPlayer::end_modal, *this, Lan,
	 _("LAN / Direct IP"), std::string(), true, false,
	 m_fn, m_fs),
back
	(this,
	 m_butx, m_yres * 3 / 4, m_butw, m_buth,
	 0,
	 &Fullscreen_Menu_MultiPlayer::end_modal, *this, Back,
	 _("Back"), std::string(), true, false,
	 m_fn, m_fs)
{title.set_font(m_fn, fs_big(), UI_FONT_CLR_FG);}

#endif // if HAVE_GGZ