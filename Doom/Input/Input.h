//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class InputClient
{
public:
	void SetUsingCursor(bool usingCursor)
	{
		newUsingCursor = usingCursor;
	}

	InputEventsClient& GetEvents()
	{
		return listener.GetEvents();
	}

	InputStateClient& GetState()
	{
		return listener.GetState();
	}

protected:
	explicit InputClient(const unique_ptr<LibInput>& libInput)
		:listener(libInput)
		, prevUsingCursor(true)
	{
		newUsingCursor = false;
	}

	InputListener listener;
	bool prevUsingCursor;
	atomic_bool newUsingCursor;
};

class Input final : private InputClient
{
public:
	explicit Input(unique_ptr<LibInput>& _libInput)
		:InputClient(_libInput)
		, libInput(move(_libInput))
	{
		UpdateInput();
	}

	~Input()
	{
		newUsingCursor = true; //For message box on exception.
		UpdateInput();
	}

	void UpdateInput()
	{
		assert(libInput);

		if (newUsingCursor != prevUsingCursor)
		{
			prevUsingCursor = !prevUsingCursor;
			libInput->EnableCursor(prevUsingCursor);
		}
	}

	InputClient& GetInputClient()
	{
		return *this;
	}

private:
	unique_ptr<LibInput> libInput;
};
