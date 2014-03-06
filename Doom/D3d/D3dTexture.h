//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibTexture::Impl
{
	friend class LibTexture;
public:
	Impl(D3dObjs& _d3d, uint width, uint height, TextureFormat format)
		:d3d(_d3d)
	{
		DXGI_FORMAT dxFormat = D3dDefs::GetDxgiFormat(format);

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = width; desc.Height = height;
		desc.MipLevels = 1; desc.ArraySize = 1;
		desc.Format = dxFormat;
		desc.SampleDesc.Count = 1; desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		d3d.device->CreateTexture2D(&desc, nullptr, texture.ToCreate());
		if (!texture)
		{
			THROW("CreateTexture2D failed.");
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = desc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MostDetailedMip = 0;
		viewDesc.Texture2D.MipLevels = 1;

		/*if (texDesc.Format == DXGI_FORMAT_R16_TYPELESS)
		{
		desc.Format = DXGI_FORMAT_R16_UNORM;
		}
		else if (texDesc.Format == DXGI_FORMAT_R24G8_TYPELESS)
		{
		desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		}
		else if (texDesc.Format == DXGI_FORMAT_R32_TYPELESS)
		{
		desc.Format = DXGI_FORMAT_R32_FLOAT;
		}*/

		d3d.device->CreateShaderResourceView(texture.Ptr(), &viewDesc, shaderResource.ToCreate());
		if (!shaderResource)
		{
			THROW("CreateShaderResourceView failed.");
		}
	}

	void operator =(const Impl&) = delete;

private:
	D3dObjs& d3d;
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> shaderResource;
};

inline void LibTexture::BindTexture(uint index)
{
	assert(impl->shaderResource);

	ID3D11ShaderResourceView* resources[] = { impl->shaderResource.Ptr() };
	impl->d3d.context->PSSetShaderResources(index, 1, resources);
}

inline ubyte* LibTexture::MapTexture()
{
	assert(impl->texture);

	D3D11_MAPPED_SUBRESOURCE mapped;
	impl->d3d.context->Map(impl->texture.Ptr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	return reinterpret_cast<ubyte*>(mapped.pData);
}

inline void LibTexture::UnmapTexture()
{
	assert(impl->texture);

	impl->d3d.context->Unmap(impl->texture.Ptr(), 0);
}
