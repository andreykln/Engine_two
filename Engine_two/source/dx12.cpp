#include "dx12.h"

void Device_DirectX12::CreateDebugAndFactory()
{
	spdlog::info("Debug layer created");
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(mDebugController.GetAddressOf())));
	mDebugController->EnableDebugLayer();
#ifdef _DEBUG	
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
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;

	ThrowIfFailed(mDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
	
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
	spdlog::info("Command objects created");
#endif // _DEBUG
	mCommandList->Close();
}

void Device_DirectX12::CreateSwapChain()
{
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC1 sd;
	sd.Width = width;
	sd.Height = heigth;
	sd.Format = mBackBufferFormat;
	sd.Stereo = FALSE;
	sd.SampleDesc = { 1, 0 };
	sd.Scaling = DXGI_SCALING_STRETCH;
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(mFactory->CreateSwapChainForHwnd(
		mCommandQueue.Get(), hWnd,
		&sd, nullptr, nullptr,
		mSwapChain.ReleaseAndGetAddressOf()));
	//TODO usage??
	DXGI_PRESENT_PARAMETERS presentParameters;
	presentParameters.DirtyRectsCount = 0u;
	presentParameters.pDirtyRects = NULL;
	presentParameters.pScrollOffset = NULL;
	presentParameters.pScrollRect = NULL;
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

