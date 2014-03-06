//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibDepthState)
void BindDepthState();
};

class DepthState
{
public:
	explicit DepthState(const bool writeDepth)
	{
		libState = RenderGlobal::Get().GetLib().MakeDepthState(writeDepth);
	}

	void BindDepthState()
	{
		assert(libState);
		libState->BindDepthState();
	}

private:
	unique_ptr<LibDepthState> libState;
};
