//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibDepthStencil)
void ClearDepthStencil();
};

class DepthStencil
{
public:
	explicit DepthStencil(uint width, uint height)
	{
		libDepth = RenderGlobal::Get().GetLib().MakeDepthStencil(width, height);
	}

	void ClearDepthStencil()
	{
		assert(libDepth);
		libDepth->ClearDepthStencil();
	}

	const unique_ptr<LibDepthStencil>& GetLibDepthStencil()
	{
		return libDepth;
	}

private:
	unique_ptr<LibDepthStencil> libDepth; 
};
