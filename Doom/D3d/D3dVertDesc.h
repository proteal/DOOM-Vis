//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibVertDesc::Impl
{
	friend class LibVertDesc;
public:
	Impl(D3dObjs& _d3d, const vector<VertType::Bit>& bits, const vector<uint>& offsets)
		:d3d(_d3d)
	{
		assert(bits.size() == offsets.size());
		elementDescs.resize(bits.size());

		for (uint i = 0; i < bits.size(); ++i)
		{
			D3D11_INPUT_ELEMENT_DESC& desc = elementDescs[i];

			D3dDefs::GetElementInfo(bits[i], desc.SemanticName, desc.SemanticIndex, desc.Format);

			desc.AlignedByteOffset = offsets[i];
			desc.InputSlot = 0;
			desc.InstanceDataStepRate = 0;
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		}
	}

	void operator =(const Impl&) = delete;

	const vector<D3D11_INPUT_ELEMENT_DESC>& GetDescs() const { return elementDescs; }

private:
	D3dObjs& d3d;
	vector<D3D11_INPUT_ELEMENT_DESC> elementDescs;
};
