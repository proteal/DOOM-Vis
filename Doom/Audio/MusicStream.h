//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class MusicStream final
{
public:
	explicit MusicStream(const unique_ptr<LibAudio>& libAudio, unique_ptr<LibMidi>& _libMidi)
		:libMidi(move(_libMidi))
		, streamThread()
		, buffers()
		, voice(libAudio, buffers)
	{
	}

	void StartSong(vector<ubyte>& _midiData, bool loop)
	{
		StopSong();
		midiData = move(_midiData);

		auto threadEntry = [this, loop](const atomic_bool& endThread)
		{
			StreamThread(endThread, loop);
		};

		streamThread.StartThread(threadEntry, nullptr);
	}

	void StopSong()
	{
		streamThread.EndThread();
	}

	void SetMusicVolume(float volume)
	{
		voice.SetVolume(volume);
	}

private:
	static const uint BUFFER_SAMPLES = AudioDefs::SAMPLE_RATE / 5;

	void StreamThread(const atomic_bool& endThread, bool loop)
	{
		MidiTrack track(libMidi, midiData);
		midiData.clear();

		bool streaming = true;
		while (!endThread && streaming)
		{
			vector<StreamSample> samples(BUFFER_SAMPLES);
			int samplesWritten = track.WriteSamples(samples);

			if (samplesWritten < BUFFER_SAMPLES)
			{
				if (loop)
				{
					track.ResetTrack();
				}
				else
				{
					streaming = false;
				}

				if (samplesWritten == 0)
				{
					continue;
				}
				samples.resize(samplesWritten);
			}

			voice.StreamBuffer(samples);
		}

		voice.SourceStop();
	}

	unique_ptr<LibMidi> libMidi;
	vector<ubyte> midiData;
	StreamBuffers buffers;
	SourceVoiceMusic voice;
	Thread streamThread;
};
