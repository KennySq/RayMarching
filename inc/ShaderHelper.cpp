#include "pch.h"
#include "ShaderHelper.hpp"

using namespace Microsoft::WRL;

HRESULT ShaderHelper::Compile(const wchar_t* path, const char* entry, const char* target, ComPtr<ID3DBlob>& outShader)
{
	wchar_t buffer[256];

	std::wstring projectPath;
	GetModuleFileName(nullptr, buffer, 256);

	projectPath = buffer;

	projectPath = projectPath.substr(0, projectPath.find_last_of('\\') + 1);
	projectPath += path;

	ComPtr<ID3DBlob> blob;

	ID3DBlob* errBlob = nullptr;
	HRESULT result = D3DCompileFromFile(projectPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, target, D3DCOMPILE_DEBUG, 0, blob.GetAddressOf(), &errBlob);
	if (result == 0x80070003)
	{
		MessageBox(nullptr, L"File Not Found.", L"Error", MB_OK);
		std::wcout << "File Not Found.\n";

		return E_INVALIDARG;
	}

	if (errBlob != nullptr)
	{
		std::wcout << "Compilation Failed!\n";
		std::cout << reinterpret_cast<const char*>(errBlob->GetBufferPointer()) << '\n';
		return result;
	}

	if (outShader != nullptr)
	{
		outShader.Reset();

	}
	outShader.Attach(blob.Detach());

	return S_OK;

}
