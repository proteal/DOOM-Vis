//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibVertLayout::Impl
{
	friend class LibVertLayout;
public:
	Impl(D3dObjs& _d3d, const unique_ptr<LibVertDesc>& vertDesc, const unique_ptr<LibMaterial>& material)
		:d3d(_d3d)
	{
		assert(vertDesc);
		assert(material);
		const ComPtr<ID3D10Blob>& vsCode = material->GetImpl().GetVertShaderCode();
		assert(vsCode);

		const auto& descs = vertDesc->GetImpl().GetDescs();
		d3d.device->CreateInputLayout(descs.data(), static_cast<UINT>(descs.size()), vsCode->GetBufferPointer(), vsCode->GetBufferSize(),
			inputLayout.ToCreate());

		if (!inputLayout)
		{
			THROW("CreateInputLayout failed.");
		}
	}

	void operator =(const Impl&) = delete;

private:
	D3dObjs& d3d;
	ComPtr<ID3D11InputLayout> inputLayout;
};

inline void LibVertLayout::BindLayout()
{
	impl->d3d.context->IASetInputLayout(impl->inputLayout.Ptr());
}
