//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibRenderTarget)
void ClearTarget(const ColorRgbaF& color);
};

class RenderTarget
{
public:
	explicit RenderTarget()
	{
		libTarget = RenderGlobal::Get().GetLib().MakeRenderTarget(width, height);
	}

	void ClearTarget(const ColorRgbaF& color)
	{
		assert(libTarget);
		libTarget->ClearTarget(color);
	}

	uint GetWidth() const { return width; }
	uint GetHeight() const { return height; }
	const unique_ptr<LibRenderTarget>& GetLibTarget() { return libTarget; }

private:
	unique_ptr<LibRenderTarget> libTarget;
	uint width;
	uint height;
};
