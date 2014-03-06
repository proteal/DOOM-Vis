//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibDepthStencil::Impl
{
	friend class LibDepthStencil;
public:
	explicit Impl(D3dObjs& _d3d, uint width, uint height)
		:d3d(_d3d)
	{
		DXGI_FORMAT texFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT dsFormat = DXGI_FORMAT_UNKNOWN;
		/*switch (format)
		{
		case DSFormat::d16:
		texFormat = DXGI_FORMAT_R16_TYPELESS;
		dsFormat = DXGI_FORMAT_D16_UNORM;
		break;
		case DSFormat::d24s8:*/
		texFormat = DXGI_FORMAT_R24G8_TYPELESS;
		dsFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		/*break;
		case DSFormat::d32:
		texFormat = DXGI_FORMAT_R32_TYPELESS;
		dsFormat = DXGI_FORMAT_D32_FLOAT;
		break;
		}*/

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = width; texDesc.Height = height;
		texDesc.MipLevels = 1; texDesc.ArraySize = 1;
		texDesc.Format = texFormat;
		texDesc.SampleDesc.Count = 1; texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		/*if (shaderResource)
		{
		texDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}*/
		texDesc.CPUAccessFlags = 0; texDesc.MiscFlags = 0;

		d3d.device->CreateTexture2D(&texDesc, nullptr, texture.ToCreate());
		if (!texture)
		{
			THROW("CreateTexture2D failed.");
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
		dsDesc.Format = dsFormat;
		dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsDesc.Flags = 0;
		dsDesc.Texture2D.MipSlice = 0;

		d3d.device->CreateDepthStencilView(texture.Ptr(), &dsDesc, view.ToCreate());
		if (!view)
		{
			THROW("CreateDepthStencilView failed.");
		}
	}

	const ComPtr<ID3D11DepthStencilView>& GetView()
	{
		return view;
	}

	const ComPtr<ID3D11Texture2D>& GetTexture()
	{
		return texture;
	}

	void operator =(const Impl&) = delete;

private:
	D3dObjs& d3d;
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11DepthStencilView> view;
};

inline void LibDepthStencil::ClearDepthStencil()
{
	assert(impl->view);
	impl->d3d.context->ClearDepthStencilView(impl->view.Ptr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
