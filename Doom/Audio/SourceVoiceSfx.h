//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class SourceVoiceSfx : public SourceVoice
{
public:
	SourceVoiceSfx(const unique_ptr<LibAudio>& libAudio, int sampleRate)
		:SourceVoice(libAudio, sampleRate, sizeof(ubyte), 1, nullptr)
	{
	}

	void SourcePlay(vector<ubyte>& _samples)
	{
		assert(GetLibVoice());
		samples = move(_samples);

		GetLibVoice()->QueueBuffer(samples.data(), samples.size());
		SourcePlayQueued();
	}

	void SetVolume(float left, float right)
	{
		assert(GetLibVoice());
		GetLibVoice()->SetStereoVolume(left, right);
	}

	void SetPitch(float pitch)
	{
		assert(GetLibVoice());
		GetLibVoice()->SetPitch(pitch);
	}

private:
	vector<ubyte> samples;
};
