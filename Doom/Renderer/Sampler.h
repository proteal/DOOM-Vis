//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibSampler)
void BindSampler(uint slot);
};

class Sampler
{
public:
	Sampler(bool addressWrap, bool comparison, bool filtered)
	{
		libSampler = RenderGlobal::Get().GetLib().MakeSampler(addressWrap, comparison, filtered);
	}

	void BindSampler(uint slot)
	{
		assert(libSampler);
		libSampler->BindSampler(slot);
	}

private:
	unique_ptr<LibSampler> libSampler;
};
