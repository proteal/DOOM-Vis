//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

template<typename Type>
class ComPtr
{
public:
	ComPtr()
		:ptr(nullptr)
	{
	}

	~ComPtr()
	{
		if (ptr)
		{
			ptr->Release();
		}
	}

	Type** ToCreate()
	{
		if (ptr)
		{
			ptr->Release();
			ptr = nullptr;
		}
		return &ptr;
	}

	Type* Ptr() const { return ptr; }

	operator bool() const { return ptr != nullptr; }

	Type* operator ->() const
	{
		assert(ptr);
		return ptr;
	}

	template<class OtherType>
	bool operator ==(const ComPtr<OtherType>& other) const { return ptr == other.ptr; }

private:
	Type* ptr;
};
