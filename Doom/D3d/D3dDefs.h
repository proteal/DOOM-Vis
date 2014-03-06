//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

struct D3dObjs
{
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
};

struct D3dDefs
{
	static const char* GetSemantic(VertType::Bit bit, uint& semanticIndex)
	{
		semanticIndex = 0;
		switch (bit)
		{
		case VertType::POS: return "POSITION";
		case VertType::NORMAL: return "NORMAL";
		case VertType::TEX_COORD_0: return "TEXCOORD";
		case VertType::TEX_COORD_1: semanticIndex = 1; return "TEXCOORD";
		case VertType::TANGENT: return "TANGENT";
		case VertType::BITANGENT: return "BITANGENT";
		case VertType::TAN_BITAN: return "TAN_BITAN";
		}

		THROW("Unknown semantic.");
#ifdef _DEBUG
		return "";
#endif
	}

	static void GetElementInfo(VertType::Bit bit, const char*& semantic, uint& semanticIndex,
		DXGI_FORMAT& format)
	{
		semantic = GetSemantic(bit, semanticIndex);
		switch (bit)
		{
		case VertType::POS: format = DXGI_FORMAT_R32G32B32_FLOAT; return;
		case VertType::NORMAL: format = DXGI_FORMAT_R32G32B32_FLOAT; return;
		case VertType::TEX_COORD_0:
		case VertType::TEX_COORD_1: format = DXGI_FORMAT_R32G32_FLOAT; return;
		case VertType::TANGENT: format = DXGI_FORMAT_R32G32B32_FLOAT; return;
		case VertType::BITANGENT: format = DXGI_FORMAT_R32G32B32_FLOAT; return;
		case VertType::TAN_BITAN: format = DXGI_FORMAT_R32G32B32A32_FLOAT; return;
		}

		THROW("Unknown VertType.");
	}

	static D3D11_PRIMITIVE_TOPOLOGY GetTopology(PrimType type)
	{
		switch (type)
		{
		case PrimType::POINT_LIST: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		case PrimType::LINE_LIST: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case PrimType::LINE_STRIP: return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case PrimType::TRIANGLE_LIST: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case PrimType::TRIANGLE_STRIP: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case PrimType::LINE_LIST_ADJ: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
		case PrimType::LINE_STRIP_ADJ: return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
		case PrimType::TRIANGLE_LIST_ADJ: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
		case PrimType::TRIANGLE_STRIP_ADJ: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
		}

		THROW("Uknown topology.");
#ifdef _DEBUG
		return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
#endif
	}

	static DXGI_FORMAT GetDxgiFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R8_G8_B8_A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case TextureFormat::R8_G8_B8_A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		}

		THROW("Unknown texture format.");
#ifdef _DEBUG
		return DXGI_FORMAT_UNKNOWN;
#endif
	}

	static ConstBufferMemberType GetConstBufferMemberType(const D3D11_SHADER_TYPE_DESC& desc)
	{
		switch (desc.Class)
		{
		case D3D10_SVC_SCALAR:
		{
								 switch (desc.Type)
								 {
								 case D3D10_SVT_FLOAT: return ConstBufferMemberType::FLOAT;
								 }
		}
		case D3D10_SVC_VECTOR:
		{
								 switch (desc.Type)
								 {
								 case D3D10_SVT_FLOAT:
								 {
														 if (desc.Elements > 1)
														 {
															 return ConstBufferMemberType::FLOAT4;
														 }
														 switch (desc.Columns)
														 {
														 case 2: return ConstBufferMemberType::FLOAT2;
														 case 3: return ConstBufferMemberType::FLOAT3;
														 case 4: return ConstBufferMemberType::FLOAT4;
														 }
								 }
								 }
		}
		case D3D10_SVC_MATRIX_COLUMNS:
		{
										 switch (desc.Type)
										 {
										 case D3D10_SVT_FLOAT:
										 {
																 switch (desc.Rows)
																 {
																 case 3:
																 {
																		   switch (desc.Columns)
																		   {
																		   case 2: return ConstBufferMemberType::FLOAT3X2;
																		   case 3: return ConstBufferMemberType::FLOAT3X3;
																		   }
																 }
																 case 4:
																 {
																		   switch (desc.Columns)
																		   {
																		   case 4: return ConstBufferMemberType::FLOAT4X4;
																		   }
																 }
																 }
										 }
										 }
		}
		}

		THROW("Unknown shader var type.");
#ifdef _DEBUG
		return ConstBufferMemberType::FLOAT;
#endif
	}
};
