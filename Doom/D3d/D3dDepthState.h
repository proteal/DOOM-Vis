//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibDepthState::Impl
{
	friend class LibDepthState;
public:
	explicit Impl(D3dObjs& _d3d, const bool writeDepth)
		:d3d(_d3d)
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = writeDepth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = FALSE;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = 0;

		D3D11_DEPTH_STENCILOP_DESC& opDesc = desc.FrontFace;
		opDesc.StencilFunc = D3D11_COMPARISON_ALWAYS;
		opDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		opDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		opDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace = desc.FrontFace;

		d3d.device->CreateDepthStencilState(&desc, state.ToCreate());
		if (!state)
		{
			THROW("CreateDepthStencilState failed.");
		}
	}

	void operator =(const Impl&) = delete;

private:
	D3dObjs& d3d;
	ComPtr<ID3D11DepthStencilState> state;
};

inline void LibDepthState::BindDepthState()
{
	assert(impl->state);

	impl->d3d.context->OMSetDepthStencilState(impl->state.Ptr(), 0);
}
