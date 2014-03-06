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
//	System interface for sound.
//
//-----------------------------------------------------------------------------

#include "z_zone.h"

#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"

#include "doomdef.h"

#include "DoomCallbacks.h"

//
// Retrieve the raw data lump index
//  for a given SFX name.
//
int I_GetSfxLumpNum(sfxinfo_t* sfx)
{
	if (sfx->link)
	{
		sfx = sfx->link;
	}

	char namebuf[9];
	sprintf_s(namebuf, "ds%s", sfx->name);
	return W_GetNumForName(namebuf);
}

// Load and convert a sound effect
// Returns true if successful
bool CacheSFX(sfxinfo_t *sfxinfo)
{
	assert(sfxinfo);

	int lumpnum;
	unsigned int lumplen;
	unsigned int length;
	byte *data;

	// need to load the sound

	lumpnum = sfxinfo->lumpnum;
	void* lump = W_CacheLumpNum(lumpnum, PU_STATIC);

	data = reinterpret_cast<byte*>(lump);
	lumplen = W_LumpLength(lumpnum);

	// Check the header, and ensure this is a valid sound
	if (lumplen < 8 || data[0] != 0x03 || data[1] != 0x00)
	{
		// Invalid sound
		return false;
	}

	// 16 bit sample rate field, 32 bit length field
	sfxinfo->samplerate = (data[3] << 8) | data[2];
	length = (data[7] << 24) | (data[6] << 16) | (data[5] << 8) | data[4];
	data += 8;

	// If the header specifies that the length of the sound is greater than
	// the length of the lump itself, this is an invalid sound lump

	// We also discard sound lumps that are less than 49 samples long,
	// as this is how DMX behaves - although the actual cut-off length
	// seems to vary slightly depending on the sample rate.  This needs
	// further investigation to better understand the correct
	// behavior.
	if (length > lumplen - 8 || length <= 48)
	{
		return false;
	}

	// The DMX sound library seems to skip the first 16 and last 16
	// bytes of the lump - reason unknown.
	data += 16;
	length -= 32;

	sfxinfo->samples = vector<ubyte>(data, data + length);

	// don't need the original lump any more
	Z_Free(lump);

	return true;
}

void CheckVolumeSeparation(int *vol, int *sep)
{
	if (*sep < 0)
	{
		*sep = 0;
	}
	else if (*sep > 254)
	{
		*sep = 254;
	}

	if (*vol < 0)
	{
		*vol = 0;
	}
	else if (*vol > 127)
	{
		*vol = 127;
	}
}

//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is set, but currently not used by mixing.
//
int I_StartSound(sfxinfo_t*	sfxinfo, int channel, int vol, int sep, int pitch)
{
	assert(sfxinfo);

	CheckVolumeSeparation(&vol, &sep);

	if (sfxinfo->samples.empty())
	{
		if (!CacheSFX(sfxinfo))
		{
			return -1;
		}
	}

	doomCallbacks->PlaySound(sfxinfo->samples, sfxinfo->samplerate, channel, vol, sep, pitch);
	return channel;
}

void I_StopSound(int handle)
{
	if (handle >= 0)
	{
		doomCallbacks->StopSound(handle);
	}
}

//
// MUSIC API.
// Still no music done.
// Remains. Dummies.
//

void I_PauseSong(int handle)
{
	//TODO
	// UNUSED.
	handle = 0;
}

void I_ResumeSong(int handle)
{
	//TODO
	// UNUSED.
	handle = 0;
}
