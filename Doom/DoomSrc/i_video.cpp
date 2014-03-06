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
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

#include "DoomCallbacks.h"

//
// I_StartTic
//
void I_StartTic(void)
{
	vector<event_t> events;
	doomCallbacks->PopInputEvents(events);

	for (const event_t& evt : events)
	{
		D_PostEvent(&evt);
	}
}

//
// I_ReadScreen
//
void I_ReadScreen(byte* scr)
{
	memcpy(scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}

//
// I_SetPalette
//
void I_SetPalette(byte* palette)
{
	doomCallbacks->SetPalette(palette);
}
