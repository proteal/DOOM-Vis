//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibConstBuffer::Impl
{
	friend class LibConstBuffer;
public:
	explicit Impl(D3dObjs& _d3d, uint size)
		:d3d(_d3d)
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = size;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		d3d.device->CreateBuffer(&desc, nullptr, buffer.ToCreate());
		if (!buffer)
		{
			THROW("CreateBuffer failed.");
		}
	}

	void operator =(const Impl&) = delete;

private:
	D3dObjs& d3d;
	ComPtr<ID3D11Buffer> buffer;
};

inline ubyte* LibConstBuffer::LockConstBuffer()
{
	assert(impl->buffer);

	D3D11_MAPPED_SUBRESOURCE mapped;
	impl->d3d.context->Map(impl->buffer.Ptr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	return reinterpret_cast<ubyte*>(mapped.pData);
}

inline void LibConstBuffer::UnlockConstBuffer()
{
	impl->d3d.context->Unmap(impl->buffer.Ptr(), 0);
}

inline void LibConstBuffer::BindToVs(uint index)
{
	assert(impl->buffer);

	ID3D11Buffer* buffers[] = { impl->buffer.Ptr() };
	impl->d3d.context->VSSetConstantBuffers(index, 1, buffers);
}

inline void LibConstBuffer::BindToPs(uint index)
{
	assert(impl->buffer);

	ID3D11Buffer* buffers[] = { impl->buffer.Ptr() };
	impl->d3d.context->PSSetConstantBuffers(index, 1, buffers);
}
