//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class TextureWriter
{
public:
	explicit TextureWriter(const shared_ptr<Texture>& _texture)
		:texture(_texture)
		, writePtr(nullptr)
#ifdef _DEBUG
		, writeEndPtr(nullptr)
#endif
	{

	}

	void BeginWrite()
	{
		assert(writePtr == nullptr);
		assert(writeEndPtr == nullptr);
		assert(texture);

		const auto& libTexture = texture->GetLibTexture();
		assert(libTexture);

		writePtr = libTexture->MapTexture();

#ifdef _DEBUG
		uint width = texture->GetWidth();
		uint height = texture->GetHeight();

		TextureFormat format = texture->GetFormat();
		uint texelSize = RendererDefs::TEXEL_SIZE(format);

		writeEndPtr = writePtr + width*height*texelSize;
#endif
	}

	template<class Val>
	void WriteVal(const Val& val)
	{
		*reinterpret_cast<Val*>(writePtr) = val;
		writePtr += sizeof(Val);

		assert(writePtr <= writeEndPtr);
	}

	void EndWrite()
	{
		assert(writePtr == writeEndPtr);
		assert(texture);

		const auto& libTexture = texture->GetLibTexture();
		assert(libTexture);
		libTexture->UnmapTexture();

		writePtr = nullptr;
#ifdef _DEBUG
		writeEndPtr = nullptr;
#endif
	}

private:
	ubyte* writePtr;
#ifdef _DEBUG
	ubyte* writeEndPtr;
#endif
	shared_ptr<Texture> texture;
};
