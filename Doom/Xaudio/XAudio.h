//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibAudio::Impl
{
	friend class LibAudio;
public:
	Impl()
		:global()
		, masterVoice(nullptr)
	{
		XAudio2Create(global.xAudio.ToCreate(), 0, XAUDIO2_DEFAULT_PROCESSOR);
		if (!global.xAudio)
		{
			THROW("XAudio2Create failed.");
		}

		/*XAUDIO2_DEBUG_CONFIGURATION dbgConfig;
		dbgConfig.TraceMask = XAUDIO2_LOG_WARNINGS | XAUDIO2_LOG_DETAIL | XAUDIO2_LOG_FUNC_CALLS | XAUDIO2_LOG_TIMING |
		XAUDIO2_LOG_LOCKS | XAUDIO2_LOG_MEMORY | XAUDIO2_LOG_STREAMING;
		dbgConfig.BreakMask = 0;
		dbgConfig.LogThreadID = FALSE;
		dbgConfig.LogFileline = dbgConfig.LogFunctionName = dbgConfig.LogTiming = TRUE;

		xAudio->SetDebugConfiguration(&dbgConfig);*/

		global.xAudio->CreateMasteringVoice(&masterVoice, 2, AudioDefs::SAMPLE_RATE, 0, nullptr, nullptr, AudioCategory_GameEffects);
		if (!masterVoice)
		{
			THROW("CreateMasteringVoice failed.");
		}
	}

	~Impl()
	{
		assert(masterVoice);
		masterVoice->DestroyVoice();
	}

private:
	XaGlobal global;
	IXAudio2MasteringVoice* masterVoice;
};

inline unique_ptr<LibSourceVoice> LibAudio::MakeSourceVoice(int sampleRate, uint sampleBytes, uint channels, StreamBuffers* streamBuffers)
{
	return make_unique<LibSourceVoice>(make_unique<LibSourceVoice::Impl>(impl->global, sampleRate, sampleBytes, channels, streamBuffers));
}
