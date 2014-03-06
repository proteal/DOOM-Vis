//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class ShaderVarTexture
{
public:
	explicit ShaderVarTexture(uint _index)
		:index(_index)
	{
	}

	void BindTextureVar()
	{
		assert(texture);
		texture->BindTexture(index);
	}

	void SetTexture(const shared_ptr<Texture>& _texture)
	{
		texture = _texture;
	}

private:
	uint index;
	shared_ptr<Texture> texture;
};
