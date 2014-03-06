//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibMidiTrack)
int WriteSamples(vector<StreamSample>& buffer);
void ResetTrack();
};

class MidiTrack
{
public:
	MidiTrack(const unique_ptr<LibMidi>& libMidi, const vector<ubyte>& midiData)
	{
		assert(libMidi);
		libMidiTrack = libMidi->MakeTrack(midiData);
	}

	int WriteSamples(vector<StreamSample>& buffer)
	{
		assert(libMidiTrack);

		int bytesWritten = libMidiTrack->WriteSamples(buffer);

		assert(bytesWritten % sizeof(StreamSample) == 0);
		return bytesWritten / sizeof(StreamSample);
	}

	void ResetTrack()
	{
		assert(libMidiTrack);
		libMidiTrack->ResetTrack();
	}

private:
	unique_ptr<LibMidiTrack> libMidiTrack;
};
