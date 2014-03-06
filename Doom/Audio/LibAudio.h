//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibAudio)

unique_ptr<class LibSourceVoice> MakeSourceVoice(int sampleRate, uint sampleBytes, uint channels, StreamBuffers* streamBuffers);

};
