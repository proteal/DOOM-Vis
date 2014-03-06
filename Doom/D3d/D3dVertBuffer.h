//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibVertBuffer::Impl
{
	friend class LibVertBuffer;
public:
	Impl(D3dObjs& _d3d, uint bytes, const ubyte* data, uint _stride)
		:d3d(_d3d)
		, stride(_stride)
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = bytes;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = data;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		d3d.device->CreateBuffer(&desc, &initData, buffer.ToCreate());
		if (!buffer)
		{
			THROW("CreateBuffer failed");
		}
	}

	void operator =(const Impl&) = delete;

private:
	D3dObjs& d3d;
	ComPtr<ID3D11Buffer> buffer;
	uint stride;
};

inline void LibVertBuffer::BindVertBuffer()
{
	uint offset = 0;
	ID3D11Buffer* buffers[] = { impl->buffer.Ptr() };
	impl->d3d.context->IASetVertexBuffers(0, 1, buffers, &impl->stride, &offset);
}
