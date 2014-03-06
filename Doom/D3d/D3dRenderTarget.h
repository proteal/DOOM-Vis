//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibRenderTarget::Impl
{
	friend class LibRenderTarget;
public:
	Impl(D3dObjs& _d3d, ComPtr<IDXGISwapChain>& swapChain, uint& width, uint& height)
		:d3d(_d3d)
	{
		assert(swapChain);
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(texture.ToCreate()));
		if (!texture)
		{
			THROW("GetBuffer failed.");
		}

		d3d.device->CreateRenderTargetView(texture.Ptr(), nullptr, target.ToCreate());
		if (!target)
		{
			THROW("CreateRenderTargetView failed.");
		}

		D3D11_TEXTURE2D_DESC bbDesc;
		texture->GetDesc(&bbDesc);
		width = bbDesc.Width;
		height = bbDesc.Height;
	}

	const ComPtr<ID3D11RenderTargetView>& GetTargetView() { return target; }

	void operator =(const Impl&) = delete;

private:
	D3dObjs& d3d;
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11RenderTargetView> target;
};

inline void LibRenderTarget::ClearTarget(const ColorRgbaF& color)
{
	assert(impl->target);

	impl->d3d.context->ClearRenderTargetView(impl->target.Ptr(), color.data());
}
