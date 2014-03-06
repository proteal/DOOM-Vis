//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class VertDescs
{
public:
	explicit VertDescs()
	{
	}

	void operator =(const VertDescs&) = delete;

	const shared_ptr<VertDesc>& GetDesc(VertTypes types)
	{
		return MapGetOrCreate(descs, types, [this, types]()
		{
			return make_shared<VertDesc>(types);
		});
	}

private:
	unordered_map<VertTypes, shared_ptr<VertDesc>> descs;
};
