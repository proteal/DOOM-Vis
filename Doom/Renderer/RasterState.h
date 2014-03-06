//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibRasterState)
void BindRasterState();
};

class RasterState
{
public:
	explicit RasterState(const bool backCull, const bool wireframe)
	{
		libState = RenderGlobal::Get().GetLib().MakeRasterState(backCull, wireframe);
	}

	void BindRasterState()
	{
		assert(libState);
		libState->BindRasterState();
	}

private:
	unique_ptr<LibRasterState> libState;
};
