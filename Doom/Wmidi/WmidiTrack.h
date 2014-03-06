//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibMidiTrack::Impl
{
	friend class LibMidiTrack;
public:
	explicit Impl(const vector<ubyte>& midiData)
	{
		wmHandle = WildMidi_OpenBuffer(const_cast<ubyte*>(midiData.data()), midiData.size());
		if (!wmHandle)
		{
			THROW("WildMidi_OpenBuffer failed.")
		}
	}

	~Impl()
	{
		assert(wmHandle);
		WildMidi_Close(wmHandle);
	}

private:
	midi* wmHandle;
};

inline int LibMidiTrack::WriteSamples(vector<StreamSample>& buffer)
{
	assert(impl->wmHandle);
	return WildMidi_GetOutput(impl->wmHandle, reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(StreamSample));
}

inline void LibMidiTrack::ResetTrack()
{
	assert(impl->wmHandle);

	unsigned long int pos = 0;
	WildMidi_FastSeek(impl->wmHandle, &pos);
}
