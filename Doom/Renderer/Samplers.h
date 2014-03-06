//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class Samplers
{
public:
	explicit Samplers()
	{
		samplers[L"pointSampler"] = make_shared<Sampler>(true, false, false);
		samplers[L"filterSampler"] = make_shared<Sampler>(false, false, true);
	}

	const shared_ptr<Sampler>& GetSampler(const wstring& name)
	{
		const shared_ptr<Sampler>& sampler = samplers[name];
		if (!sampler)
		{
			THROW("Unknown sampler.");
		}
		return sampler;
	}

private:
	unordered_map<wstring, shared_ptr<Sampler>> samplers;
};
