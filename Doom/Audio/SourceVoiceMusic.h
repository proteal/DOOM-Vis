//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class SourceVoiceMusic : public SourceVoice
{
public:
	SourceVoiceMusic(const unique_ptr<LibAudio>& libAudio, StreamBuffers& _streamBuffers)
		:SourceVoice(libAudio, AudioDefs::SAMPLE_RATE, sizeof(short), 2, &_streamBuffers)
		, streamBuffers(_streamBuffers)
	{
	}

	void StreamBuffer(vector<StreamSample>& chunk)
	{
		assert(GetLibVoice());

		bool queueEmpty;
		bool queueFull;
		streamBuffers.GetQueueState(queueEmpty, queueFull);

		if (queueFull && !IsPlaying())
		{
			SourcePlayQueued();
		}
		else if (queueEmpty && IsPlaying())
		{
			SourceStop();
		}

		const vector<StreamSample>& buffer = streamBuffers.AddBuffer(chunk);
		GetLibVoice()->QueueBuffer(reinterpret_cast<const ubyte*>(buffer.data()), buffer.size() * sizeof(StreamSample));
	}

	void SetVolume(float volume)
	{
		assert(GetLibVoice());
		GetLibVoice()->SetMonoVolume(volume);
	}

private:
	StreamBuffers& streamBuffers;
};
