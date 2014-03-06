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
//
//-----------------------------------------------------------------------------

#include "doomdef.h"
#include "m_misc.h"
#include "i_video.h"
#include "i_sound.h"

#include "d_net.h"
#include "g_game.h"

#include "i_system.h"

#include "DoomCallbacks.h"

int	mb_used = 6;

ticcmd_t	emptycmd;
ticcmd_t*	I_BaseTiccmd(void)
{
    return &emptycmd;
}

byte* I_ZoneBase (int*	size)
{
    *size = mb_used*1024*1024;
    return (byte *) malloc (*size);
}

//
// I_GetTime
// returns time in 1/70th second tics
//
int  I_GetTime (void)
{
	static long long basetime = 0;

	long long now = doomCallbacks->GetMicroSecs();
	if (!basetime)
	{
		basetime = now;
	}

	return static_cast<int>((now - basetime) * TICRATE / 1000000);
}

//
// I_Quit
//
void I_Quit (void)
{
    D_QuitNetGame ();
    M_SaveDefaults ();
	doomCallbacks->EndGame();
}

byte*	I_AllocLow(int length)
{
    byte*	mem;
        
    mem = (byte *)malloc (length);
    memset (mem,0,length);
    return mem;
}


//
// I_Error
//
void I_Error (char *error, ...)
{
    va_list	argptr;
    // Message first.
    va_start (argptr,error);

	char buffer[1024];
	vsprintf_s(buffer, error, argptr);

    va_end (argptr);

    // Shutdown. Here might be other errors.
    D_QuitNetGame ();

 	THROW(buffer);
}
