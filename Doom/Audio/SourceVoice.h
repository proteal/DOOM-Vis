//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibSourceVoice)
void QueueBuffer(const ubyte* samples, const uint sizeBytes);
void SourcePlayQueued();
void SourceStop();
void SetStereoVolume(float left, float right);
void SetMonoVolume(float volume);
void SetPitch(float pitch);
};

class SourceVoice
{
public:
	SourceVoice(const unique_ptr<LibAudio>& libAudio, int sampleRate, uint sampleBytes,
		uint numChannels, StreamBuffers* streamBuffers)
		:playing(false)
	{
		assert(libAudio);
		libSourceVoice = libAudio->MakeSourceVoice(sampleRate, sampleBytes, numChannels, streamBuffers);
	}

	void SourcePlayQueued()
	{
		assert(libSourceVoice);

		libSourceVoice->SourcePlayQueued();
		playing = true;
	}

	void SourceStop()
	{
		assert(libSourceVoice);

		libSourceVoice->SourceStop();
		playing = false;
	}

	bool IsPlaying() const
	{
		return playing;
	}

	const unique_ptr<LibSourceVoice>& GetLibVoice()
	{
		return libSourceVoice;
	}

private:
	unique_ptr<LibSourceVoice> libSourceVoice;
	bool playing;
};
