#include "../stdafx.h"
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM Network game communication and protocol,
//	all OS independend parts.
//
//-----------------------------------------------------------------------------

#include "m_menu.h"
#include "i_system.h"
#include "i_video.h"
#include "i_net.h"
#include "g_game.h"
#include "doomdef.h"
#include "doomstat.h"

#include "DoomCallbacks.h"

#define	NCMD_EXIT		0x80000000
#define	NCMD_RETRANSMIT		0x40000000
#define	NCMD_SETUP		0x20000000
#define	NCMD_KILL		0x10000000	// kill game
#define	NCMD_CHECKSUM	 	0x0fffffff


doomcom_t*	doomcom;
doomdata_t*	netbuffer;		// points inside doomcom


//
// NETWORKING
//
// gametic is the tic about to (or currently being) run
// maketic is the tick that hasn't had control made for it yet
// nettics[] has the maketics for all players 
//
// a gametic cannot be run until nettics[] > gametic for all players
//
#define	RESENDCOUNT	10
#define	PL_DRONE	0x80	// bit flag in doomdata->player

ticcmd_t	localcmds[BACKUPTICS];

ticcmd_t        netcmds[MAXPLAYERS][BACKUPTICS];
int         	nettics[MAXNETNODES];
qboolean		nodeingame[MAXNETNODES];		// set false as nodes leave game
qboolean		remoteresend[MAXNETNODES];		// set when local needs tics
int		resendto[MAXNETNODES];			// set when remote needs tics
int		resendcount[MAXNETNODES];

int		nodeforplayer[MAXPLAYERS];

int             maketic;
int		lastnettic;
int		skiptics;
int		ticdup;
int		maxsend;	// BACKUPTICS/(2*ticdup)-1


void D_ProcessEvents(void);
void G_BuildTiccmd(ticcmd_t *cmd);
void D_DoAdvanceDemo(void);

qboolean		reboundpacket;
doomdata_t	reboundstore;



//
//
//
int NetbufferSize(void)
{
	return (int)&(((doomdata_t *)0)->cmds[netbuffer->numtics]);
}

//
// Checksum 
//
unsigned NetbufferChecksum(void)
{
	unsigned		c;
	int		i, l;

	c = 0x1234567;

	// FIXME -endianess?
	l = (NetbufferSize() - (int)&(((doomdata_t *)0)->retransmitfrom)) / 4;
	for (i = 0; i < l; i++)
		c += ((unsigned *)&netbuffer->retransmitfrom)[i] * (i + 1);

	return c & NCMD_CHECKSUM;
}

//
//
//
int ExpandTics(int low)
{
	int	delta;

	delta = low - (maketic & 0xff);

	if (delta >= -64 && delta <= 64)
		return (maketic&~0xff) + low;
	if (delta > 64)
		return (maketic&~0xff) - 256 + low;
	if (delta < -64)
		return (maketic&~0xff) + 256 + low;

	I_Error("ExpandTics: strange value %i at maketic %i", low, maketic);
#ifdef _DEBUG
	return 0;
#endif
}



//
// HSendPacket
//
void
HSendPacket
(int	node,
int	flags)
{
	node;

	netbuffer->checksum = NetbufferChecksum() | flags;
	assert(!node);

	reboundstore = *netbuffer;
	reboundpacket = true;
}

//
// HGetPacket
// Returns false if no packet is waiting
//
qboolean HGetPacket(void)
{
	if (reboundpacket)
	{
		*netbuffer = reboundstore;
		doomcom->remotenode = 0;
		reboundpacket = false;
		return true;
	}

	assert(!netgame);
	return false;
}


//
// GetPackets
//
char    exitmsg[80];

void GetPackets(void)
{
	int		netconsole;
	int		netnode;
	ticcmd_t	*src, *dest;
	int		realend;
	int		realstart;

	while (HGetPacket())
	{
		if (netbuffer->checksum & NCMD_SETUP)
			continue;		// extra setup packet

		netconsole = netbuffer->player & ~PL_DRONE;
		netnode = doomcom->remotenode;

		// to save bytes, only the low byte of tic numbers are sent
		// Figure out what the rest of the bytes are
		realstart = ExpandTics(netbuffer->starttic);
		realend = (realstart + netbuffer->numtics);

		// check for exiting the game
		if (netbuffer->checksum & NCMD_EXIT)
		{
			if (!nodeingame[netnode])
				continue;
			nodeingame[netnode] = false;
			playeringame[netconsole] = false;
			strcpy_s(exitmsg, "Player 1 left the game");
			exitmsg[7] += static_cast<char>(netconsole);
			players[consoleplayer].message = exitmsg;
			continue;
		}

		// check for a remote game kill
		if (netbuffer->checksum & NCMD_KILL)
			I_Error("Killed by network driver");

		nodeforplayer[netconsole] = netnode;

		// check for retransmit request
		if (resendcount[netnode] <= 0
			&& (netbuffer->checksum & NCMD_RETRANSMIT))
		{
			resendto[netnode] = ExpandTics(netbuffer->retransmitfrom);
			resendcount[netnode] = RESENDCOUNT;
		}
		else
			resendcount[netnode]--;

		// check for out of order / duplicated packet		
		if (realend == nettics[netnode])
			continue;

		if (realend < nettics[netnode])
		{
			continue;
		}

		// check for a missed packet
		if (realstart > nettics[netnode])
		{
			// stop processing until the other system resends the missed tics
			remoteresend[netnode] = true;
			continue;
		}

		// update command store from the packet
		{
			int		start;

			remoteresend[netnode] = false;

			start = nettics[netnode] - realstart;
			src = &netbuffer->cmds[start];

			while (nettics[netnode] < realend)
			{
				dest = &netcmds[netconsole][nettics[netnode] % BACKUPTICS];
				nettics[netnode]++;
				*dest = *src;
				src++;
			}
		}
	}
}


//
// NetUpdate
// Builds ticcmds for console player,
// sends out a packet
//
int      gametime;

void NetUpdate(void)
{
	int             nowtime;
	int             newtics;
	int				i, j;
	int				realstart;
	int				gameticdiv;

	// check time
	nowtime = I_GetTime() / ticdup;
	newtics = nowtime - gametime;
	gametime = nowtime;

	if (newtics <= 0) 	// nothing new to update
		goto listen;

	if (skiptics <= newtics)
	{
		newtics -= skiptics;
		skiptics = 0;
	}
	else
	{
		skiptics -= newtics;
		newtics = 0;
	}


	netbuffer->player = static_cast<byte>(consoleplayer);

	// build new ticcmds for console player
	gameticdiv = gametic / ticdup;
	for (i = 0; i < newtics; i++)
	{
		I_StartTic();
		D_ProcessEvents();
		if (maketic - gameticdiv >= BACKUPTICS / 2 - 1)
			break;          // can't hold any more

		G_BuildTiccmd(&localcmds[maketic%BACKUPTICS]);
		maketic++;
	}

	// send the packet to the other nodes
	for (i = 0; i<doomcom->numnodes; i++)
	if (nodeingame[i])
	{
		netbuffer->starttic = static_cast<byte>(realstart = resendto[i]);
		netbuffer->numtics = static_cast<byte>(maketic - realstart);
		if (netbuffer->numtics > BACKUPTICS)
			I_Error("NetUpdate: netbuffer->numtics > BACKUPTICS");

		resendto[i] = maketic - doomcom->extratics;

		for (j = 0; j < netbuffer->numtics; j++)
			netbuffer->cmds[j] =
			localcmds[(realstart + j) % BACKUPTICS];

		if (remoteresend[i])
		{
			netbuffer->retransmitfrom = static_cast<byte>(nettics[i]);
			HSendPacket(i, NCMD_RETRANSMIT);
		}
		else
		{
			netbuffer->retransmitfrom = 0;
			HSendPacket(i, 0);
		}
	}

	// listen for other packets
listen:
	GetPackets();
}



//
// CheckAbort
//
void CheckAbort(void)
{
	event_t *ev;
	int		stoptic;

	stoptic = I_GetTime() + 2;
	while (I_GetTime() < stoptic)
		I_StartTic();

	I_StartTic();
	for (; eventtail != eventhead
		; eventtail = (++eventtail)&(MAXEVENTS - 1))
	{
		ev = &events[eventtail];
		if (ev->type == ev_keydown && ev->data1 == KEY_ESCAPE)
			I_Error("Network game synchronization aborted.");
	}
}

//
// D_CheckNetGame
// Works out player numbers among the net participants
//
extern	int			viewangleoffset;

void D_CheckNetGame(void)
{
	int             i;

	for (i = 0; i < MAXNETNODES; i++)
	{
		nodeingame[i] = false;
		nettics[i] = 0;
		remoteresend[i] = false;	// set when local needs tics
		resendto[i] = 0;		// which tic to start sending
	}

	// I_InitNetwork sets doomcom and netgame
	I_InitNetwork();
	if (doomcom->id != DOOMCOM_ID)
		I_Error("Doomcom buffer invalid!");

	netbuffer = &doomcom->data;
	consoleplayer = displayplayer = doomcom->consoleplayer;
	assert(!netgame);

	doomCallbacks->Printf("startskill %i  deathmatch: %i  startmap: %i  startepisode: %i\n",
		startskill, deathmatch, startmap, startepisode);

	// read values out of doomcom
	ticdup = doomcom->ticdup;
	maxsend = BACKUPTICS / (2 * ticdup) - 1;
	if (maxsend < 1)
		maxsend = 1;

	for (i = 0; i < doomcom->numplayers; i++)
		playeringame[i] = true;
	for (i = 0; i < doomcom->numnodes; i++)
		nodeingame[i] = true;

	doomCallbacks->Printf("player %i of %i (%i nodes)\n",
		consoleplayer + 1, doomcom->numplayers, doomcom->numnodes);

}


//
// D_QuitNetGame
// Called before quitting to leave a net game
// without hanging the other players
//
void D_QuitNetGame(void)
{
	int             i, j;

	if (!netgame || !usergame || consoleplayer == -1 || demoplayback)
		return;

	// send a bunch of packets for security
	netbuffer->player = static_cast<byte>(consoleplayer);
	netbuffer->numtics = 0;
	for (i = 0; i < 4; i++)
	{
		for (j = 1; j < doomcom->numnodes; j++)
		if (nodeingame[j])
			HSendPacket(j, NCMD_EXIT);
	}
}



//
// TryRunTics
//
int	frametics[4];
int	frameon;
int	frameskip[4];
int	oldnettics;

extern	qboolean	advancedemo;

void TryRunTics(int& elapsedTicks)
{
	int		i;
	int		lowtic;
	int		entertic;
	static int	oldentertics;
	int		realtics;
	int		availabletics;
	int		counts;
	int		numplaying;

	// get real tics		
	entertic = I_GetTime() / ticdup;
	realtics = entertic - oldentertics;
	oldentertics = entertic;
	elapsedTicks = 0;

	// get available tics
	NetUpdate();

	lowtic = MAXINT;
	numplaying = 0;
	for (i = 0; i < doomcom->numnodes; i++)
	{
		if (nodeingame[i])
		{
			numplaying++;
			if (nettics[i] < lowtic)
				lowtic = nettics[i];
		}
	}
	availabletics = lowtic - gametic / ticdup;

	// decide how many tics to run
	if (realtics < availabletics - 1)
		counts = realtics + 1;
	else if (realtics < availabletics)
		counts = realtics;
	else
		counts = availabletics;

	if (counts < 1)
		counts = 1;

	frameon++;

	if (!demoplayback)
	{
		// ideally nettics[0] should be 1 - 3 tics above lowtic
		// if we are consistantly slower, speed up time
		for (i = 0; i < MAXPLAYERS; i++)
		if (playeringame[i])
			break;
		if (consoleplayer == i)
		{
			// the key player does not adapt
		}
		else
		{
			if (nettics[0] <= nettics[nodeforplayer[i]])
			{
				gametime--;
			}
			frameskip[frameon & 3] = (oldnettics > nettics[nodeforplayer[i]]);
			oldnettics = nettics[0];
			if (frameskip[0] && frameskip[1] && frameskip[2] && frameskip[3])
			{
				skiptics = 1;
			}
		}
	}// demoplayback

	// wait for new tics if needed
	while (lowtic < gametic / ticdup + counts)
	{
		doomCallbacks->Sleep();
		NetUpdate();
		lowtic = MAXINT;

		for (i = 0; i < doomcom->numnodes; i++)
		if (nodeingame[i] && nettics[i] < lowtic)
			lowtic = nettics[i];

		if (lowtic < gametic / ticdup)
			I_Error("TryRunTics: lowtic < gametic");

		// don't stay in here forever -- give the menu a chance to work
		if (I_GetTime() / ticdup - entertic >= 20)
		{
			M_Ticker();
			return;
		}
	}

	// run the count * ticdup dics
	elapsedTicks = counts;
	while (counts--)
	{
		for (i = 0; i<ticdup; i++)
		{
			if (gametic / ticdup > lowtic)
				I_Error("gametic>lowtic");
			if (advancedemo)
				D_DoAdvanceDemo();
			M_Ticker();
			G_Ticker();
			gametic++;

			// modify command for duplicated tics
			if (i != ticdup - 1)
			{
				ticcmd_t	*cmd;
				int			buf;
				int			j;

				buf = (gametic / ticdup) % BACKUPTICS;
				for (j = 0; j < MAXPLAYERS; j++)
				{
					cmd = &netcmds[j][buf];
					cmd->chatchar = 0;
					if (cmd->buttons & BT_SPECIAL)
						cmd->buttons = 0;
				}
			}
		}
		NetUpdate();	// check for new console commands
	}
}
