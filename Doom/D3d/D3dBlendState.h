//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibBlendState::Impl
{
	friend class LibBlendState;
public:
	explicit Impl(D3dObjs& _d3d, const bool alphaBlend)
		:d3d(_d3d)
	{
		D3D11_BLEND_DESC desc;
		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;

		D3D11_RENDER_TARGET_BLEND_DESC& rtDesc = desc.RenderTarget[0];
		if (alphaBlend)
		{
			rtDesc.BlendEnable = TRUE;
			rtDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			rtDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		}
		else
		{
			rtDesc.BlendEnable = FALSE;
			rtDesc.DestBlend = D3D11_BLEND_ZERO;
			rtDesc.SrcBlend = D3D11_BLEND_ONE;
		}

		rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		d3d.device->CreateBlendState(&desc, state.ToCreate());
		if (!state)
		{
			THROW("CreateBlendState failed.");
		}
	}

	void operator =(const Impl&) = delete;

private:
	D3dObjs& d3d;
	ComPtr<ID3D11BlendState> state;
};

inline void LibBlendState::BindBlendState()
{
	assert(impl->state);

	FLOAT blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	impl->d3d.context->OMSetBlendState(impl->state.Ptr(), blendFactor, 0xffffffff);
}
