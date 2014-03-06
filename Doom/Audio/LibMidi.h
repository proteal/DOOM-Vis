//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibMidi)

unique_ptr<class LibMidiTrack> MakeTrack(const vector<ubyte>& midiData);

};
