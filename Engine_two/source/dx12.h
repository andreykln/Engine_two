#pragma once
#include <d3d12.h>
//#include <DXGI1_4.h>

#include <DXGI1_5.h>
#include <wrl.h>

#include "Device.h"
#include <unordered_map>
#include "spdlog/spdlog.h"
//#pragma comment(lib, "dxgi.lib")

class Device_DirectX12 : public Device
{
public:
	void CreateDebugAndFactory() override;
	void CreateCommandObjects() override;
	void CreateSwapChain() override;
	void CreateDescriptorHeap(DescriptorHeap type, UINT numDesc, DescriptorHeapFlag flag, std::string name) override;
private:
	Microsoft::WRL::ComPtr<ID3D12Device5> mDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	Microsoft::WRL::ComPtr<IDXGIFactory5> mFactory;
	Microsoft::WRL::ComPtr<ID3D12Debug3> mDebugController;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
	static const int SwapChainBufferCount{ 2 };
	Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
	UINT mRtvDescriptorSize{ 0 };
	UINT mDsvDescriptorSize{ 0 };
	UINT mCbvSrvUavDescriptorSize{ 0 };
	UINT m4xMsaaQuality;
	//TODO figure out MS
	bool m4xMsaaState = false;
	const DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	const DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> mDescriptorMap;

};


class dx12
{

};

