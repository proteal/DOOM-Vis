//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibMaterial::Impl
{
	friend class LibMaterial;
public:
	Impl(D3dObjs& _d3d, const DataFile& file)
		:d3d(_d3d)
	{
		uint flags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR |
			D3D10_SHADER_OPTIMIZATION_LEVEL3;
#ifdef _DEBUG
		flags |= D3D10_SHADER_DEBUG;
#endif

		Compile(file, vsCode, flags, "VS", "vs_4_0");

		d3d.device->CreateVertexShader(vsCode->GetBufferPointer(), vsCode->GetBufferSize(), nullptr, vs.ToCreate());
		if (!vs)
		{
			THROW("CreateVertexShader failed.");
		}

		//if (havePS)
		{
			Compile(file, psCode, flags, "PS", "ps_4_0");

			d3d.device->CreatePixelShader(psCode->GetBufferPointer(), psCode->GetBufferSize(), nullptr, ps.ToCreate());
			if (!ps)
			{
				THROW("CreatePixelShader failed.");
			}
		}
	}

	void operator =(const Impl&) = delete;

	const ComPtr<ID3D10Blob>& GetVertShaderCode() { return vsCode; }

private:
	void Compile(const DataFile& file, ComPtr<ID3D10Blob>& code, uint flags, const char* func,
		const char* target)
	{
		D3dMaterialInclude include;
		ComPtr<ID3D10Blob> errorMsgs;

		D3DCompile(file.GetData(), file.GetSize(), nullptr, nullptr, &include, func, target, flags, 0,
			code.ToCreate(), errorMsgs.ToCreate());

		if (errorMsgs)
		{
			THROW(static_cast<char*>(errorMsgs->GetBufferPointer()));
		}

		if (!code)
		{
			THROW("D3DCompile failed.");
		}
	}

	D3dObjs& d3d;
	ComPtr<ID3D10Blob> vsCode;
	ComPtr<ID3D10Blob> psCode;
	ComPtr<ID3D11VertexShader> vs;
	ComPtr<ID3D11PixelShader> ps;
};

inline void LibMaterial::BindMaterial()
{
	impl->d3d.context->VSSetShader(impl->vs.Ptr(), nullptr, 0);
	impl->d3d.context->GSSetShader(nullptr, nullptr, 0);
	impl->d3d.context->PSSetShader(impl->ps.Ptr(), nullptr, 0);
}

inline unique_ptr<LibMaterialReflect> LibMaterial::MakeReflect()
{
	return make_unique<LibMaterialReflect>(make_unique<LibMaterialReflect::Impl>(impl->vsCode, impl->psCode));
}
