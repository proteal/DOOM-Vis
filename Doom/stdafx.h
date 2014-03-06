//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

#include <wildmidi_lib.h>

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <D3D11.h>
#include <D3Dcompiler.h>

#include <xaudio2.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>
#include <io.h>
#define R_OK 4
#include <fcntl.h>
#include <sys/stat.h>

#include <string>
#include <memory>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <array>
#include <unordered_map>
#include <thread>
#include <condition_variable>
#include <atomic>

#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

typedef unsigned int uint;
typedef long long int64;
typedef unsigned char ubyte;
typedef unsigned short ushort;

inline void ThrowException(const char* file, long line, const char* errText)
{
	stringstream ss;
	ss  << errText << "\n"<< file << " (" << line << ")\n";
	throw exception(ss.str().c_str());
}

#define THROW(errText) ThrowException(__FILE__, __LINE__, errText);

template<typename Key, typename Val, typename CreateFunc>
const Val& MapGetOrCreate(unordered_map<Key, Val>& map, const Key& key, const CreateFunc& createFunc)
{
	const Val& val = map[key];
	if (val)
	{
		return val;
	}

	Val newVal = createFunc();
	return map[key] = newVal;
}

inline wstring StrA_ToW(const char* strA)
{
	string a(strA);
	return wstring(a.begin(), a.end());
}

inline string StrW_ToA(const wstring& strW)
{
	return string(strW.begin(), strW.end());
}

#ifdef _DEBUG
#define USE_LOG 1
#else
#define USE_LOG 0
#endif
