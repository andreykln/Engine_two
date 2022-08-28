#include "dx11.h"

bool Device_DirectX11::DeviceInitialized() const
{
	if (mDevice)
		return true;
	return false;
}

void Device_DirectX11::D3DInitialized() const
{
	assert(mDevice);
	assert(mSwapChain);
}

void Device_DirectX11::CreateDeviceDebugFactory()
{
#ifdef _DEBUG
	ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(mFactory.ReleaseAndGetAddressOf())));
#endif // _DEBUG
#ifndef _DEBUG
	ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(mFactory.ReleaseAndGetAddressOf())));
#endif // !_DEBUG


#ifdef _DIRECTX11
	ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		d3dFeatureLevels,
		featureLevelNum,
		D3D11_SDK_VERSION,
		mDevice.ReleaseAndGetAddressOf(),
		&featureLevelIsSupported,
		mDeviceContext.ReleaseAndGetAddressOf()));
#endif // DIRECTX11
#ifdef _DEBUG
	spdlog::info("DX11 device and factory created");
#endif // _DEBUG
}

void Device_DirectX11::CreateCommandObjects()
{
}

void Device_DirectX11::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{ 0 };
	swapChainDesc.Width = mClientWidth;
	swapChainDesc.Height = mClientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2u;
	swapChainDesc.SampleDesc.Count = 1u;
	swapChainDesc.SampleDesc.Quality = 0u;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //DXGI_SWAP_EFFECT_DISCARD
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = 0u; //DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING 
	mFactory->CreateSwapChainForHwnd(mDevice.Get(), hWnd,
		&swapChainDesc, 0, 0, mSwapChain.ReleaseAndGetAddressOf());
#ifdef _DEBUG
	spdlog::info("Swap chain created");
#endif // _DEBUG
}

void Device_DirectX11::CreateDescriptorHeap(DescriptorHeap type, UINT numDesc, DescriptorHeapFlag flag, std::string name)
{
}

void Device_DirectX11::PrepareCommandList()
{
}

void Device_DirectX11::PrepareCommandListAndAllocator()
{
}

void Device_DirectX11::CloseCommandList()
{
}

void Device_DirectX11::FlushCommandQueue()
{
}

void Device_DirectX11::ReleaseAndResizeSwapChain()
{
	if(mRenderTargetView)
	mRenderTargetView->Release();
	if (mDepthStencilView)
	mDepthStencilView->Release();
	if (mDepthStencilBuffer)
	mDepthStencilBuffer->Release();

	ThrowIfFailed(mSwapChain->ResizeBuffers(2, mClientWidth, mClientHeight, mBackBufferFormat, 0));

#ifdef _DEBUG
	spdlog::info("SwapChain recreated with dimensions {} by {}", mClientWidth, mClientHeight);
#endif // _DEBUG

}

void Device_DirectX11::CreateRenderTargetView()
{
	ID3D11Texture2D* backBuffer;
	ThrowIfFailed(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	ThrowIfFailed(mDevice->CreateRenderTargetView(backBuffer, 0, mRenderTargetView.GetAddressOf()));
	backBuffer->Release();
#ifdef _DEBUG
	spdlog::info("RenderTargetView recreated");
#endif // _DEBUG
}

void Device_DirectX11::CreateDepthStencilView()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	ThrowIfFailed(mDevice->CreateTexture2D(&depthStencilDesc, 0, mDepthStencilBuffer.GetAddressOf()));
	ThrowIfFailed(mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), 0, mDepthStencilView.GetAddressOf()));
#ifdef _DEBUG
	spdlog::info("Depth stencil view recreated");
#endif // _DEBUG

}

void Device_DirectX11::SetDepthStencilView()
{
	mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
}

void Device_DirectX11::UpdateViewport()
{
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;
	mDeviceContext->RSSetViewports(1, &mScreenViewport);
}

void Device_DirectX11::SetViewportScissorRect()
{
	mDeviceContext->RSSetViewports(1u, &mScreenViewport);
}

void Device_DirectX11::ClearRTVAndStencil() const
{
	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), mRTVClearColor);
	mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}

void Device_DirectX11::PrepareRenderTarget()
{
}

void Device_DirectX11::SetRenderTarget()
{
	mDeviceContext->OMSetRenderTargets(1u, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
}

void Device_DirectX11::PrepareRTVtoPresent()
{
}

void Device_DirectX11::Present()
{
	mSwapChain->Present(0u, 0u);
}
