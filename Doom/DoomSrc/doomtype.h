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
// DESCRIPTION:
//	Simple basic typedefs, isolated here to make it easier
//	 separating modules.
//    
//-----------------------------------------------------------------------------


#ifndef __DOOMTYPE__
#define __DOOMTYPE__

#ifndef __BYTEBOOL__
#define __BYTEBOOL__

typedef int qboolean;
#define FALSE 0
#define TRUE 1

typedef unsigned char byte;
#endif

template<size_t size>
inline void SafeStrncpy(char(&dest)[size], const char* src, size_t count)
{
	assert(count <= size);
	size_t srcIdx = 0;
	for (size_t i = 0; i < count; i++)
	{
		dest[i] = src[srcIdx];
		if (src[srcIdx])
		{
			srcIdx++;
		}
	}
}

template<size_t size>
inline void SafeStrupr(char(&str)[size])
{
	for (size_t i = 0; i < size && str[i]; i++)
	{
		str[i] = static_cast<char>(toupper(str[i]));
	}
}

#endif
