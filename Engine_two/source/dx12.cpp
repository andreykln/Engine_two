#include "dx12.h"



bool Device_DirectX12::DeviceInitialized()
{
	if (mDevice)
		return true;
	return false;
}

void Device_DirectX12::D3DInitialized()
{
	assert(mDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAlloc);
}

void Device_DirectX12::CreateDebugAndFactory()
{
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(mDebugController.GetAddressOf())));
	mDebugController->EnableDebugLayer();
	mDebugController->SetEnableGPUBasedValidation(TRUE);
	mDebugController->SetEnableSynchronizedCommandQueueValidation(TRUE);
	mDebugController->SetGPUBasedValidationFlags(D3D12_GPU_BASED_VALIDATION_FLAGS_NONE);

#ifdef _DEBUG	
	spdlog::info("Debug layer created");
	spdlog::info("GPU Based validation enabled");
	spdlog::info("GPU based validation flags set to all");
	spdlog::info("Synchronized command queue validation enabled");

	ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(mFactory.ReleaseAndGetAddressOf())));

	static const D3D_FEATURE_LEVEL s_featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(mDevice.ReleaseAndGetAddressOf())));
	ThrowIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));

	D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
	{
		_countof(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
	};
	
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hr = mDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS,
		&featLevels, sizeof(featLevels));
	ThrowIfFailed(hr);
	if (SUCCEEDED(hr))
	{
		featureLevel = featLevels.MaxSupportedFeatureLevel;
	}

	static const std::unordered_map<int, std::string> featureLevelNames{ 
		{0xc200, "D3D_FEATURE_LEVEL_12_2"},
		{0xc100, "D3D_FEATURE_LEVEL_12_1"},
		{0xc000, "D3D_FEATURE_LEVEL_12_0"},
		{0xb100, "D3D_FEATURE_LEVEL_11_1"},
		{0xb100, "D3D_FEATURE_LEVEL_11_1"}
	};
	spdlog::info("Feature level: {}", featureLevelNames.at(featureLevel));
#endif
#ifndef _DEBUG
	CreateDXGIFactory2(0, IID_PPV_ARGS(mFactory.ReleaseAndGetAddressOf()));
	ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(mDevice.ReleaseAndGetAddressOf())));
	ThrowIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
#endif
	mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
#ifdef _DEBUG
	spdlog::info("RTV Descriptor size: {}", mRtvDescriptorSize);
	spdlog::info("DSV Descriptor size: {}", mDsvDescriptorSize);
	spdlog::info("CBV_SRV_UAV Descriptor size: {}", mCbvSrvUavDescriptorSize);
#endif // _DEBUG


}

void Device_DirectX12::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(mCommandQueue.ReleaseAndGetAddressOf())));

	ThrowIfFailed(mDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCmdListAlloc.ReleaseAndGetAddressOf())));

	ThrowIfFailed(mDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(), 
		nullptr,                   
		IID_PPV_ARGS(mCommandList.ReleaseAndGetAddressOf())));

#ifdef _DEBUG
	spdlog::info("Command object created");
#endif // _DEBUG
	mCommandList->Close();
}

void Device_DirectX12::CreateSwapChain()
{
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC1 sd;
	sd.Width = mClientWidth;
	sd.Height = mClientHeight;
	sd.Format = mBackBufferFormat;
	sd.Stereo = FALSE;
	sd.SampleDesc = { 1, 0 };
	sd.Scaling = DXGI_SCALING_STRETCH;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGISwapChain1* pDXGIsw1 = nullptr;
	ThrowIfFailed(mFactory->CreateSwapChainForHwnd(
		mCommandQueue.Get(), hWnd,
		&sd, nullptr, nullptr,
		&pDXGIsw1));
	//TODO IID_IDXGISwapChain4 instead of uuidof causes linker error, possible to ignore?
	ThrowIfFailed(pDXGIsw1->QueryInterface(__uuidof(IDXGISwapChain4), (LPVOID*)mSwapChain.ReleaseAndGetAddressOf()));
	//ThrowIfFailed(pDXGIsw1->QueryInterface(IID_IDXGISwapChain4, (LPVOID*)mSwapChain.ReleaseAndGetAddressOf()));

	pDXGIsw1->Release();
	
}

void Device_DirectX12::CreateDescriptorHeap(DescriptorHeap type, UINT numDesc, DescriptorHeapFlag flag, std::string name)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	switch (type)
	{
	case DescriptorHeap::SRV_UAV_CBV:
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		break;
	case DescriptorHeap::Sampler:
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		break;
	case DescriptorHeap::RTV:
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		break;
	case DescriptorHeap::DSV:
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		break;
	case DescriptorHeap::Num_types:
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
		break;
	}
	switch (flag)
	{
	case DescriptorHeapFlag::None:
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		break;
	case DescriptorHeapFlag::ShaderVisible:
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		break;
	}
	
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = numDesc;
	ThrowIfFailed(mDevice->CreateDescriptorHeap(
		&heapDesc, IID_PPV_ARGS(mDescriptorMap[name].GetAddressOf())));
#ifdef _DEBUG
	spdlog::info("Descriptor heap {} created", name);
#endif // _DEBUG

}

void Device_DirectX12::PrepareCommandList()
{
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
#ifdef _DEBUG
	spdlog::info("Command list reset");
#endif // _DEBUG
}

void Device_DirectX12::CloseCommandList()
{
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

void Device_DirectX12::FlushCommandQueue()
{
	mCurrentFence++;
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));
	if (mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventExA(nullptr, NULL, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void Device_DirectX12::ReleaseAndResizeSwapChain()
{
	for (int i = 0; i < SwapChainBufferCount; ++i)
		mSwapChainBuffer[i].Reset();
	mDepthStencilBuffer.Reset();
	mCurrBackBuffer = 0;
	ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount,
		mClientWidth, mClientHeight,
		mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

#ifdef _DEBUG
	spdlog::info("Swapchain resized with {} by {}", mClientWidth, mClientHeight);
#endif // _DEBUG

}

void Device_DirectX12::CreateRenderTargetView()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mDescriptorMap[mDescriptorHeapMapNames.renderTargetView]->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		mDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}
#ifdef _DEBUG
	spdlog::info("Create render target view");
#endif // _DEBUG
}

void Device_DirectX12::PrepareRenderTarget()
{
	CD3DX12_RESOURCE_BARRIER t = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mCommandList->ResourceBarrier(1, &t);
}

void Device_DirectX12::SetRenderTarget()
{
	D3D12_CPU_DESCRIPTOR_HANDLE t = DepthStencilView();
	D3D12_CPU_DESCRIPTOR_HANDLE m = CurrentBackBufferView();
	mCommandList->OMSetRenderTargets(1,
		&m,
		true, &t);
}

void Device_DirectX12::PrepareRTVtoPresent()
{
	CD3DX12_RESOURCE_BARRIER t = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	mCommandList->ResourceBarrier(1, &t);
}

void Device_DirectX12::Present()
{
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
}

void Device_DirectX12::PrepareCommandListAndAllocator()
{
	ThrowIfFailed(mDirectCmdListAlloc->Reset());
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
}

void Device_DirectX12::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	//TODO wtf
	CD3DX12_HEAP_PROPERTIES hp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(mDevice->CreateCommittedResource(
		&hp,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

#ifdef _DEBUG
	spdlog::info("Depth stencil view created");
#endif // _DEBUG
}

void Device_DirectX12::SetDepthStencilView()
{
	CD3DX12_RESOURCE_BARRIER t = CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	mCommandList->ResourceBarrier(1, &t);
}

void Device_DirectX12::UpdateViewport()
{
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;
#ifdef _DEBUG
	spdlog::info("view port updated");
#endif // _DEBUG

	mScissorRect = { 0, 0, mClientWidth, mClientHeight };
}

void Device_DirectX12::SetViewportScissorRect()
{
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);
}

void Device_DirectX12::ClearRTVAndStencil()
{
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

}



D3D12_CPU_DESCRIPTOR_HANDLE Device_DirectX12::DepthStencilView()const
{
	return mDescriptorMap.at(mDescriptorHeapMapNames.depthStencilView)->GetCPUDescriptorHandleForHeapStart();
}


ID3D12Resource* Device_DirectX12::CurrentBackBuffer()const
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE Device_DirectX12::CurrentBackBufferView()const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mDescriptorMap.at(mDescriptorHeapMapNames.renderTargetView)->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer,
		mRtvDescriptorSize);
}