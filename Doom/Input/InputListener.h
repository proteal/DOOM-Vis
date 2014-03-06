//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibInputListener)
};

class InputListener : public InputCallbacks
{
public:
	explicit InputListener(const unique_ptr<LibInput>& libInput)
		:InputCallbacks()
	{
		assert(libInput);
		libInputListener = libInput->MakeInputListener(*this);
	}

	InputEvents& GetEvents()
	{
		return events;
	}

	InputState& GetState()
	{
		return state;
	}

private:
	unique_ptr<LibInputListener> libInputListener;
};
