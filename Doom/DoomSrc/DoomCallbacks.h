//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

#include "d_event.h"
#include "r_defs.h"

struct IDoomCallbacks
{
public:
	virtual void Printf(const char* str, ...) = 0;
	virtual int64 GetMicroSecs() = 0;
	virtual void SetPalette(const ubyte* _palette) = 0;
	virtual void Sleep() = 0;
	virtual void EndGame() = 0;
	virtual void PopInputEvents(vector<event_t>& buffer) = 0;
	virtual void SetUsingCursor(bool usingCursor) = 0;
	virtual void UpdateRenderData(const vector<BspNodeInfo>& nodes, const vector<SegInfo>& segs) = 0;
	virtual void InitSound(int numChannels) = 0;
	virtual void PlaySound(const vector<ubyte>& samples, int sampleRate, int channel, int vol, int sep, int pitch) = 0;
	virtual void UpdateSound(int channel, int vol, int sep) = 0;
	virtual void StopSound(int channel) = 0;
	virtual void PlaySong(const ubyte* data, int length, bool loop) = 0;
	virtual void StopSong() = 0;
	virtual void SetMusicVolume(int volume) = 0;
};

extern IDoomCallbacks* doomCallbacks;
