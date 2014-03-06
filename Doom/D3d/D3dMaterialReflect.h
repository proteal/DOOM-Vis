//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibMaterialReflect::Impl
{
	friend class LibMaterialReflect;
public:
	explicit Impl(const ComPtr<ID3D10Blob>& vsCode, const ComPtr<ID3D10Blob>& psCode)
	{
		assert(vsCode);
		assert(psCode);

		InitReflect(vsCode, vsReflect, vsDesc);
		InitReflect(psCode, psReflect, psDesc);
	}

private:
	void InitReflect(const ComPtr<ID3D10Blob>& code, ComPtr<ID3D11ShaderReflection>& reflect,
		D3D11_SHADER_DESC& desc)
	{
		D3DReflect(code->GetBufferPointer(), code->GetBufferSize(), IID_ID3D11ShaderReflection,
			reinterpret_cast<void**>(reflect.ToCreate()));

		if (!reflect)
		{
			THROW("D3DReflect failed.");
		}

		reflect->GetDesc(&desc);
	}

	void ShaderFindConstBuffers(const ComPtr<ID3D11ShaderReflection>& reflect, const D3D11_SHADER_DESC& desc,
		unordered_map<wstring, ConstBufferShaderIndex>& buffers, bool isVs)
	{
		assert(reflect);

		for (uint i = 0; i < desc.ConstantBuffers; ++i)
		{
			ID3D11ShaderReflectionConstantBuffer* reflectBuffer = reflect->GetConstantBufferByIndex(i);
			assert(reflectBuffer);

			D3D11_SHADER_BUFFER_DESC bufferDesc;
			reflectBuffer->GetDesc(&bufferDesc);

			wstring bufferName = StrA_ToW(bufferDesc.Name);
			if (buffers.find(bufferName) == buffers.end())
			{
				buffers[bufferName] = { ConstBufferShaderIndex::NULL_INDEX, ConstBufferShaderIndex::NULL_INDEX };
			}

			ConstBufferShaderIndex& buffer = buffers[bufferName];
			int& index = isVs ? buffer.vsIndex : buffer.psIndex;
			index = i;
		}
	}

	void ShaderFindConstBufferInfo(const ComPtr<ID3D11ShaderReflection>& reflect, int index,
		vector<ConstBufferMemberInfo>& infos, uint& size)
	{
		assert(reflect);

		ID3D11ShaderReflectionConstantBuffer* reflectBuffer = reflect->GetConstantBufferByIndex(index);
		assert(reflectBuffer);

		D3D11_SHADER_BUFFER_DESC bufferDesc;
		reflectBuffer->GetDesc(&bufferDesc);

		infos.resize(bufferDesc.Variables);
		for (uint j = 0; j < bufferDesc.Variables; j++)
		{
			ID3D11ShaderReflectionVariable* var = reflectBuffer->GetVariableByIndex(j);
			assert(var);

			D3D11_SHADER_VARIABLE_DESC varDesc;
			var->GetDesc(&varDesc);

			ID3D11ShaderReflectionType* type = var->GetType();
			assert(type);

			D3D11_SHADER_TYPE_DESC typeDesc;
			type->GetDesc(&typeDesc);

			ConstBufferMemberInfo& info = infos[j];
			info.name = StrA_ToW(varDesc.Name);
			info.type = D3dDefs::GetConstBufferMemberType(typeDesc);

			info.numElements = typeDesc.Elements;
			info.offset = varDesc.StartOffset;
		}

		size = bufferDesc.Size;
	}

	ComPtr<ID3D11ShaderReflection> vsReflect;
	D3D11_SHADER_DESC vsDesc;
	ComPtr<ID3D11ShaderReflection> psReflect;
	D3D11_SHADER_DESC psDesc;
};

inline VertTypes LibMaterialReflect::FindInputs()
{
	assert(impl->vsReflect);
	VertTypes inputTypes = 0;

	for (uint i = 0; i < impl->vsDesc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		impl->vsReflect->GetInputParameterDesc(i, &paramDesc);

		for (uint j = 0; j < VertType::NUM_TYPES; ++j)
		{
			VertType::Bit bit = static_cast<VertType::Bit>(1 << j);
			uint semanticIndex;
			const char* semantic = D3dDefs::GetSemantic(bit, semanticIndex);

			if (strcmp(paramDesc.SemanticName, semantic) == 0 && (semanticIndex == paramDesc.SemanticIndex))
			{
				inputTypes |= bit;
				break;
			}
		}
	}

	return inputTypes;
}

inline void LibMaterialReflect::FindTextures(vector<TextureSlot>& textureSlots, vector<wstring>& samplers)
{
	assert(impl->psReflect);

	for (uint i = 0; i < impl->psDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC resource;
		impl->psReflect->GetResourceBindingDesc(i, &resource);

		if (resource.Type == D3D10_SIT_TEXTURE)
		{
			TextureSlot texture;
			texture.index = resource.BindPoint;
			texture.name = StrA_ToW(resource.Name);

			textureSlots.push_back(texture);
		}
		else if (resource.Type == D3D10_SIT_SAMPLER)
		{
			samplers.push_back(StrA_ToW(resource.Name));
		}
	}
}

inline void LibMaterialReflect::FindConstBuffers(unordered_map<wstring, ConstBufferShaderIndex>& buffers)
{
	impl->ShaderFindConstBuffers(impl->vsReflect, impl->vsDesc, buffers, true);
	impl->ShaderFindConstBuffers(impl->psReflect, impl->psDesc, buffers, false);
}

inline void LibMaterialReflect::FindConstBufferInfo(const ConstBufferShaderIndex& index, vector<ConstBufferMemberInfo>& infos,
	uint& size)
{
	if (index.vsIndex != ConstBufferShaderIndex::NULL_INDEX)
	{
		impl->ShaderFindConstBufferInfo(impl->vsReflect, index.vsIndex, infos, size);
	}
	else
	{
		assert(index.psIndex != ConstBufferShaderIndex::NULL_INDEX);
		impl->ShaderFindConstBufferInfo(impl->psReflect, index.psIndex, infos, size);
	}
}
