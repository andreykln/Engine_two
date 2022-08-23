#pragma once
#include <d3d11.h>
#include "dxgi1_3.h"
#include "dxgidebug.h"
#include "Device.h"



class Device_DirectX11 : public Device
{
public:
	bool DeviceInitialized() override;
	void D3DInitialized() override;
	void CreateDeviceDebugFactory() override;
	void CreateCommandObjects() override;
	void CreateSwapChain() override;
	void CreateDescriptorHeap(DescriptorHeap type, UINT numDesc, DescriptorHeapFlag flag, std::string name) override;
	void PrepareCommandList() override;
	void PrepareCommandListAndAllocator() override;
	void CloseCommandList() override;
	void FlushCommandQueue() override;
	void ReleaseAndResizeSwapChain() override;
	void CreateRenderTargetView() override;
	void CreateDepthStencilView() override;
	void SetDepthStencilView() override;
	void UpdateViewport() override;
	void SetViewportScissorRect() override;
	void ClearRTVAndStencil() override;
	void PrepareRenderTarget() override;
	void SetRenderTarget() override;
	void PrepareRTVtoPresent() override;
	void Present() override;

	Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDeviceContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;
	Microsoft::WRL::ComPtr<IDXGIFactory2> mFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mBackBuffer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTextureDepthStencil;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
	const DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D11_VIEWPORT mScreenViewport;
#ifdef _DEBUG
	ID3D11Debug* debugDevice = nullptr;
#endif
	D3D_FEATURE_LEVEL featureLevelIsSupported = {};
	static constexpr UINT featureLevelNum = 7;
	const D3D_FEATURE_LEVEL d3dFeatureLevels[featureLevelNum]
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
};


class dx11
{
};

