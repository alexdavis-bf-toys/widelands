/*
 * Copyright (C) 2004, 2006-2009 by the Widelands Development Team
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

#include "netsetup_ggz.h"

#if HAVE_GGZ

#include "constants.h"
#include "graphic/graphic.h"
#include "i18n.h"
#include "network/network.h"
#include "network/network_ggz.h"
#include "profile/profile.h"

Fullscreen_Menu_NetSetupGGZ::Fullscreen_Menu_NetSetupGGZ ()
:
Fullscreen_Menu_Base("singleplmenu.jpg"), // change this

// Values for alignment and size
m_butx
	(m_xres * 13 / 40),
m_butw
	(m_xres / 4),
m_buth
	(m_yres * 19 / 400),
m_lisw
	(m_xres * 20 / 63),
m_fs
	(fs_small()),
m_fn
	(ui_fn()),

// Text labels
title
	(this,
	 m_xres / 2, m_yres / 20,
	 _("Metaserver Lobby"), Align_HCenter),
m_users
	(this,
	 m_xres * 3 / 50, m_yres * 15 / 100,
	 _("Users online:"), Align_Left),
m_opengames
	(this,
	 m_lisw + m_xres * 1 / 50, m_yres * 15 / 100,
	 _("Games promoted on the metaserver:"), Align_Left),
m_playername
	(this,
	 m_xres * 17 / 25, m_yres * 15 / 100,
	 _("Your nickname:"), Align_Left),
m_servername
	(this,
	 m_xres * 17 / 25, m_yres * 175 / 400,
	 _("Name of your server:"), Align_Left),

// Buttons
joingame
	(this,
	 m_xres * 17 / 25, m_yres * 268 / 1000, m_butw, m_buth,
	 1,
	 &Fullscreen_Menu_NetSetupGGZ::clicked_joingame, *this,
	 _("Join this game"), std::string(), false, false,
	 m_fn, m_fs),
hostgame
	(this,
	 m_xres * 17 / 25, m_yres * 555 / 1000, m_butw, m_buth,
	 1,
	 &Fullscreen_Menu_NetSetupGGZ::clicked_hostgame, *this,
	 _("Host a new game"), std::string(), true, false,
	 m_fn, m_fs),
back
	(this,
	 m_xres * 17 / 25, m_yres * 9 / 10, m_butw, m_buth,
	 0,
	 &Fullscreen_Menu_NetSetupGGZ::end_modal, *this, CANCEL,
	 _("Back"), std::string(), true, false,
	 m_fn, m_fs),

// Edit boxes
playername
	(this, m_xres * 17 / 25, m_yres * 2 / 10,  m_butw, m_buth, 2, 0),
servername
	(this, m_xres * 17 / 25, m_yres * 195 / 400, m_butw, m_buth, 2, 0),

// List
usersonline
	(this,
	  m_xres * 3 / 50, m_yres * 2 / 10,
	  m_lisw * 4 / 5, m_yres * 4 / 10),
opengames
	(this,
	  m_lisw + m_xres * 1 / 50, m_yres * 2 / 10,
	  m_lisw, m_yres * 4 / 10),

// The chat UI
chat(0)
{
	// Set the texts and style of UI elements
	Section & s = g_options.pull_section("global"); //  for playername

	title       .set_font(m_fn, fs_big(), UI_FONT_CLR_FG);
	m_opengames .set_font(m_fn, m_fs, UI_FONT_CLR_FG);
	m_users     .set_font(m_fn, m_fs, UI_FONT_CLR_FG);
	m_playername.set_font(m_fn, m_fs, UI_FONT_CLR_FG);
	m_servername.set_font(m_fn, m_fs, UI_FONT_CLR_FG);
	std::string server = s.get_string("servername", "");
	servername  .setText (server);
	servername  .changed.set
		(this, &Fullscreen_Menu_NetSetupGGZ::change_servername);
	servername  .set_font(m_fn, m_fs, UI_FONT_CLR_FG);
	playername  .setText(s.get_string("nickname", (_("nobody"))));
	playername  .changed.set
		(this, &Fullscreen_Menu_NetSetupGGZ::change_playername);
	playername  .set_font(m_fn, m_fs, UI_FONT_CLR_FG);

	// prepare the lists
	usersonline .set_font(m_fn, m_fs);
	usersonline .add_column(m_lisw * 2 / 5, _("Name"));
	usersonline .add_column(m_lisw * 2 / 5, _("Server"));
	opengames   .set_font(m_fn, m_fs);
	opengames   .add_column(m_lisw * 4 / 7, _("Server"));
	opengames   .add_column(m_lisw * 3 / 7, _("State"));
	opengames   .selected.set
		(this, &Fullscreen_Menu_NetSetupGGZ::server_selected);
	opengames   .double_clicked.set
		(this, &Fullscreen_Menu_NetSetupGGZ::server_doubleclicked);

	// try to connect to the metaserver
	if (!NetGGZ::ref().usedcore())
		connectToMetaserver();
}


/// think function of the UI (main loop)
void Fullscreen_Menu_NetSetupGGZ::think ()
{
	Fullscreen_Menu_Base::think ();

	// If we have no connection try to connect
	if (!NetGGZ::ref().usedcore())
		connectToMetaserver();

	// Check ggz ports for new data
	NetGGZ::ref().data();

	if (!NetGGZ::ref().users().empty())
		fillUserList(NetGGZ::ref().users());

	if (!NetGGZ::ref().tables().empty())
		fillServersList(NetGGZ::ref().tables());

	// Late initialization
	if (!chat) {
		GGZChatProvider & chatprovider = NetGGZ::ref().get_chatprovider();
		chat =
			new GameChatPanel
				(this,
				 m_xres * 3 / 50, m_yres * 125 / 200,
				 m_lisw * 9 / 5 + m_xres * 12 / 500, m_yres * 32 / 100,
				 chatprovider);
	}
}


/// connects Widelands with the metaserver
void Fullscreen_Menu_NetSetupGGZ::connectToMetaserver()
{
	Section & s = g_options.pull_section("global");
	const char *metaserver;
	metaserver = s.get_string("metaserver", WL_METASERVER);

	NetGGZ::ref().initcore(metaserver, playername.text().c_str());
}


/// fills the server list
void Fullscreen_Menu_NetSetupGGZ::fillServersList
	(std::vector<Net_Game_Info> tables)
{
	opengames.clear();
	for (uint32_t i = 0; i < tables.size(); ++i) {
		Net_Open_Game * newOG;
		newOG = new Net_Open_Game;
		newOG->info = Net_Game_Info(tables[i]);
		UI::Table<Net_Open_Game const * const>::Entry_Record & er =
			opengames.add(newOG);

		er.set_string (0, newOG->info.hostname);

		switch (newOG->info.state) {
			case LAN_GAME_OPEN:
				er.set_string(1, _("Open"));
				break;
			case LAN_GAME_CLOSED:
				er.set_string(1, _("Closed"));
				break;
			default:
				er.set_string(1, _("Unknown"));
		}
	}
}


/// fills the user list
void Fullscreen_Menu_NetSetupGGZ::fillUserList
	(std::vector<Net_Player> users)
{
	usersonline.clear();
	for (uint32_t i = 0; i < users.size(); ++i) {
		Net_Player user(users[i]);
		UI::Table<const Net_Player * const>::Entry_Record & er =
			usersonline.add(&user);
			er.set_string(0, users[i].name);
			er.set_string(1, users[i].table);
	}
}


/// \returns the name of the player
std::string const & Fullscreen_Menu_NetSetupGGZ::get_playername()
{
	return playername.text();
}


/// called when an entry of the server list was selected
void Fullscreen_Menu_NetSetupGGZ::server_selected (uint32_t)
{
	if (opengames.has_selection()) {
		if (const Net_Open_Game * const game = opengames.get_selected()) {
			servername.setText(game->info.hostname);
			joingame.set_enabled(true);
			hostgame.set_enabled(false);
		}
	}
}


/// called when an entry of the server list was doubleclicked
void Fullscreen_Menu_NetSetupGGZ::server_doubleclicked (uint32_t)
{
	clicked_joingame();
}


/// called when the servername was changed
void Fullscreen_Menu_NetSetupGGZ::change_servername()
{
	// Allow user to enter a servername manually
	opengames.select(opengames.no_selection_index());
	hostgame.set_enabled(true);
	joingame.set_enabled(false);
}


/// called when the playername was changed
void Fullscreen_Menu_NetSetupGGZ::change_playername()
{
	g_options.pull_section("global").set_string("nickname", playername.text());
	NetGGZ::ref().deinitcore();
	connectToMetaserver();
}


/// called when the 'join game' button was clicked
void Fullscreen_Menu_NetSetupGGZ::clicked_joingame()
{
	NetGGZ::ref().join(servername.text().c_str());
	end_modal(JOINGAME);
}


/// called when the 'host game' button was clicked
void Fullscreen_Menu_NetSetupGGZ::clicked_hostgame()
{
	// Save selected servername as default for next time.
	g_options.pull_section("global").set_string("servername", servername.text());
	NetGGZ::ref().set_local_servername(servername.text());
	end_modal(HOSTGAME);
}

#endif // if HAVE_GGZ