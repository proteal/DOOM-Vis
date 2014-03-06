//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class App
{
public:
	explicit App(unique_ptr<LibMidi>& libMidi, unique_ptr<LibAudio>& libAudio, unique_ptr<LibInput>& libInput, 
		unique_ptr<LibRenderer>& libRenderer)
		:renderer(libRenderer)
		, audio(libMidi, libAudio)
		, input(libInput)
		, doom(renderer.GetFrameData(), input.GetInputClient(), audio)
	{
	}

	void RunApp()
	{
		DoomExecScope doomExec(doom);

		while (Global::Get().GetSys().BeginFrame())
		{
			input.UpdateInput();
			Global::Get().CheckThreadException();
			renderer.DrawFrame();

			if (doom.GameEnded())
			{
				break;
			}
		}
	}

	void OnResize()
	{
		renderer.OnResize();
	}

private:
	Renderer renderer;
	Audio audio;
	Input input;
	DoomInstance doom;
};
