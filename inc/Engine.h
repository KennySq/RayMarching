#pragma once

using namespace Microsoft::WRL;

#define address64 unsigned long long
#include"FileBrowser.h"

class Engine
{
public:
	Engine(HWND hWnd, unsigned int width, unsigned int height);
	~Engine();

	void Start();
	void Update();
	void Render();
	void Release();
private:
	struct ConstantBufferData
	{
		DirectX::XMFLOAT4 Fade;
	};

	void generateHardware();
	void generateCommands();
	void makeAssets();

	void waitGPU();

	ComPtr<ID3D12Debug> mDebug;

	ComPtr<ID3D12Device> mDevice;
	ComPtr<IDXGISwapChain4> mSwapChain;
	ComPtr<IDXGIFactory7> mFactory;
	ComPtr<IDXGIAdapter4> mAdapter;

	ComPtr<ID3D12CommandAllocator> mCmdAllocator;
	ComPtr<ID3D12CommandQueue> mCmdQueue;
	ComPtr<ID3D12GraphicsCommandList> mCmdList;

	ComPtr<ID3D12Fence> mFrameFence;
	HANDLE mFrameHandle;
	size_t mFrameIndex;

	ComPtr<ID3D12DescriptorHeap> mRTVHeap;

	D3D12_RESOURCE_BARRIER mBackBufferBarrier{};

	ComPtr<ID3D12PipelineState> mPso;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC mPsoDesc;

	unsigned int mWidth;
	unsigned int mHeight;
	const size_t CONSTANT_BUFFER_SIZE = (sizeof(ConstantBufferData) + 255) & ~255;

	ComPtr<ID3D12Resource1> mBackBuffer[2];
	D3D12_CPU_DESCRIPTOR_HANDLE mBackBufferHandle[2];

	size_t RTV_HEAP_INCREMENT;
	size_t SRV_HEAP_INCREMENT;
	ComPtr<ID3DBlob> mVertexBlob;
	ComPtr<ID3DBlob> mPixelBlob;

	ComPtr<ID3D12RootSignature> mRootSignature;

	ComPtr<ID3D12Resource> mVertexBuffer;
	ComPtr<ID3D12Resource> mIndexBuffer;

	ComPtr<ID3D12Resource> mConstantBuffer;

	ComPtr<ID3D12DescriptorHeap> mCbvSrvHeap;
	CD3DX12_ROOT_PARAMETER mRootParameters[3]{};

	std::fstream mShaderFile;

	FileBrowser mFileBrowser;

	D3D12_INPUT_ELEMENT_DESC mInputElements[2];
	HWND mHandle;

	ComPtr<ID3D12Resource> mCloudTexture;
	ComPtr<ID3D12PipelineState> mTexturePso;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC mTexturePsoDesc;
	D3D12_INPUT_ELEMENT_DESC mTextureIL[3];

	D3D12_BLEND_DESC mBlendDesc{};
	D3D12_RASTERIZER_DESC mRasterDesc{};
	D3D12_DEPTH_STENCIL_DESC mDsDesc{};
	D3D12_DEPTH_STENCILOP_DESC mDsOpDesc{};
	D3D12_RENDER_TARGET_BLEND_DESC mRtBlendDesc{};

	ComPtr<ID3DBlob> mTextureVS;
	ComPtr<ID3DBlob> mTexturePS;
	ComPtr<ID3DBlob> mTextureRSBlob;
	ComPtr<ID3D12RootSignature> mTextureRS;

	D3D12_CPU_DESCRIPTOR_HANDLE mTextureRTVHandle;
	ComPtr<ID3D12DescriptorHeap> mGeneralRTVHeap;

	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;

	D3D12_VIEWPORT mMainViewport{};
	D3D12_VIEWPORT mTextureViewport{};

	D3D12_RECT mMainScissorRect{};
	D3D12_RECT mTextureScissorRect{};
};