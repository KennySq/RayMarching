#pragma once
#include"pch.h"


class ShaderHelper
{
public:
	static HRESULT Compile(const wchar_t* path, const char* entry, const char* target, ComPtr<ID3DBlob>& outShader)
	{
		ID3DBlob* errBlob = nullptr;
		HRESULT result = D3DCompileFromFile(path, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, target, D3DCOMPILE_DEBUG, 0, outShader.GetAddressOf(), &errBlob);
		if (errBlob != nullptr)
		{
			std::wcout << reinterpret_cast<const wchar_t*>(errBlob->GetBufferPointer()) << '\n';
			assert(true);
		}

		return S_OK;

	}

};