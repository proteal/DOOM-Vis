//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class ShaderVar
{
public:
	explicit ShaderVar(const shared_ptr<ConstBufferMember>& _member)
		:member(_member)
	{
		assert(member);
		data.resize(member->GetSize());
	}

	virtual ~ShaderVar()
	{
	}

	void BindVarData()
	{
		assert(member);
		member->SetMemberData(data);
	}

	void SetVarData(const ubyte* begin, const ubyte* end)
	{
		copy(begin, end, data.begin());
	}

	virtual ConstBufferMemberType GetVarType() const = 0;

private:	
	shared_ptr<ConstBufferMember> member;
	vector<ubyte> data;
};

template<ConstBufferMemberType _TYPE, class ValType>
class ShaderVarTemplate : public ShaderVar
{
public:
	using TemplateType = ShaderVarTemplate<_TYPE, ValType>;
	static const ConstBufferMemberType TYPE = _TYPE;

	explicit ShaderVarTemplate(const shared_ptr<ConstBufferMember>& member)
		:ShaderVar(member)
	{
		assert(member);

		if (sizeof(ValType) != member->GetSize())
		{
			THROW("Shader var size mismatch.");
		}
	}

	void SetVarData(const ValType& item)
	{
		const ubyte* itemBytes = reinterpret_cast<const ubyte*>(&item);
		ShaderVar::SetVarData(itemBytes, itemBytes + sizeof(ValType));
	}

	ConstBufferMemberType GetVarType() const final
	{
		return TYPE;
	}
};

class ShaderVarFloat : public ShaderVarTemplate<ConstBufferMemberType::FLOAT, float>
{
public:
	explicit ShaderVarFloat(const shared_ptr<ConstBufferMember>& member)
		:TemplateType(member)
	{
	}
};

class ShaderVarFloat2 : public ShaderVarTemplate<ConstBufferMemberType::FLOAT2, XMFLOAT2>
{
public:
	explicit ShaderVarFloat2(const shared_ptr<ConstBufferMember>& member)
		:TemplateType(member)
	{
	}
};

class ShaderVarFloat3 : public ShaderVarTemplate<ConstBufferMemberType::FLOAT3, XMFLOAT3>
{
public:
	explicit ShaderVarFloat3(const shared_ptr<ConstBufferMember>& member)
		:TemplateType(member)
	{
	}
};

class ShaderVarFloat4 : public ShaderVarTemplate<ConstBufferMemberType::FLOAT4, XMFLOAT4>
{
public:
	explicit ShaderVarFloat4(const shared_ptr<ConstBufferMember>& member)
		:TemplateType(member)
	{
	}
};

class ShaderVarFloat3x2 : public ShaderVarTemplate<ConstBufferMemberType::FLOAT3X2, XMFLOAT4[2]>
{
public:
	explicit ShaderVarFloat3x2(const shared_ptr<ConstBufferMember>& member)
		:TemplateType(member)
	{
	}

	void SetVarData(const XMFLOAT3X3& val)
	{
		XMFLOAT4 vecs[2] = { { val._11, val._21, val._31, 0.0f },
		{ val._12, val._22, val._32, 0.0f } };

		TemplateType::SetVarData(vecs);
	}
};

class ShaderVarFloat3x3 : public ShaderVarTemplate<ConstBufferMemberType::FLOAT3X3, XMFLOAT4X3>
{
public:
	explicit ShaderVarFloat3x3(const shared_ptr<ConstBufferMember>& member)
		:TemplateType(member)
	{
	}

	void SetVarData(const XMFLOAT3X3& val)
	{
		XMFLOAT4X3 val4x3(val._11, val._21, val._31, 0.0f,
			val._12, val._22, val._32, 0.0f,
			val._13, val._23, val._33, 0.0f);

		TemplateType::SetVarData(val4x3);
	}
};

class ShaderVarFloat4x4 : public ShaderVarTemplate<ConstBufferMemberType::FLOAT4X4, XMMATRIX>
{
public:
	explicit ShaderVarFloat4x4(const shared_ptr<ConstBufferMember>& member)
		:TemplateType(member)
	{
	}

	void SetVarData(const XMMATRIX& val)
	{
		XMMATRIX trans = XMMatrixTranspose(val);
		TemplateType::SetVarData(trans);
	}
};

struct ShaderVarFactory
{
	static shared_ptr<ShaderVar> MakeShaderVar(const shared_ptr<ConstBufferMember>& member)
	{
		assert(member);
		const ConstBufferMemberInfo& info = member->GetInfo();

		switch (info.type)
		{
		case ConstBufferMemberType::FLOAT: return make_shared<ShaderVarFloat>(member);
		case ConstBufferMemberType::FLOAT2: return make_shared<ShaderVarFloat2>(member);
		case ConstBufferMemberType::FLOAT3: return make_shared<ShaderVarFloat3>(member);
		case ConstBufferMemberType::FLOAT4: return make_shared<ShaderVarFloat4>(member);
		case ConstBufferMemberType::FLOAT3X2: return make_shared<ShaderVarFloat3x2>(member);
		case ConstBufferMemberType::FLOAT3X3: return make_shared<ShaderVarFloat3x3>(member);
		case ConstBufferMemberType::FLOAT4X4: return make_shared<ShaderVarFloat4x4>(member);
		}

		THROW("MakeShaderVar: unknown type.");
		return nullptr;
	}
};
