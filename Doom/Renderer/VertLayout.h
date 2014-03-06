//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibVertLayout)
void BindLayout();
};

class VertLayout
{
public:
	explicit VertLayout(const unique_ptr<LibVertDesc>& libDesc, const shared_ptr<Material>& material)
	{
		assert(material);
		libLayout = RenderGlobal::Get().GetLib().MakeVertLayout(libDesc, material->GetLibMaterial());
	}

	void BindLayout()
	{
		assert(libLayout);

		libLayout->BindLayout();
	}

private:
	unique_ptr<LibVertLayout> libLayout;
};
