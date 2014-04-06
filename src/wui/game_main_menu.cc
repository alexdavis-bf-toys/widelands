/*
 * Copyright (C) 2002-2004, 2006, 2008 by the Widelands Development Team
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

#include "wui/game_main_menu.h"

#include <boost/bind.hpp>

#include "graphic/graphic.h"
#include "i18n.h"
#include "ui_basic/unique_window.h"
#include "wui/building_statistics_menu.h"
#include "wui/general_statistics_menu.h"
#include "wui/interactive_player.h"
#include "wui/stock_menu.h"
#include "wui/ware_statistics_menu.h"


GameMainMenu::GameMainMenu
	(Interactive_Player                         & plr,
	 UI::UniqueWindow::Registry                 & registry,
	 Interactive_Player::Game_Main_Menu_Windows & windows)
:
UI::UniqueWindow(&plr, "main_menu", &registry, 180, 55, _("Main Menu")),
m_player      (plr),
m_windows     (windows),
general_stats
	(this, "general_stats",
	 posx(0, 4), posy(0, 3), buttonw(4), buttonh(1),
	 g_gr->images().get("pics/but4.png"),
	 g_gr->images().get("pics/menu_general_stats.png"),
	 _("General Statistics")),
ware_stats
	(this, "ware_stats",
	 posx(1, 4), posy(0, 3), buttonw(4), buttonh(1),
	 g_gr->images().get("pics/but4.png"),
	 g_gr->images().get("pics/menu_ware_stats.png"),
	 _("Ware Statistics")),
building_stats
	(this, "building_stats",
	 posx(2, 4), posy(0, 3), buttonw(4), buttonh(1),
	 g_gr->images().get("pics/but4.png"),
	 g_gr->images().get("pics/menu_building_stats.png"),
	 _("Building Statistics")),
stock
	(this, "stock",
	 posx(3, 4), posy(0, 3), buttonw(4), buttonh(1),
	 g_gr->images().get("pics/but4.png"),
	 g_gr->images().get("pics/menu_stock.png"),
	 _("Stock"))
{
	general_stats.sigclicked.connect
		(boost::bind(&General_Statistics_Menu::Registry::toggle, boost::ref(m_windows.general_stats)));
	ware_stats.sigclicked.connect
		(boost::bind(&UI::UniqueWindow::Registry::toggle, boost::ref(m_windows.ware_stats)));
	building_stats.sigclicked.connect
		(boost::bind(&UI::UniqueWindow::Registry::toggle, boost::ref(m_windows.building_stats)));
	stock.sigclicked.connect
		(boost::bind(&UI::UniqueWindow::Registry::toggle, boost::ref(m_windows.stock)));

#define INIT_BTN_HOOKS(registry, btn)                                        \
 assert (not registry.on_create);                                             \
 assert (not registry.on_delete);                                             \
 registry.on_create = std::bind(&UI::Button::set_perm_pressed, &btn, true);  \
 registry.on_delete = std::bind(&UI::Button::set_perm_pressed, &btn, false); \
 if (registry.window) btn.set_perm_pressed(true);                            \

	INIT_BTN_HOOKS(m_windows.general_stats, general_stats)
	INIT_BTN_HOOKS(m_windows.ware_stats, ware_stats)
	INIT_BTN_HOOKS(m_windows.building_stats, building_stats)
	INIT_BTN_HOOKS(m_windows.stock, stock)

	m_windows.general_stats.open_window = [this] {
		new General_Statistics_Menu(m_player, m_windows.general_stats);
	};
	m_windows.ware_stats.open_window = [this] {
		new Ware_Statistics_Menu(m_player, m_windows.ware_stats);
	};
	m_windows.building_stats.open_window = [this] {
		new Building_Statistics_Menu(m_player, m_windows.building_stats);
	};
	m_windows.stock.open_window = [this] {
		new Stock_Menu(m_player, m_windows.stock);
	};

	if (get_usedefaultpos())
		center_to_parent();
}

GameMainMenu::~GameMainMenu() {
	// We need to remove these callbacks because the opened window might
	// live longer than 'this' window, and thus the buttons. The assertions
	// are safeguards in case somewhere else in the code someone would
	// overwrite our hooks.

#define DEINIT_BTN_HOOKS(registry, btn)                                                            \
	registry.on_create = 0;                                                                         \
	registry.on_delete = 0;

	DEINIT_BTN_HOOKS(m_windows.general_stats, general_stats)
	DEINIT_BTN_HOOKS(m_windows.ware_stats, ware_stats)
	DEINIT_BTN_HOOKS(m_windows.building_stats, building_stats)
	DEINIT_BTN_HOOKS(m_windows.stock, stock)
}
