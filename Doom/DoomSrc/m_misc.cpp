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
//
// $Log:$
//
// DESCRIPTION:
//	Main loop menu stuff.
//	Default Config File.
//	PCX Screenshots.
//
//-----------------------------------------------------------------------------

#include "doomdef.h"

#include "z_zone.h"

#include "m_swap.h"
#include "m_argv.h"

#include "w_wad.h"

#include "i_system.h"
#include "i_video.h"
#include "v_video.h"

#include "hu_stuff.h"

// State.
#include "doomstat.h"

// Data.
#include "dstrings.h"

#include "m_misc.h"

//
// M_DrawText
// Returns the final X coordinate
// HU_Init must have been called to init the font
//
extern patch_t*		hu_font[HU_FONTSIZE];

int
M_DrawText
(int		x,
int		y,
qboolean	direct,
char*		string)
{
	int 	c;
	int		w;

	while (*string)
	{
		c = toupper(*string) - HU_FONTSTART;
		string++;
		if (c < 0 || c> HU_FONTSIZE)
		{
			x += 4;
			continue;
		}

		w = SHORT(hu_font[c]->width);
		if (x + w > SCREENWIDTH)
			break;
		if (direct)
			V_DrawPatchDirect(x, y, 0, hu_font[c]);
		else
			V_DrawPatch(x, y, 0, hu_font[c]);
		x += w;
	}

	return x;
}




//
// M_WriteFile
//
#ifndef O_BINARY
#define O_BINARY 0
#endif

qboolean
M_WriteFile
(char const*	name,
void*		source,
int		length)
{
	int		handle;
	int		count;

	_sopen_s(&handle, name, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, SH_DENYRW, _S_IWRITE);
	if (handle == -1)
		return false;

	count = _write(handle, source, length);
	_close(handle);

	if (count < length)
		return false;

	return true;
}


//
// M_ReadFile
//
int
M_ReadFile
(char const*	name,
byte**	buffer)
{
	int	handle, count, length;
	struct stat	fileinfo;
	byte		*buf;

	_sopen_s(&handle, name, O_RDONLY | O_BINARY, SH_DENYRW, _S_IREAD);
	if (handle == -1)
		I_Error("Couldn't read file %s", name);
	if (fstat(handle, &fileinfo) == -1)
		I_Error("Couldn't read file %s", name);
	length = fileinfo.st_size;
	buf = reinterpret_cast<byte*>(Z_Malloc(length, PU_STATIC, NULL));
	count = _read(handle, buf, length);
	_close(handle);

	if (count < length)
		I_Error("Couldn't read file %s", name);

	*buffer = buf;
	return length;
}


//
// DEFAULTS
//
int		usemouse;
int snd_SfxVolume = 15;
int snd_MusicVolume = 15;

extern int	key_right;
extern int	key_left;
extern int	key_up;
extern int	key_down;

extern int	key_strafeleft;
extern int	key_straferight;

extern int	key_fire;
extern int	key_use;
extern int	key_strafe;
extern int	key_speed;

extern int	mousebfire;
extern int	mousebstrafe;
extern int	mousebforward;

extern int	viewwidth;
extern int	viewheight;

extern int	mouseSensitivity;
extern int	showMessages;

extern int	screenblocks;

extern int	showMessages;

// machine-independent sound params
extern	int	numChannels;

extern char*	chat_macros[];



typedef struct
{
	char*	name;
	int*	location;
	int		defaultvalue;
	int		scantranslate;		// PC scan code hack
	int		untranslated;		// lousy hack
} default_t;

default_t	defaults[] =
{
	{ "mouse_sensitivity", &mouseSensitivity, 5 },
	{ "sfx_volume", &snd_SfxVolume, 8 },
	{ "music_volume", &snd_MusicVolume, 8 },
	{ "show_messages", &showMessages, 1 },

	{"key_right",&key_right, KEY_RIGHTARROW},
	{"key_left",&key_left, KEY_LEFTARROW},
	{"key_up",&key_up, KEY_UPARROW},
	{"key_down",&key_down, KEY_DOWNARROW},
	{"key_strafeleft",&key_strafeleft, ','},
	{"key_straferight",&key_straferight, '.'},

	{"key_fire",&key_fire, KEY_RCTRL},
	{"key_use",&key_use, ' '},
	{"key_strafe",&key_strafe, KEY_RALT},
	{"key_speed",&key_speed, KEY_RSHIFT},

	{ "use_mouse", &usemouse, 1 },
	{ "mouseb_fire", &mousebfire, 0 },
	{ "mouseb_strafe", &mousebstrafe, 1 },
	{ "mouseb_forward", &mousebforward, 2 },

	{ "screenblocks", &screenblocks, 9 },

	{ "snd_channels", &numChannels, 3 },

	{ "usegamma", &usegamma, 0 },

	{ "chatmacro0", (int *)&chat_macros[0], (int)HUSTR_CHATMACRO0 },
	{ "chatmacro1", (int *)&chat_macros[1], (int)HUSTR_CHATMACRO1 },
	{ "chatmacro2", (int *)&chat_macros[2], (int)HUSTR_CHATMACRO2 },
	{ "chatmacro3", (int *)&chat_macros[3], (int)HUSTR_CHATMACRO3 },
	{ "chatmacro4", (int *)&chat_macros[4], (int)HUSTR_CHATMACRO4 },
	{ "chatmacro5", (int *)&chat_macros[5], (int)HUSTR_CHATMACRO5 },
	{ "chatmacro6", (int *)&chat_macros[6], (int)HUSTR_CHATMACRO6 },
	{ "chatmacro7", (int *)&chat_macros[7], (int)HUSTR_CHATMACRO7 },
	{ "chatmacro8", (int *)&chat_macros[8], (int)HUSTR_CHATMACRO8 },
	{ "chatmacro9", (int *)&chat_macros[9], (int)HUSTR_CHATMACRO9 }

};

int	numdefaults;
char*	defaultfile;


//
// M_SaveDefaults
//
void M_SaveDefaults(void)
{
	int		i;
	int		v;
	FILE*	f;

	if (fopen_s(&f, defaultfile, "w"))
		return; // can't write the file, but don't complain

	for (i = 0; i<numdefaults; i++)
	{
		if (defaults[i].defaultvalue > -0xfff
			&& defaults[i].defaultvalue < 0xfff)
		{
			v = *defaults[i].location;
			fprintf(f, "%s\t\t%i\n", defaults[i].name, v);
		}
		else {
			fprintf(f, "%s\t\t\"%s\"\n", defaults[i].name,
				*(char **)(defaults[i].location));
		}
	}

	fclose(f);
}


//
// M_LoadDefaults
//
extern byte	scantokey[128];

void M_LoadDefaults(void)
{
	int		i;
	int		len;
	FILE*	f;
	const int defSize = 80;
	const int paramSize = 100;
	char	def[defSize];
	char	strparm[paramSize];
	char*	newstring = nullptr;
	int		parm = 0;
	qboolean	isstring;

	// set everything to base values
	numdefaults = sizeof(defaults) / sizeof(defaults[0]);
	for (i = 0; i < numdefaults; i++)
		*defaults[i].location = defaults[i].defaultvalue;

	defaultfile = basedefault;

	// read the file in, overriding any set defaults
	if (fopen_s(&f, defaultfile, "r") == 0)
	{
		while (!feof(f))
		{
			isstring = false;
			if (fscanf_s(f, "%s %[^\n]\n", def, defSize, strparm, paramSize) == 2)
			{
				if (strparm[0] == '"')
				{
					// get a string default
					isstring = true;
					len = strlen(strparm);
					newstring = (char *)malloc(len);
					strparm[len - 1] = 0;
					strcpy_s(newstring, len, strparm + 1);
				}
				else if (strparm[0] == '0' && strparm[1] == 'x')
					sscanf_s(strparm + 2, "%x", &parm);
				else
					sscanf_s(strparm, "%i", &parm);
				for (i = 0; i < numdefaults; i++)
				if (!strcmp(def, defaults[i].name))
				{
					if (!isstring)
						*defaults[i].location = parm;
					else
						*defaults[i].location =
						(int)newstring;
					break;
				}
			}
		}

		fclose(f);
	}
}


//
// SCREEN SHOTS
//


typedef struct
{
	char		manufacturer;
	char		version;
	char		encoding;
	char		bits_per_pixel;

	unsigned short	xmin;
	unsigned short	ymin;
	unsigned short	xmax;
	unsigned short	ymax;

	unsigned short	hres;
	unsigned short	vres;

	unsigned char	palette[48];

	char		reserved;
	char		color_planes;
	unsigned short	bytes_per_line;
	unsigned short	palette_type;

	char		filler[58];
	unsigned char	data;		// unbounded
} pcx_t;
