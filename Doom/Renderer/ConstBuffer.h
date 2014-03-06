//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibConstBuffer)
ubyte* LockConstBuffer();
void UnlockConstBuffer();
void BindToVs(uint index);
void BindToPs(uint index);
};

class ConstBuffer
{
public:
	ConstBuffer(uint bufferSize, const vector<ConstBufferMemberInfo>& infos)
		:size(bufferSize)
	{
		members.resize(infos.size());

		for (uint i = 0; i < infos.size(); i++)
		{
			const ConstBufferMemberInfo& info = infos[i];
			members[i] = make_shared<ConstBufferMember>(info);
		}

		libConstBuffer = RenderGlobal::Get().GetLib().MakeConstBuffer(bufferSize);
	}

	void BindConstantBuffer(const ConstBufferShaderIndex& index)
	{
		assert(libConstBuffer);

		/*for (uint i = 0; i < autoVars.Size(); ++i)
		{
		ShaderVarAuto& var = autoVars.At(i);
		var.UpdateVar();
		var.BindVar();
		}*/

		ubyte* data = libConstBuffer->LockConstBuffer();
		assert(data);

		for (shared_ptr<ConstBufferMember>& member : members)
		{
			assert(member);
			member->CopyToBuffer(data, size);
		}
		libConstBuffer->UnlockConstBuffer();

		if (index.vsIndex != ConstBufferShaderIndex::NULL_INDEX)
		{
			libConstBuffer->BindToVs(index.vsIndex);
		}

		if (index.psIndex != ConstBufferShaderIndex::NULL_INDEX)
		{
			libConstBuffer->BindToPs(index.psIndex);
		}
	}

	const vector<shared_ptr<ConstBufferMember>>& GetMembers()
	{
		return members;
	}

private:
	vector<shared_ptr<ConstBufferMember>> members;
	unique_ptr<LibConstBuffer> libConstBuffer;
	uint size;
};
