//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class Materials
{
public:
	Materials(Samplers& _samplers, ConstBuffers& _buffers)
		:samplers(_samplers)
		, buffers(_buffers)
	{
	}

	void operator =(const Materials&) = delete;

	const shared_ptr<Material>& GetMaterial(const wstring& file)
	{
		return MapGetOrCreate(materials, file, [this, &file]()
		{
			return make_shared<Material>(file, samplers, buffers);
		});
	}

private:
	Samplers& samplers;
	ConstBuffers& buffers;
	unordered_map<wstring, shared_ptr<Material>> materials;
};
