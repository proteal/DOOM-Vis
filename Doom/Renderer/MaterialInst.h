//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class MaterialInst
{
public:
	explicit MaterialInst(const shared_ptr<Material>& _material)
		:material(_material)
	{
		assert(material);

		const auto& textureSlots = material->GetTextureSlots();
		for (const TextureSlot& slot : textureSlots)
		{
			textureVars[slot.name] = make_shared<ShaderVarTexture>(slot.index);
		}

		const auto& cBuffers = material->GetConstBuffers();
		for (const auto& pair : cBuffers)
		{
			const shared_ptr<ConstBuffer>& buffer = pair.first;
			assert(buffer);
			const vector<shared_ptr<ConstBufferMember>>& members = buffer->GetMembers();

			for (const shared_ptr<ConstBufferMember>& member : members)
			{
				assert(member);
				const ConstBufferMemberInfo& info = member->GetInfo();

				assert(vars[info.name] == nullptr);
				vars[info.name] = ShaderVarFactory::MakeShaderVar(member);
			}
		}
	}

	void BindMaterialInst()
	{
		assert(material);

		for (const auto& pair : vars)
		{
			const shared_ptr<ShaderVar>& var = pair.second;
			assert(var);

			var->BindVarData();
		}

		material->BindMaterial();

		for (const auto& pair : textureVars)
		{
			const shared_ptr<ShaderVarTexture>& var = pair.second;
			assert(var);

			var->BindTextureVar();
		}
	}

	template<class Type>
	shared_ptr<Type> GetShaderVar(const wstring& name)
	{
		const shared_ptr<ShaderVar>& var = vars[name];
		if (!var)
		{
			THROW("Shader var doesn't exist.");
		}

		ConstBufferMemberType requestedType = Type::TYPE;
		if (requestedType != var->GetVarType())
		{
			THROW("Incorrect shader var type.");
		}

		return static_pointer_cast<Type>(var);
	}

	const shared_ptr<ShaderVarTexture>& GetTextureVar(const wstring& name)
	{
		const shared_ptr<ShaderVarTexture>& var = textureVars[name];
		if (!var)
		{
			THROW("Shader texture var doesn't exist.");
		}

		return var;
	}

	const shared_ptr<Material>& GetMaterial() 
	{ 
		return material; 
	}

private:
	shared_ptr<Material> material;
	unordered_map<wstring, shared_ptr<ShaderVar>> vars;
	unordered_map<wstring, shared_ptr<ShaderVarTexture>> textureVars;
};
