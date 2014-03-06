//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibVertBuffer)
void BindVertBuffer();
};

class VertBuffer
{
public:
	VertBuffer(VertDescs& vertDescs, VertTypes elements, uint bytes, const ubyte* data)
	{
		vertDesc = vertDescs.GetDesc(elements);
		uint vertSize = vertDesc->GetSize();

		libBuffer = RenderGlobal::Get().GetLib().MakeVertBuffer(bytes, data, vertSize);
	}

	void BindVertBuffer()
	{
		assert(libBuffer);

		libBuffer->BindVertBuffer();
	}

	const shared_ptr<VertDesc>& GetVertDesc() 
	{ 
		return vertDesc; 
	}

private:
	shared_ptr<VertDesc> vertDesc;
	unique_ptr<LibVertBuffer> libBuffer;
};
