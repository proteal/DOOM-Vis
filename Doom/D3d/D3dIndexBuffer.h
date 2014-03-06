//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibIndexBuffer::Impl
{
	friend class LibIndexBuffer;
public:
	explicit Impl(D3dObjs& _d3d, uint bytes, const ubyte* data, IndexType indexSize)
		:d3d(_d3d)
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = bytes;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
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
			THROW("CreateBuffer failed.");
		}

		if (indexSize == IndexType::UINT32)
		{
			format = DXGI_FORMAT_R32_UINT;
		}
		else if (indexSize == IndexType::UINT16)
		{
			format = DXGI_FORMAT_R16_UINT;
		}
	}

	void operator =(const Impl&) = delete;

private:
	D3dObjs& d3d;
	ComPtr<ID3D11Buffer> buffer;
	DXGI_FORMAT format;
};

inline void LibIndexBuffer::BindIndices()
{
	assert(impl->buffer);
	impl->d3d.context->IASetIndexBuffer(impl->buffer.Ptr(), impl->format, 0);
}
