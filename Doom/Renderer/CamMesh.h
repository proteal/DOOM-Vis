//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class CamMesh final
{
public:
	explicit CamMesh(VertDescs& vertDescs)
	{
		const float length = 15.0f;
		const float height = 8.0f;
		const float width = 15.0f;
		XMFLOAT3 verts[] = { { 0.0f, 0.0f, 0.0f }, { length, width, height },
		{ length, -width, height }, { length, -width, -height }, { length, width, -height } };

		unique_ptr<VertBuffer> vertBuffer = make_unique<VertBuffer>(vertDescs, VertType::POS,
			sizeof(verts), reinterpret_cast<ubyte*>(verts));

		ushort indices[] = { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1 };
		unique_ptr<IndexBuffer> indexBuffer = make_unique<IndexBuffer>(sizeof(indices),
			reinterpret_cast<ubyte*>(indices), IndexType::UINT16);

		primitive = make_unique<Primitive>(PrimType::TRIANGLE_LIST, sizeof(indices) / sizeof(*indices), 
			0, vertBuffer, indexBuffer);
	}

	void DrawCam(const unique_ptr<MaterialInst>& material)
	{
		assert(primitive);
		primitive->DrawPrimitive(material);
	}

	void operator =(const CamMesh&) = delete;

private:
	unique_ptr<Primitive> primitive;
};
