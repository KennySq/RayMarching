#include"pch.h"
#include"Engine.h"

#include"ShaderHelper.hpp"

using namespace DirectX;

Engine::Engine(HWND hWnd, unsigned int width, unsigned int height)
	: mHandle(hWnd), mWidth(width), mHeight(height)
{
	AllocConsole();
}

Engine::~Engine()
{
}

void Engine::Start()
{
	std::cout << "Engine Started\n";
	generateHardware();
	makeAssets();


}

void Engine::Update()
{
	HRESULT result = mCmdAllocator->Reset();
	assert(result == S_OK);
	
	result = mCmdList->Reset(mCmdAllocator.Get(), nullptr);
	assert(result == S_OK);

	D3D12_RESOURCE_TRANSITION_BARRIER transition{};

	transition.pResource = mBackBuffer[mFrameIndex].Get();
	transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	mBackBufferBarrier.Transition = transition;
	mBackBufferBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	
	mCmdList->ResourceBarrier(1, &mBackBufferBarrier);

	mCmdList->ClearRenderTargetView(mBackBufferHandle[mFrameIndex], Colors::Red, 0, nullptr);
	

	transition.pResource = mBackBuffer[mFrameIndex].Get();
	transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	mBackBufferBarrier.Transition = transition;
	mBackBufferBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	mCmdList->ResourceBarrier(1, &mBackBufferBarrier);

	
	mCmdList->Close();



}

void Engine::Render()
{
	static ID3D12CommandList* cmdLists[] = { mCmdList.Get() };



	mCmdQueue->ExecuteCommandLists(1, cmdLists);


	mSwapChain->Present(0, 0);

	waitGPU();
}

void Engine::Release()
{
	std::cout << "Engine Destroy\n";

}

void Engine::generateHardware()
{
	HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&mDebug));
	assert(result == S_OK);

	mDebug->EnableDebugLayer();

	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&mFactory));
	assert(result == S_OK);

	result = mFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&mAdapter));
	assert(result == S_OK);

	DXGI_ADAPTER_DESC adapterDesc{};
	result = mAdapter->GetDesc(&adapterDesc);
	assert(result == S_OK);

	std::wcout << "GPU Selected : " << adapterDesc.Description << '\n';

	result = D3D12CreateDevice(mAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mDevice));
	assert(result == S_OK);

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};

	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	result = mDevice->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&mCmdQueue));
	assert(result == S_OK);

	DXGI_SWAP_CHAIN_DESC1 scDesc{};
	
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scDesc.BufferCount = 2;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.Width = mWidth;
	scDesc.Height = mHeight;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Scaling = DXGI_SCALING_NONE;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Stereo = false;

	ComPtr<IDXGISwapChain1> swapChain1;

	result = mFactory->CreateSwapChainForHwnd(mCmdQueue.Get(), mHandle, &scDesc, nullptr, nullptr, swapChain1.GetAddressOf());
	assert(result == S_OK);
		
	result = swapChain1.As<IDXGISwapChain4>(&mSwapChain);
	assert(result == S_OK);

}

void Engine::generateCommands()
{
}

void Engine::makeAssets()
{
	HRESULT result = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCmdAllocator));
	assert(result == S_OK);

	result = mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFrameFence));
	assert(result == S_OK);

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	result = mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRTVHeap));
	assert(result == S_OK);

	D3D12_CPU_DESCRIPTOR_HANDLE firstHeapHandle = mRTVHeap->GetCPUDescriptorHandleForHeapStart();

	RTV_HEAP_INCREMENT = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (unsigned int i = 0; i < 2; i++)
	{
		result = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mBackBuffer[i]));
		assert(result == S_OK);

		mBackBufferHandle[i].ptr = firstHeapHandle.ptr + (RTV_HEAP_INCREMENT * i);


		mDevice->CreateRenderTargetView(mBackBuffer[i].Get(), &rtvDesc, mBackBufferHandle[i]);
	}

	result = mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&mCmdList));
	assert(result == S_OK);

	mCmdList->Close();

	waitGPU();

	result = ShaderHelper::Compile(L"C:\\Users\\odess\\Desktop\\Projects\\RayMarching\\assets\\someShader.hlsl", "main", "vs_5_0", mVertexBlob);
	assert(result == S_OK);
}

void Engine::waitGPU()
{
	const size_t frameIndex = mFrameFence->GetCompletedValue() + 1;

	mCmdQueue->Signal(mFrameFence.Get(), frameIndex);

	mFrameIndex++;

	if (mFrameFence->GetCompletedValue() < frameIndex)
	{
		mFrameFence->SetEventOnCompletion(frameIndex, mFrameHandle);
		WaitForSingleObject(mFrameHandle, INFINITE);

	}
	
	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

	return;
}
