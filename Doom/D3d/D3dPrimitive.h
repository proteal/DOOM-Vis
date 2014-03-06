//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibPrimitive::Impl
{
	friend class LibPrimitive;
public:
	Impl(D3dObjs& _d3d, PrimType type, uint _drawCount, uint _drawOffset)
		:d3d(_d3d)
	{
		topology = D3dDefs::GetTopology(type);

		drawCount = _drawCount;
		drawOffset = _drawOffset;
		baseVertexLocation = 0;
	}

	void operator =(const Impl&) = delete;

private:
	void BindTopology()
	{
		d3d.context->IASetPrimitiveTopology(topology);
	}

	D3dObjs& d3d;
	D3D11_PRIMITIVE_TOPOLOGY topology;
	uint drawCount;
	uint drawOffset;
	int baseVertexLocation;
};

inline void LibPrimitive::DrawNonIndexed()
{
	impl->BindTopology();
	impl->d3d.context->Draw(impl->drawCount, impl->drawOffset);
}

inline void LibPrimitive::DrawIndexed()
{
	impl->BindTopology();
	impl->d3d.context->DrawIndexed(impl->drawCount, impl->drawOffset, impl->baseVertexLocation);
}
