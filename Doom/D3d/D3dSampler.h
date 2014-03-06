//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibSampler::Impl
{
	friend class LibSampler;
public:
	Impl(D3dObjs& _d3d, bool addressWrap, bool comparison, bool filtered)
		:d3d(_d3d)
	{
		D3D11_SAMPLER_DESC desc;
		if (comparison)
		{
			desc.Filter = filtered ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		}
		else
		{
			desc.Filter = filtered ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		}
		desc.AddressU = desc.AddressV = desc.AddressW = (addressWrap ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_CLAMP);
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 16;
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0.0f;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;

		d3d.device->CreateSamplerState(&desc, state.ToCreate());
		if (!state)
		{
			THROW("CreateSamplerState failed.");
		}
	}

	void operator =(const Impl&) = delete;

private:
	D3dObjs& d3d;
	ComPtr<ID3D11SamplerState> state;
};

inline void LibSampler::BindSampler(uint slot)
{
	ID3D11SamplerState* states[] = { impl->state.Ptr() };
	impl->d3d.context->PSSetSamplers(slot, 1, states);
}
