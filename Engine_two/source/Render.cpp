#include "Render.h"

Render::Render(HWND handle, const int w, const int h)
{
	mDX12Device.SetWindowParams(handle, w, h);
}

void Render::InitializeD3D()
{
	mDX12Device.CreateDebugAndFactory();
	mDX12Device.CreateCommandObjects();
	mDX12Device.CreateSwapChain();
	//swap chain
	mDX12Device.CreateDescriptorHeap(DescriptorHeap::RTV, 2, DescriptorHeapFlag::None, "RenderTargetView");
	mDX12Device.CreateDescriptorHeap(DescriptorHeap::DSV, 1, DescriptorHeapFlag::None, "DepthStencilView");

}


