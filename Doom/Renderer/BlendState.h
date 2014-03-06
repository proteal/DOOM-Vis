//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibBlendState)
void BindBlendState();
};

class BlendState
{
public:
	explicit BlendState(const bool alphaBlend)
	{
		libState = RenderGlobal::Get().GetLib().MakeBlendState(alphaBlend);
	}

	void BindBlendState()
	{
		assert(libState);
		libState->BindBlendState();
	}

private:
	unique_ptr<LibBlendState> libState;
};
