//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibTexture)
void BindTexture(uint index);
ubyte* MapTexture();
void UnmapTexture();
};

class Texture
{
public:
	Texture(uint _width, uint _height, TextureFormat _format)
		:width(_width)
		, height(_height)
		, format(_format)
	{
		libTexture = RenderGlobal::Get().GetLib().MakeTexture(width, height, format);
	}

	void BindTexture(uint index)
	{
		assert(libTexture);
		libTexture->BindTexture(index);
	}

	const unique_ptr<LibTexture>& GetLibTexture() { return libTexture; }
	uint GetWidth() const { return width; }
	uint GetHeight() const { return height; }
	TextureFormat GetFormat() const { return format; }

private:
	uint width;
	uint height;
	TextureFormat format;
	unique_ptr<LibTexture> libTexture;
};
