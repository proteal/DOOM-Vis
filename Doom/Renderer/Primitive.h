//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibPrimitive)
void DrawNonIndexed();
void DrawIndexed();
};

class Primitive
{
public:
	Primitive(PrimType type, uint drawCount, uint drawOffset, unique_ptr<VertBuffer>& _vertBuffer, unique_ptr<IndexBuffer>& _indBuffer)
		:vertBuffer(move(_vertBuffer))
		, indBuffer(move(_indBuffer))
	{
		libPrimitive = RenderGlobal::Get().GetLib().MakePrimitive(type, drawCount, drawOffset);
	}

	void DrawPrimitive(const unique_ptr<MaterialInst>& overrideMat)
	{
		assert(libPrimitive);
		assert(vertBuffer);
		const shared_ptr<VertDesc>& vertDesc = vertBuffer->GetVertDesc();
		assert(vertDesc);

		const unique_ptr<MaterialInst>& curMatInst = overrideMat ? overrideMat : matInst;
		assert(curMatInst);

		vertBuffer->BindVertBuffer();
		const shared_ptr<Material>& material = curMatInst->GetMaterial();
		vertDesc->BindVertDesc(material);
		curMatInst->BindMaterialInst();

		if (indBuffer)
		{
			indBuffer->BindIndices();
			libPrimitive->DrawIndexed();
		}
		else
		{
			libPrimitive->DrawNonIndexed();
		}
	}

private:
	unique_ptr<MaterialInst> matInst;
	unique_ptr<VertBuffer> vertBuffer;
	unique_ptr<IndexBuffer> indBuffer;
	unique_ptr<LibPrimitive> libPrimitive;
};
