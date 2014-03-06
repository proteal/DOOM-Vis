//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibRasterState::Impl
{
	friend class LibRasterState;
public:
	explicit Impl(D3dObjs& _d3d, const bool backCull, const bool wireframe)
		:d3d(_d3d)
	{
		D3D11_RASTERIZER_DESC desc;
		desc.FillMode = wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
		desc.CullMode = backCull ? D3D11_CULL_BACK : D3D11_CULL_NONE;
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.ScissorEnable = FALSE;
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = FALSE;

		d3d.device->CreateRasterizerState(&desc, state.ToCreate());
		if (!state)
		{
			THROW("CreateRasterizerState failed.");
		}
	}

	void operator =(const Impl&) = delete;

private:
	D3dObjs& d3d;
	ComPtr<ID3D11RasterizerState> state;
};

inline void LibRasterState::BindRasterState()
{
	assert(impl->state);
	impl->d3d.context->RSSetState(impl->state.Ptr());
}
