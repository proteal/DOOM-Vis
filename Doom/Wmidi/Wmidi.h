//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibMidi::Impl
{
public:
	explicit Impl()
	{
		if (WildMidi_Init("Data/dgguspat/timidity.cfg", AudioDefs::SAMPLE_RATE, WM_MO_LOG_VOLUME) != 0)
		{
			THROW("WildMidi_Init failed.");
		}
	}

	~Impl()
	{
		WildMidi_Shutdown();
	}
};

unique_ptr<LibMidiTrack> LibMidi::MakeTrack(const vector<ubyte>& midiData)
{
	return make_unique<LibMidiTrack>(make_unique<LibMidiTrack::Impl>(midiData));
}
