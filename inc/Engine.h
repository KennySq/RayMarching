#pragma once

using namespace Microsoft::WRL;

#define address64 unsigned long long

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

	ComPtr<ID3D12Resource1> mBackBuffer[2];
	D3D12_CPU_DESCRIPTOR_HANDLE mBackBufferHandle[2];

	size_t RTV_HEAP_INCREMENT;

	ComPtr<ID3DBlob> mVertexBlob;
	ComPtr<ID3DBlob> mPixelBlob;

	ComPtr<ID3D12RootSignature> mRootSignature;

	ComPtr<ID3D12Resource> mVertexBuffer;
	ComPtr<ID3D12Resource> mIndexBuffer;

	HWND mHandle;

};
