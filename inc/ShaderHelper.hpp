#pragma once
#include"pch.h"

using namespace Microsoft::WRL;

class ShaderHelper
{
public:
	static HRESULT Compile(const wchar_t* path, const char* entry, const char* target, ComPtr<ID3DBlob>& outShader);

};