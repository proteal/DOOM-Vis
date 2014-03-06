// Emacs style mode select   -*- C++ -*- 
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
//
// DESCRIPTION:
//	System interface, sound.
//
//-----------------------------------------------------------------------------

#ifndef __I_SOUND__
#define __I_SOUND__

#include "doomdef.h"

#include "doomstat.h"
#include "sounds.h"

//
//  SFX I/O
//

// Get raw data lump index for sound descriptor.
int I_GetSfxLumpNum (sfxinfo_t* sfxinfo );

// Starts a sound in a particular sound channel.
int I_StartSound(sfxinfo_t*	sfxinfo, int channel, int vol, int sep, int pitch);

// Stops a sound channel.
void I_StopSound(int handle);

// PAUSE game handling.
void I_PauseSong(int handle);
void I_ResumeSong(int handle);

#endif
