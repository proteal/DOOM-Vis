//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibVertDesc)
};

class VertDesc
{
public:
	explicit VertDesc(VertTypes types)
		:size(0)
	{
		vector<VertType::Bit> bits;
		vector<uint> offsets;

		for (uint i = 0; i < VertType::NUM_TYPES; ++i)
		{
			VertType::Bit bit = static_cast<VertType::Bit>(1 << i);

			if (bit & types)
			{
				bits.push_back(bit);

				offsets.push_back(size);
				size += RendererDefs::VERT_SIZE(bit);
			}
		}

		libDesc = RenderGlobal::Get().GetLib().MakeVertDesc(bits, offsets);
	}

	void BindVertDesc(const shared_ptr<Material>& material)
	{
		assert(material);

		VertTypes types = material->GetInputTypes();

		const shared_ptr<VertLayout>& layout = MapGetOrCreate(layouts, types, [this, &material]()
		{
			return make_shared<VertLayout>(libDesc, material);
		});
		layout->BindLayout();
	}

	uint GetSize() const { return size; }

private:
	unique_ptr<LibVertDesc> libDesc;
	uint size;
	unordered_map<VertTypes, shared_ptr<VertLayout>> layouts;
};
