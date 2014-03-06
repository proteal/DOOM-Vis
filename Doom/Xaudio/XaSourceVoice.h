//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibSourceVoice::Impl
{
	friend class LibSourceVoice;
public:
	explicit Impl(XaGlobal& _global, int sampleRate, uint sampleBytes, uint channels, StreamBuffers* streamBuffers)
		:global(_global)
		, voice(nullptr)
		, isStream(streamBuffers != nullptr)
		, callback(streamBuffers)
	{
		WAVEFORMATEX format;

		format.wFormatTag = WAVE_FORMAT_PCM;
		format.nChannels = static_cast<WORD>(channels);
		format.nSamplesPerSec = sampleRate;
		format.wBitsPerSample = static_cast<WORD>(8 * sampleBytes);
		format.nBlockAlign = static_cast<WORD>(sampleBytes * format.nChannels);
		format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
		format.cbSize = 0;

		IXAudio2VoiceCallback* callbackArg = isStream ? &callback : nullptr;
		global.xAudio->CreateSourceVoice(&voice, &format, 0, 2.0f, callbackArg, nullptr, nullptr);
		if (!voice)
		{
			THROW("CreateSourceVoice failed.");
		}
	}

	~Impl()
	{
		assert(voice);
		voice->DestroyVoice();
	}

	void operator =(const Impl&) = delete;

private:
	XaGlobal& global;
	IXAudio2SourceVoice* voice;
	XaVoiceCallback callback;
	bool isStream;
};

inline void LibSourceVoice::QueueBuffer(const ubyte* samples, const uint sizeBytes)
{
	assert(impl->voice);

	XAUDIO2_BUFFER buffer;
	buffer.Flags = impl->isStream ? 0 : XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = sizeBytes;
	buffer.pAudioData = samples;
	buffer.PlayBegin = 0;
	buffer.PlayLength = 0;
	buffer.LoopBegin = 0;
	buffer.LoopLength = 0;
	buffer.LoopCount = 0;
	buffer.pContext = nullptr;

	impl->voice->SubmitSourceBuffer(&buffer, nullptr);
}

inline void LibSourceVoice::SourcePlayQueued()
{
	assert(impl->voice);
	impl->voice->Start(0, XAUDIO2_COMMIT_NOW);
}

inline void LibSourceVoice::SourceStop()
{
	assert(impl->voice);

	impl->voice->Stop();
	impl->voice->FlushSourceBuffers();
}

inline void LibSourceVoice::SetStereoVolume(float left, float right)
{
	assert(impl->voice);

	const float matrix[] = { left, right };
	impl->voice->SetOutputMatrix(nullptr, 1, 2, matrix, XAUDIO2_COMMIT_NOW);
}

inline void LibSourceVoice::SetMonoVolume(float volume)
{
	assert(impl->voice);
	impl->voice->SetVolume(volume);
}

inline void LibSourceVoice::SetPitch(float pitch)
{
	assert(impl->voice);
	impl->voice->SetFrequencyRatio(pitch);
}
