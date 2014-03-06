//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class D3dMaterialInclude : public ID3DInclude
{
public:
	explicit D3dMaterialInclude()
	{
	}

	HRESULT __stdcall Open(D3D_INCLUDE_TYPE /*IncludeType*/, LPCSTR pFileName, LPCVOID /*pParentData*/,
		LPCVOID *ppData, UINT *pBytes) final
	{
		wstring name = StrA_ToW(pFileName);
		file = make_unique<DataFile>(name);

		*ppData = file->GetData();
		*pBytes = static_cast<UINT>(file->GetSize());
		return S_OK;
	}

	HRESULT __stdcall Close(LPCVOID /*pData*/) final
	{
		file = nullptr;
		return S_OK;
	}

private:
	unique_ptr<DataFile> file;
};
