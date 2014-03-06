//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class DataFile
{
public:
	explicit DataFile(const wstring& fileName)
	{
		wstring fileFull = Global::Get().GetDataPath() + fileName;
		ifstream file(fileFull, ios::in | ios::binary | ios::ate);

		if (!file.good())
		{
			THROW("File open failed.");
		}

		ifstream::pos_type fileSize = file.tellg();
		file.seekg(0, ios::beg);

		data.resize(static_cast<size_t>(fileSize));
		file.read(reinterpret_cast<char*>(data.data()), fileSize);
		file.close();
	}

	const ubyte* GetData() const { return data.data(); }
	size_t GetSize() const { return data.size(); }

private:
	vector<ubyte> data;
};
