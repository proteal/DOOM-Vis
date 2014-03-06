//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class WallMesh
{
public:
	explicit WallMesh(VertDescs& vertDescs)
	{
		XMFLOAT3 verts[] = { { 0.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } };

		uint bytes = sizeof(verts);
		ubyte* data = reinterpret_cast<ubyte*>(verts);

		unique_ptr<VertBuffer> vertBuffer = make_unique<VertBuffer>(vertDescs, VertType::POS | VertType::NORMAL, bytes, data);
		primitive = make_unique<Primitive>(PrimType::TRIANGLE_STRIP, 4, 0, vertBuffer, unique_ptr<IndexBuffer>());
	}

	void DrawWall(const unique_ptr<MaterialInst>& material)
	{
		assert(primitive);

		primitive->DrawPrimitive(material);
	}

private:
	unique_ptr<Primitive> primitive;
};
