//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class XaVoiceCallback : public IXAudio2VoiceCallback
{
public:
	explicit XaVoiceCallback(StreamBuffers* _streamBuffers)
		:streamBuffers(_streamBuffers)
	{
	}

	void __stdcall OnVoiceProcessingPassStart(UINT32 /*BytesRequired*/) final
	{
	}

	void __stdcall OnVoiceProcessingPassEnd() final
	{
	}

	void __stdcall OnStreamEnd() final
	{
	}

	void __stdcall OnBufferStart(void* /*pBufferContext*/) final
	{
	}

	void __stdcall OnBufferEnd(void* /*pBufferContext*/) final
	{
		assert(streamBuffers);
		streamBuffers->ReleaseBuffer();
	}

	void __stdcall OnLoopEnd(void* /*pBufferContext*/) final
	{
	}

	void __stdcall OnVoiceError(void* /*pBufferContext*/, HRESULT /*Error*/) final
	{
	}

private:
	StreamBuffers* streamBuffers;
};
