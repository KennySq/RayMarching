#include"pch.h"
#include"Engine.h"

#include"ShaderHelper.hpp"
#include"ScreenHelper.h"

#include"FileBrowser.h"

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
	ImGui::NewFrame();
	ImGui::SetNextWindowSize(ImVec2(400, 600));
	static char buffer[8192];
	static bool bOpenBrowser;
	static size_t fileSize = 0;
	static bool bCompile = true;

	if (bOpenBrowser == true)
	{
		mFileBrowser.DisplayAssets();
		bool bOpenFile = false;
		std::fstream& file = mFileBrowser.GetFile();

		if (file.is_open() == true)
		{
			ZeroMemory(buffer, 8192);

			file.read(buffer, 8192);

			std::string bufferString = buffer;
			size_t size = bufferString.size();

			fileSize = size;

			file.close();
		}
	}
	ImGui::SetNextWindowSize(ImVec2(800, 800));

	ImGui::Begin("Shader Editor");
	if (ImGui::Button("Open File"))
	{
		bOpenBrowser = !bOpenBrowser;


	}
	if (ImGui::InputTextMultiline("Editor", buffer, 8192, ImVec2(800, 500), ImGuiInputTextFlags_AllowTabInput))
	{
		std::string bufferString = buffer;
		size_t size = bufferString.size();

		fileSize = size;
	}

	ImGui::Text("File Size : %d bytes", fileSize);
	if (bCompile == false)
	{
		ImGui::Text("Compilation Failed, Check console log for further details.");
	}
	if (ImGui::Button("Compile"))
	{
		const std::wstring& path = mFileBrowser.GetFileName();

		ComPtr<ID3DBlob> blob;
		ShaderHelper::Compile(path.c_str(), "frag", "ps_5_0", blob);

		if (blob != nullptr)
		{
			bCompile = true;
			mPixelBlob.ReleaseAndGetAddressOf();
			mPixelBlob = blob.Detach();

			ComPtr<ID3D12PipelineState> pso;

			mPsoDesc.PS = D3D12_SHADER_BYTECODE{ mPixelBlob->GetBufferPointer(), mPixelBlob->GetBufferSize() };
			HRESULT result = mDevice->CreateGraphicsPipelineState(&mPsoDesc, IID_PPV_ARGS(&pso));
			if (result != S_OK)
			{
				std::cout << "Something went wrong.\n";
			}
			else
			{
				mPso.ReleaseAndGetAddressOf();
				mPso = pso.Detach();
			}
		}
		else
		{
			bCompile = false;
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		std::ofstream file = std::ofstream(mFileBrowser.GetFilePath(), std::ofstream::out);

		if (file.is_open() == true)
		{
			std::wcout << mFileBrowser.GetFilePath() << L" has been saved.\n";
		}

		file << buffer;

		file.close();
	}

	ImGui::End();

	ImGui::Begin("Variable Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	static float fade = 0.0f;
	ImGui::SliderFloat("Fade", &fade, 0.1f, 3.0f, "%f", 1.0f);

	D3D12_RANGE mapRange{};

	ConstantBufferData* cData{};
	HRESULT result = mConstantBuffer->Map(0, &mapRange, reinterpret_cast<void**>(&cData));
	assert(result == S_OK);

	cData->Fade = XMFLOAT4(fade, fade, fade, fade);

	mConstantBuffer->Unmap(0, nullptr);

	ImGui::End();

	ImGui::EndFrame();
	result = mCmdAllocator->Reset();
	assert(result == S_OK);
	
	result = mCmdList->Reset(mCmdAllocator.Get(), mPso.Get());
	assert(result == S_OK);

	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};

	cbvDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = CONSTANT_BUFFER_SIZE;

	vbView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(Vertex) * 4;
	vbView.StrideInBytes = sizeof(Vertex);

	ibView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
	ibView.SizeInBytes = sizeof(unsigned int) * 6;
	ibView.Format = DXGI_FORMAT_R32_UINT;

	D3D12_RESOURCE_TRANSITION_BARRIER transition{};

	transition.pResource = mBackBuffer[mFrameIndex].Get();
	transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	mBackBufferBarrier.Transition = transition;
	mBackBufferBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	
	mCmdList->ResourceBarrier(1, &mBackBufferBarrier);

	D3D12_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(mWidth);
	viewport.Height = static_cast<float>(mHeight);
	viewport.MaxDepth = 1.0f;

	D3D12_RECT scissorRect{};
	scissorRect.right = mWidth;
	scissorRect.bottom = mHeight;

	mCmdList->RSSetViewports(1, &viewport);
	mCmdList->RSSetScissorRects(1, &scissorRect);

	mCmdList->ClearRenderTargetView(mBackBufferHandle[mFrameIndex], Colors::Red, 0, nullptr);

	mCmdList->SetPipelineState(mPso.Get());
	
	mCmdList->IASetVertexBuffers(0, 1, &vbView);
	mCmdList->IASetIndexBuffer(&ibView);
	mCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	mCmdList->SetGraphicsRootSignature(mRootSignature.Get());
	mCmdList->SetGraphicsRootConstantBufferView(0, mConstantBuffer->GetGPUVirtualAddress());
	
	mCmdList->OMSetRenderTargets(1, &mBackBufferHandle[mFrameIndex], false, nullptr);
	//mCmdList->SetDescriptorHeaps(1, mCbvSrvHeap.GetAddressOf());
	mCmdList->SetDescriptorHeaps(1, mCbvSrvHeap.GetAddressOf());

	D3D12_GPU_DESCRIPTOR_HANDLE cbvgpuHandle = mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart();

	mCmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

	transition.pResource = mBackBuffer[mFrameIndex].Get();
	transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	mBackBufferBarrier.Transition = transition;
	mBackBufferBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCmdList.Get());

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
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();

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


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsLight();
	D3D12_DESCRIPTOR_HEAP_DESC cbvsrvHeapDesc{};

	cbvsrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvsrvHeapDesc.NumDescriptors = 2;
	cbvsrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	result = mDevice->CreateDescriptorHeap(&cbvsrvHeapDesc, IID_PPV_ARGS(&mCbvSrvHeap));
	assert(result == S_OK);

	D3D12_CPU_DESCRIPTOR_HANDLE cbvsrvCPUHandle = mCbvSrvHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE cbvsrvGPUHandle = mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart();

	bool bResult = ImGui_ImplDX12_Init(mDevice.Get(), 2, DXGI_FORMAT_R8G8B8A8_UNORM, mCbvSrvHeap.Get(), cbvsrvCPUHandle, cbvsrvGPUHandle);
	assert(bResult == true);

	bResult = ImGui_ImplWin32_Init(mHandle);
	assert(bResult == true);

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX12_NewFrame();
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

	result = ShaderHelper::Compile(L"ScreenQuad.hlsl", "vert", "vs_5_0", mVertexBlob);
	//assert(result == S_OK);
	//assert(mVertexBlob != nullptr);

	result = ShaderHelper::Compile(L"RayMarch_0.hlsl", "frag", "ps_5_0", mPixelBlob);
	//assert(result == S_OK);
	//assert(mPixelBlob != nullptr);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	ScreenHelper::MakeQuad(vertices, indices);

	D3D12_RESOURCE_DESC vbDesc{}, ibDesc{}, cbDesc{};
	D3D12_HEAP_PROPERTIES heapProps{};

	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProps.VisibleNodeMask = 1;
	heapProps.CreationNodeMask = 1;

	vbDesc.Format = DXGI_FORMAT_UNKNOWN;
	vbDesc.Width = sizeof(Vertex) * 4;
	vbDesc.Height = 1;
	vbDesc.DepthOrArraySize = 1;
	vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vbDesc.MipLevels = 1;
	vbDesc.SampleDesc.Count = 1;
	vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ibDesc.Format = DXGI_FORMAT_UNKNOWN;
	ibDesc.Width = sizeof(unsigned int) * 6;
	ibDesc.Height = 1;
	ibDesc.DepthOrArraySize = 1;
	ibDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ibDesc.MipLevels = 1;
	ibDesc.SampleDesc.Count = 1;
	ibDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	cbDesc.Format = DXGI_FORMAT_UNKNOWN;
	cbDesc.Width = CONSTANT_BUFFER_SIZE;
	cbDesc.Height = 1;
	cbDesc.DepthOrArraySize = 1;
	cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbDesc.MipLevels = 1;
	cbDesc.SampleDesc.Count = 1;
	cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_INPUT_ELEMENT_DESC inputElements[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	mInputElements[0] = inputElements[0];
	mInputElements[1] = inputElements[1];

	result = mDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &vbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mVertexBuffer));
	assert(result == S_OK);

	result = mDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &ibDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mIndexBuffer));
	assert(result == S_OK);

	result = mDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &cbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mConstantBuffer));
	assert(result == S_OK);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = CONSTANT_BUFFER_SIZE;

	D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle;
	cbvHandle.ptr = mCbvSrvHeap->GetCPUDescriptorHandleForHeapStart().ptr + mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	mDevice->CreateConstantBufferView(&cbvDesc, cbvHandle);
	
	void* mapPtr;
	D3D12_RANGE readRange{};

	result = mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mapPtr));
	assert(result == S_OK);
	memcpy(mapPtr, vertices.data(), sizeof(Vertex) * 4);
	mVertexBuffer->Unmap(0, nullptr);

	mapPtr = nullptr;

	result = mIndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mapPtr));
	assert(result == S_OK);
	memcpy(mapPtr, indices.data(), sizeof(unsigned int) * 6);
	mIndexBuffer->Unmap(0, nullptr);

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	D3D12_BLEND_DESC blendDesc{};
	D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc{};
	D3D12_DEPTH_STENCIL_DESC dsDesc{};
	D3D12_DEPTH_STENCILOP_DESC dsopDesc{};
	D3D12_RASTERIZER_DESC rasterDesc{};
	D3D12_ROOT_SIGNATURE_DESC rsDesc{};

	D3D12_DESCRIPTOR_RANGE descRange{};
	
	rtBlendDesc.BlendEnable = false;
	rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	rtBlendDesc.SrcBlend = D3D12_BLEND_ONE;
	rtBlendDesc.DestBlend = D3D12_BLEND_ZERO;
	rtBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	rtBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	rtBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	rtBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	rtBlendDesc.LogicOpEnable = false;
	rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	for (int i = 0; i < 8; i++)
	{
		blendDesc.RenderTarget[i] = rtBlendDesc;
	}

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	inputLayoutDesc.NumElements = ARRAYSIZE(mInputElements);
	inputLayoutDesc.pInputElementDescs = mInputElements;

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterDesc.DepthClipEnable = true;

	mRootParameters[0].InitAsConstantBufferView(0);

	rsDesc.NumParameters = 1;
	rsDesc.pParameters = mRootParameters;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	
	/*
	rsDesc.pParameters = nullptr;
	rsDesc.NumParameters = 0;*/
	
	ComPtr<ID3DBlob> rsBlob, errBlob;

	result = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, rsBlob.GetAddressOf(), errBlob.GetAddressOf());
	if (errBlob != nullptr)
	{
		std::cout << reinterpret_cast<const char*>(errBlob->GetBufferPointer()) << '\n';
	}
	assert(result == S_OK);

	result = mDevice->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
	assert(result == S_OK);

	mPsoDesc.InputLayout = inputLayoutDesc;
	mPsoDesc.BlendState = blendDesc;
	mPsoDesc.DepthStencilState = dsDesc;
	mPsoDesc.NumRenderTargets = 1;
	mPsoDesc.RasterizerState = rasterDesc;
	mPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	
	if (mVertexBlob != nullptr)
	{
		mPsoDesc.VS = D3D12_SHADER_BYTECODE{ mVertexBlob->GetBufferPointer(), mVertexBlob->GetBufferSize() };
	}

	if (mPixelBlob != nullptr)
	{
		mPsoDesc.PS = D3D12_SHADER_BYTECODE{ mPixelBlob->GetBufferPointer(), mPixelBlob->GetBufferSize() };
	}

	mPsoDesc.SampleDesc.Count = 1;
	mPsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	mPsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	mPsoDesc.pRootSignature = mRootSignature.Get();
	mPsoDesc.SampleMask = UINT_MAX;

	result = mDevice->CreateGraphicsPipelineState(&mPsoDesc, IID_PPV_ARGS(&mPso));
	assert(result == S_OK);


	mFrameHandle = CreateEvent(nullptr, false, false, nullptr);
	
	
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