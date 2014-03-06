//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibMaterialReflect)
VertTypes FindInputs();
void FindTextures(vector<TextureSlot>& textureSlots, vector<wstring>& samplers);
void FindConstBuffers(unordered_map<wstring, ConstBufferShaderIndex>& buffers);
void FindConstBufferInfo(const ConstBufferShaderIndex& index, vector<ConstBufferMemberInfo>& infos, uint& size);
};

DECL_PIMPL(LibMaterial)
void BindMaterial();
unique_ptr<LibMaterialReflect> MakeReflect();
};

class Material
{
public:
	explicit Material(const wstring& file, Samplers& samplerLib, ConstBuffers& buffersLib)
		:inputTypes(0)
	{
		DataFile dataFile(file);
		libMaterial = RenderGlobal::Get().GetLib().MakeMaterial(dataFile);

		unique_ptr<LibMaterialReflect> reflect = libMaterial->MakeReflect();
		inputTypes = reflect->FindInputs();

		vector<wstring> samplerNames;
		reflect->FindTextures(textureSlots, samplerNames);

		samplers.resize(samplerNames.size());
		for (size_t i = 0; i < samplers.size(); i++)
		{
			const shared_ptr<Sampler>& sampler = samplerLib.GetSampler(samplerNames[i]);
			samplers[i] = sampler;
		}

		unordered_map<wstring, ConstBufferShaderIndex> buffers;
		reflect->FindConstBuffers(buffers);

		cBuffers.resize(buffers.size());
		uint bufferIdx = 0;

		for (auto i = buffers.begin(); i != buffers.end(); i++)
		{
			const wstring& name = i->first;
			const ConstBufferShaderIndex& index = i->second;

			shared_ptr<ConstBuffer> buffer = buffersLib.GetBuffer(name);
			if (!buffer)
			{
				vector<ConstBufferMemberInfo> infos;
				uint bufferSize;

				reflect->FindConstBufferInfo(index, infos, bufferSize);
				buffer = make_shared<ConstBuffer>(bufferSize, infos);

				buffersLib.AddBuffer(name, buffer);
			}

			cBuffers[bufferIdx++] = make_pair(buffer, index);
		}
	}

	void BindMaterial()
	{
		assert(libMaterial);

		for (uint i = 0; i < samplers.size(); ++i)
		{
			shared_ptr<Sampler>& sampler = samplers[i];

			assert(sampler);
			sampler->BindSampler(i);
		}

		for (pair<shared_ptr<ConstBuffer>, ConstBufferShaderIndex>& pair : cBuffers)
		{
			const shared_ptr<ConstBuffer>& buffer = pair.first;

			assert(buffer);
			buffer->BindConstantBuffer(pair.second);
		}

		libMaterial->BindMaterial();
	}

	VertTypes GetInputTypes() const 
	{ 
		return inputTypes; 
	}

	const vector<TextureSlot>& GetTextureSlots() const 
	{ 
		return textureSlots; 
	}

	const unique_ptr<LibMaterial>& GetLibMaterial() 
	{ 
		return libMaterial; 
	}

	const vector<pair<shared_ptr<ConstBuffer>, ConstBufferShaderIndex>>& GetConstBuffers()
	{
		return cBuffers;
	}

private:
	unique_ptr<LibMaterial> libMaterial;
	VertTypes inputTypes;
	vector<TextureSlot> textureSlots;
	vector<shared_ptr<Sampler>> samplers;
	vector<pair<shared_ptr<ConstBuffer>, ConstBufferShaderIndex>> cBuffers;
};
