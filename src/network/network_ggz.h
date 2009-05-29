/*
 * Copyright (C) 2004-2006, 2008 by the Widelands Development Team
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

#ifndef NETWORK_GGZ_H
#define NETWORK_GGZ_H

#ifdef USE_GGZ
#define HAVE_GGZ 1

#define WL_METASERVER "janus.animux.de" // temp. testserver sponsored by janus
#define WL_METASERVER_PORT 5688

#define ERRMSG "</p><p font-size=14 font-color=#ff6633 font-weight=bold>ERROR: "

#include "chat.h"
#include "network_lan_promotion.h"

#include <ggzmod.h>
#include <ggzcore.h>
#include <stdint.h>
#include <string>
#include <vector>

#ifdef WIN32
#include <winsock2.h>
#include <io.h>
#endif

/// A simply network player struct
struct Net_Player {
	std::string table;
	std::string name;
};

/// The GGZChatProvider
struct GGZChatProvider : public ChatProvider {
	GGZChatProvider() {};

	void send(std::string const &);

	std::vector<ChatMessage> const & getMessages() const {
		return messages;
	}

	void receive(ChatMessage const & msg) {
		messages.push_back(msg);
		ChatProvider::send(msg);
	}

private:
	std::vector<ChatMessage> messages;
};

/// The GGZ implementation
struct NetGGZ {
	static NetGGZ & ref();

	void init();
	bool connect();

	bool used();
	bool host();
	void data();
	const char *ip();

	std::vector<Net_Game_Info> tables();
	std::vector<Net_Player>    users();

	void write_userlist();

	enum Protocol
	{
		op_greeting = 1,
		op_request_ip = 2,
		op_reply_ip = 3,
		op_broadcast_ip = 4
	};

	void initcore(const char * metaserver, const char * playername);
	void deinitcore();
	bool usedcore();
	void datacore();
	void launch  ();
	void request_server_ip();
	void join(const char *tablename);

	void set_local_servername(std::string name) {
		if (name.empty())
			name = "WL-Default";
		servername = name;
	}

	// Communication with chatprovider
	GGZChatProvider & get_chatprovider() {
		return chatprovider;
	}
	void chatSendMessage(std::string const &);
	void chatReceiveMessage
		(std::string const &, std::string const &, bool system = false);

private:
	NetGGZ();
	static void ggzmod_server(GGZMod *cbmod, GGZModEvent e, const void *cbdata);
	static GGZHookReturn
		callback_server(uint32_t id, const void *cbdata, const void *user);
	static GGZHookReturn
		callback_room(uint32_t id, const void *cbdata, const void *user);
	static GGZHookReturn
		callback_game(uint32_t id, const void *cbdata, const void *user);
	void event_server(uint32_t id, const void *cbdata);
	void event_room(uint32_t id, const void *cbdata);
	void event_game(uint32_t id, const void *cbdata);

	bool use_ggz;
	int32_t m_fd;
	int32_t channelfd;
	int32_t gamefd;
	int32_t tableid;
	char *server_ip_addr;
	bool ggzcore_login;
	bool ggzcore_ready;
	GGZRoom *room;

	std::string servername;

	std::vector<Net_Game_Info> tablelist;
	std::vector<Net_Player>    userlist;

	GGZChatProvider chatprovider;
};

#endif

#endif