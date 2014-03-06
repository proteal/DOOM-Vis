//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibIndexBuffer)
void BindIndices();
};

class IndexBuffer
{
public:
	explicit IndexBuffer(uint bytes, const ubyte* data, IndexType indexSize)
	{
		libBuffer = RenderGlobal::Get().GetLib().MakeIndexBuffer(bytes, data, indexSize);
	}

	void BindIndices()
	{
		assert(libBuffer);
		libBuffer->BindIndices();
	}

private:
	unique_ptr<LibIndexBuffer> libBuffer;
};
