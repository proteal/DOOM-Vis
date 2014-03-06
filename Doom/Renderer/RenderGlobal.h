//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class RenderGlobal
{
public:
	static RenderGlobal& Get()
	{
		assert(inst);
		return *inst;
	}

	explicit RenderGlobal(unique_ptr<LibRenderer>& _lib)
		:lib(_lib)
	{
		assert(lib);
		inst = this;
	}

	~RenderGlobal()
	{
		inst = nullptr;
	}

	LibRenderer& GetLib()
	{
		assert(lib);
		return *lib;
	}

	void operator =(const RenderGlobal&) = delete;

private:
	unique_ptr<LibRenderer>& lib;
	static RenderGlobal* inst;
};
