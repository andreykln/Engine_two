#pragma once
#include <d3d12.h>
//#include <DXGI1_4.h>

#include <DXGI1_5.h>
#include "d3dx12.h"
#include "Device.h"

#pragma comment(lib, "dxgi.lib")

class Device_DirectX12 : public Device
{
public:
	bool DeviceInitialized() const override;
	void D3DInitialized() const override;
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
	void ClearRTVAndStencil() const override;
	void PrepareRenderTarget() override;
	void SetRenderTarget() override;
	void PrepareRTVtoPresent() override;
	void Present() override;
private:
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	ID3D12Resource* CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	Microsoft::WRL::ComPtr<ID3D12Device5> mDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	UINT64 mCurrentFence = 0;
	Microsoft::WRL::ComPtr<IDXGIFactory5> mFactory;
	Microsoft::WRL::ComPtr<ID3D12Debug3> mDebugController;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
	static const int SwapChainBufferCount{ 2 };
	int mCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	UINT mRtvDescriptorSize{ 0 };
	UINT mDsvDescriptorSize{ 0 };
	UINT mCbvSrvUavDescriptorSize{ 0 };
	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;
	const DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	const DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> mDescriptorMap;
	const DescriptorHeapMap mDescriptorHeapMapNames;
};


class dx12
{

};

