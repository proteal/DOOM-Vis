//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

struct MUSheader_t 
{
	char ID[4];          // identifier "MUS" 0x1A
	ushort scoreLen;
	ushort scoreStart;
	ushort channels;        // count of primary channels
	ushort sec_channels;        // count of secondary channels
	ushort instrCnt;
	ushort dummy;
	//// variable-length part starts here
};

struct MidiHeaderChunk_t 
{
	char name[4];
	int  length;

	short format;                        // make 0
	short ntracks;                        // make 1
	short division;                        // 0xe250??
};

struct MidiTrackChunk_t 
{
	char name[4];
	int length;
};

enum class MusEvent
{
	KEY_OFF,
	KEY_ON,
	PITCH_WHEEL,
	CHANNEL_MODE,
	CONTROLLER_CHANGE,
	UNKNOWN5,
	END,
	UNKNOWN7
};

struct StreamSample
{
	short left;
	short right;
};

struct AudioDefs
{
	static const uint SAMPLE_RATE = 11025;

	static const uint MIDI_MAXCHANNELS = 16;
	static const uint MIDIHEADERSIZE = 14;

	static float MAX_SEP()
	{
		return 254.0f;
	}

	static float MAX_VOL()
	{
		return 127.0f;
	}

	static float NORMAL_PITCH()
	{
		return 128.0f;
	}

	static const uint MIDI_MAP_SIZE = 15;

	static const array<ubyte, MIDI_MAP_SIZE>& MIDI_MAP()
	{
		static array<ubyte, MIDI_MAP_SIZE> midiMap =
		{
			0,                                // prog change
			0,                                // bank sel
			1,        //2                        // mod pot
			0x07,        //3                // volume
			0x0A,        //4                // pan pot
			0x0B,        //5                // expression pot
			0x5B,        //6                // reverb depth
			0x5D,        //7                // chorus depth
			0x40,        //8                // sustain pedal
			0x43,        //9                // soft pedal
			0x78,        //10                // all sounds off
			0x7B,        //11                // all notes off
			0x7E,        //12                // mono(use numchannels + 1)
			0x7F,        //13                // poly
			0x79,        //14        // reset all controllers
		};
		return midiMap;
	}
};
