//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class ConstBufferMember
{
public:
	explicit ConstBufferMember(const ConstBufferMemberInfo& _info)
		:info(_info)
	{
		uint size = RendererDefs::CONST_BUFFER_MEMBER_SIZE(info.type);

		if (info.numElements > 0)
		{
			size *= info.numElements;
		}
		data.resize(size);
	}

	void SetMemberData(const vector<ubyte>& _data)
	{
		assert(data.size() == _data.size());
		data = _data;
	}

	void CopyToBuffer(ubyte* buffer, uint bufferSize)
	{
		stdext::checked_array_iterator<ubyte*> iter(buffer, bufferSize, info.offset);
		copy(data.begin(), data.end(), iter);
	}

	const ConstBufferMemberInfo& GetInfo() const
	{
		return info;
	}

	uint GetSize() const
	{
		return static_cast<uint>(data.size());
	}

private:
	vector<ubyte> data;
	ConstBufferMemberInfo info;
};
