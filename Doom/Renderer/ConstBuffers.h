//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class ConstBuffers
{
public:
	explicit ConstBuffers()
	{
	}

	const shared_ptr<ConstBuffer>& GetBuffer(const wstring& name)
	{
		return buffers[name];
	}

	void AddBuffer(const wstring& name, const shared_ptr<ConstBuffer>& buffer)
	{
		assert(buffers[name] == nullptr);
		buffers[name] = buffer;
	}

private:
	unordered_map<wstring, shared_ptr<ConstBuffer>> buffers;
};
