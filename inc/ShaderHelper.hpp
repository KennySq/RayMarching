#pragma once
#include"pch.h"


class ShaderHelper
{
public:
	static HRESULT Compile(const wchar_t* path, const char* entry, const char* target, ComPtr<ID3DBlob>& outShader)
	{
		wchar_t buffer[256];
		
		std::wstring projectPath;
		GetModuleFileName(nullptr, buffer, 256);

		projectPath = buffer;
		
		projectPath = projectPath.substr(0, projectPath.find_last_of('\\'));
		projectPath = projectPath.substr(0, projectPath.find_last_of('\\'));
		projectPath = projectPath.substr(0, projectPath.find_last_of('\\'));
		projectPath += L"\\assets\\";
		projectPath += path;

		ID3DBlob* errBlob = nullptr;
		HRESULT result = D3DCompileFromFile(projectPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, target, D3DCOMPILE_DEBUG, 0, outShader.GetAddressOf(), &errBlob);
		if (result == 0x80070003)
		{
			MessageBox(nullptr, L"File Not Found.", L"Error", MB_OK);
			std::wcout << "File Not Found.\n";

			return E_INVALIDARG;
		}
		
		if (errBlob != nullptr)
		{
			std::wcout << reinterpret_cast<const wchar_t*>(errBlob->GetBufferPointer()) << '\n';
			assert(true);
		}

		return S_OK;

	}	

};