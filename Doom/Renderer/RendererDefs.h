//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

enum class ConstBufferMemberType
{
	FLOAT, FLOAT2, FLOAT3, FLOAT4,
	FLOAT3X2, FLOAT3X3, FLOAT4X4
};

struct ConstBufferMemberInfo
{
	wstring name;
	uint offset;
	uint numElements;
	ConstBufferMemberType type;
};

struct ConstBufferShaderIndex
{
	static const int NULL_INDEX = -1;

	int vsIndex;
	int psIndex;
};

struct VertType
{
	enum Bit
	{
		POS = 1 << 0,
		NORMAL = 1 << 1,
		TEX_COORD_0 = 1 << 2,
		TEX_COORD_1 = 1 << 3,
		TANGENT = 1 << 4,
		BITANGENT = 1 << 5,
		TAN_BITAN = 1 << 6,
		NUM_TYPES = 7
	};
};

typedef uint VertTypes;

enum class IndexType
{
	UINT16,
	UINT32
};

enum class PrimType
{
	POINT_LIST,
	LINE_LIST,
	LINE_STRIP,
	TRIANGLE_LIST,
	TRIANGLE_STRIP,
	LINE_LIST_ADJ,
	LINE_STRIP_ADJ,
	TRIANGLE_LIST_ADJ,
	TRIANGLE_STRIP_ADJ
};

enum class TextureFormat
{
	R8_G8_B8_A8_UNORM,
	R8_G8_B8_A8_UNORM_SRGB
};

struct TextureSlot
{
	wstring name;
	uint index;
};

enum class RasterStateType
{
	NORMAL,
	TWO_SIDE,
	TWO_SIDE_WIREFRAME,
	NUM
};

enum class BlendStateType
{
	NORMAL,
	ALPHA_BLEND,
	NUM
};

enum class DepthStateType
{
	NORMAL,
	NO_WRITE,
	NUM
};

struct ViewportDims
{
	uint width;
	uint height;
	uint leftX;
	uint rightX;
	uint y;
};

struct DoomSeg
{
	XMFLOAT2 v1;
	XMFLOAT2 v2;
	float floorZ;
	float ceilingZ;
};

struct DoomBspNode
{
	XMFLOAT2 v1;
	XMFLOAT2 v2;
	bool twoSide;
};

struct RendererDefs
{
	static const uint DOOM_WIDTH = 320;
	static const uint DOOM_HEIGHT = 200;
	static const uint FRAME_PIXELS = DOOM_WIDTH * DOOM_HEIGHT;

	static const uint RGB_COMPONENTS = 3;
	static const uint RGBA_COMPONENTS = 4;

	static float DOOM_PIXEL_ASPECT()
	{
		return 5.0f / 6.0f;
	}
	static float DOOM_DISPLAY_WIDTH()
	{
		return DOOM_PIXEL_ASPECT() * DOOM_WIDTH;
	}
	static float TOTAL_WIDTH()
	{
		return DOOM_DISPLAY_WIDTH() * 2.0f;
	}

	static uint VERT_SIZE(VertType::Bit bit)
	{
		switch (bit)
		{
		case VertType::POS: return 3 * sizeof(float);
		case VertType::NORMAL: return 3 * sizeof(float);
		case VertType::TEX_COORD_0:
		case VertType::TEX_COORD_1: return 2 * sizeof(float);
		case VertType::TANGENT: return 3 * sizeof(float);
		case VertType::BITANGENT: return 3 * sizeof(float);
		case VertType::TAN_BITAN: return 4 * sizeof(float);
		}

		THROW("Unknown vert type.");
#ifdef _DEBUG
		return 0;
#endif
	}

	static uint TEXEL_SIZE(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R8_G8_B8_A8_UNORM:
		case TextureFormat::R8_G8_B8_A8_UNORM_SRGB: return 4;
		}

		THROW("Unknown texture format.");
		return 0;
	}

	static uint CONST_BUFFER_MEMBER_SIZE(ConstBufferMemberType type)
	{
		switch (type)
		{
		case ConstBufferMemberType::FLOAT: return sizeof(float);
		case ConstBufferMemberType::FLOAT2: return 2 * sizeof(float);
		case ConstBufferMemberType::FLOAT3: return 3 * sizeof(float);
		case ConstBufferMemberType::FLOAT4: return 4 * sizeof(float);
		case ConstBufferMemberType::FLOAT3X2: return 4 * 2 * sizeof(float);
		case ConstBufferMemberType::FLOAT3X3: return 4 * 3 * sizeof(float);
		case ConstBufferMemberType::FLOAT4X4: return 4 * 4 * sizeof(float);
		}

		THROW("Unknown member type.");
#ifdef _DEBUG
		return 0;
#endif
	}
};

using ColorRgb = array<ubyte, RendererDefs::RGB_COMPONENTS>;
using ColorRgba = array<ubyte, RendererDefs::RGBA_COMPONENTS>;
using ColorRgbaF = array<float, RendererDefs::RGBA_COMPONENTS>;

struct FrameDataBuffer
{
	array<ColorRgb, RendererDefs::FRAME_PIXELS> pixels;
	vector<DoomSeg> segs;
	XMFLOAT3 viewPos;
	XMFLOAT3 viewDir;
	XMFLOAT3 playerPos;
	float playerAngle;
	vector<DoomBspNode> nodes;
};
