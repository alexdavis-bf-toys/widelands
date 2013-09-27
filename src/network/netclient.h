/*
 * Copyright (C) 2008-2012 by the Widelands Development Team
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

#ifndef NETCLIENT_H
#define NETCLIENT_H

#include "chat.h"
#include "gamecontroller.h"
#include "gamesettings.h"
#include "network/network.h"

struct NetClientImpl;

//FIXME Use composition instead of inheritance
/**
 * NetClient manages the lifetime of a network game in which this computer
 * participates as a client.
 *
 * This includes running the game setup screen and the actual game after
 * launch, as well as dealing with the actual network protocol.
 */
struct NetClient :
	public  GameController,
	public  GameSettingsProvider,
	private SyncCallback,
	public  ChatProvider
{
	NetClient (IPaddress *, const std::string & playername, bool internet = false);
	virtual ~NetClient ();

	void run();

	// GameController interface
	void think();
	void sendPlayerCommand(Widelands::PlayerCommand &);
	int32_t getFrametime();
	std::string getGameDescription();

	uint32_t realSpeed();
	uint32_t desiredSpeed();
	void setDesiredSpeed(uint32_t speed);
	bool isPaused();
	void setPaused(bool paused);
	void report_result
		(uint8_t player, Widelands::PlayerEndResult result, const std::string & info);
	// End GameController interface

	// GameSettingsProvider interface
	virtual const GameSettings & settings();

	virtual void setScenario(bool);
	virtual bool canChangeMap();
	virtual bool canChangePlayerState(uint8_t number);
	virtual bool canChangePlayerTribe(uint8_t number);
	virtual bool canChangePlayerInit (uint8_t number);
	virtual bool canChangePlayerTeam (uint8_t number);

	virtual bool canLaunch();

	virtual void setMap
		(const std::string & mapname,
		 const std::string & mapfilename,
		 uint32_t maxplayers,
		 bool savegame = false);
	virtual void setPlayerState    (uint8_t number, PlayerSettings::State state);
	virtual void setPlayerAI       (uint8_t number, const std::string & ai, bool const random_ai = false);
	virtual void nextPlayerState   (uint8_t number);
	virtual void setPlayerTribe   (uint8_t number, const std::string & tribe, bool const random_tribe = false);
	virtual void setPlayerInit     (uint8_t number, uint8_t index);
	virtual void setPlayerName     (uint8_t number, const std::string & name);
	virtual void setPlayer         (uint8_t number, PlayerSettings ps);
	virtual void setPlayerNumber   (uint8_t number);
	virtual void setPlayerTeam     (uint8_t number, Widelands::TeamNumber team);
	virtual void setPlayerCloseable(uint8_t number, bool closeable);
	virtual void setPlayerShared   (uint8_t number, uint8_t shared);
	virtual void setWinCondition   (std::string);
	virtual void nextWinCondition  ();
	virtual std::string getWinCondition();

	// ChatProvider interface
	void send(const std::string & msg);
	const std::vector<ChatMessage> & getMessages() const;

private:
	/// for unique backupname
	std::string backupFileName(std::string & path) {return path + "~backup";}

	NetTransferFile * file;

	void syncreport();

	void handle_packet(RecvPacket &);
	void handle_network ();
	void sendTime();
	void recvOnePlayer(uint8_t  number, Widelands::StreamRead &);
	void recvOneUser  (uint32_t number, Widelands::StreamRead &);
	void disconnect
		(const std::string & reason, const std::string & arg = "", bool sendreason = true, bool showmsg = true);

	NetClientImpl * d;
	bool m_internet;
	bool m_dedicated_access;
	bool m_dedicated_temp_scenario;
};

#endif
