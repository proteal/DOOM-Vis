//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class Audio
{
public:
	explicit Audio(unique_ptr<LibMidi>& libMidi, unique_ptr<LibAudio>& _libAudio)
		:libAudio(move(_libAudio))
		, musicStream(libAudio, libMidi)
		, audioThread()
	{
		auto exitFunc = [this]()
		{
			AddCommand([]()
			{
				//Break out of command wait.
			});
		};

		auto entryFunc = [this](const atomic_bool& endThread)
		{
			while (!endThread)
			{
				function<void()> cmd;
				{
					unique_lock<mutex> lock(commandMutex);
					commandVar.wait(lock, [this]()
					{
						return !commands.empty();
					});

					cmd = move(commands.front());
					commands.erase(commands.begin());
				}

				cmd();
			}
		};

		audioThread.StartThread(entryFunc, exitFunc);
	}

	void InitChannels(int numChannels)
	{
		AddCommand([this, numChannels]()
		{
			channels.resize(numChannels);
		});
	}

	void PlaySound(const vector<ubyte>& samples, int sampleRate, int channel, int vol, int sep, int pitch)
	{
		vector<ubyte>& samplesCopy = const_cast<vector<ubyte>&>(samples);
		AddCommand([this, samplesCopy, sampleRate, channel, vol, sep, pitch]() mutable
		{
			if (sampleRate != AudioDefs::SAMPLE_RATE)
			{
#if USE_LOG
				Global::Get().DebugLog("Skipping uncommon sample-rate audio.\n");
#endif
				return;
			}

			unique_ptr<SourceVoiceSfx>& voice = channels[channel];
			if (voice)
			{
				voice->SourceStop();
			}
			else
			{
				voice = make_unique<SourceVoiceSfx>(libAudio, sampleRate);
			}

			voice->SourcePlay(samplesCopy);
			UpdateVolume(channel, vol, sep);

			float pitchF = static_cast<float>(pitch) / AudioDefs::NORMAL_PITCH();
			voice->SetPitch(pitchF);
		});
	}

	void UpdateSound(int channel, int vol, int sep)
	{
		AddCommand([this, channel, vol, sep]()
		{
			UpdateVolume(channel, vol, sep);
		});
	}

	void StopSound(int channel)
	{
		AddCommand([this, channel]()
		{
			unique_ptr<SourceVoiceSfx>& voice = channels[channel];

			if (voice)
			{
				voice->SourceStop();
			}
		});
	}

	void PlaySong(const ubyte* data, int length, bool loop)
	{
		shared_ptr<const vector<ubyte>> musData = make_shared<const vector<ubyte>>(data, data + length);

		AddCommand([this, musData, loop]()
		{
			vector<ubyte> midiData;
			MusConvert convert(musData, midiData);

			musicStream.StartSong(midiData, loop);
		});
	}

	void StopSong()
	{
		AddCommand([this]()
		{
			musicStream.StopSong();
		});
	}

	void SetMusicVolume(int volume)
	{
		AddCommand([this, volume]()
		{
			float volumeF = static_cast<float>(volume) / AudioDefs::MAX_VOL();
			musicStream.SetMusicVolume(volumeF);
		});
	}

private:
	void AddCommand(function<void()> cmd)
	{
		{
			lock_guard<mutex> lock(commandMutex);
			commands.push_back(move(cmd));
		}

		commandVar.notify_one();
	}

	void UpdateVolume(int channel, int vol, int sep)
	{
		unique_ptr<SourceVoiceSfx>& voice = channels[channel];
		if (voice)
		{
			float left = (AudioDefs::MAX_SEP() - sep) / AudioDefs::MAX_SEP();
			float right = static_cast<float>(sep) / AudioDefs::MAX_SEP();
			float volF = static_cast<float>(vol) / AudioDefs::MAX_VOL();
			left *= volF;
			right *= volF;

			voice->SetVolume(left, right);
		}
	}

	unique_ptr<LibAudio> libAudio;
	vector<function<void()>> commands;
	mutex commandMutex;
	condition_variable commandVar;
	vector<unique_ptr<SourceVoiceSfx>> channels;
	MusicStream musicStream;
	Thread audioThread;
};
