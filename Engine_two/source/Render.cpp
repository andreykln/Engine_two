#include "Render.h"

Device_DirectX12* Render::mDX12Device = new Device_DirectX12;
//Device_DirectX11* Render::mDX11Device = new Device_DirectX11();
DescriptorHeapMap Render::mdhMapNames;



Render& Render::Initialize(HWND handle, const int w, const int h)
{
	static Render* instance = new Render();
	CreateRender(handle, w, h);
	return *instance;
}

void Render::InitializeD3D()
{
	mDX12Device->CreateDebugAndFactory();
	mDX12Device->CreateCommandObjects();
	mDX12Device->CreateSwapChain();
	//swap chain DH
	mDX12Device->CreateDescriptorHeap(DescriptorHeap::RTV, 2, DescriptorHeapFlag::None, mdhMapNames.renderTargetView);
	mDX12Device->CreateDescriptorHeap(DescriptorHeap::DSV, 1, DescriptorHeapFlag::None, mdhMapNames.depthStencilView);

}

void Render::OnResize()
{
	
	//Device()->GetNewWindowSize()
	Device()->FlushCommandQueue();
	Device()->PrepareCommandList();
	Device()->ReleaseAndResizeSwapChain();
	Device()->CreateRenderTargetView();

}

void Render::DrawEmptyScreen()
{
	Device()->PrepareCommandList();
	Device()->CloseCommandList();
	Device()->FlushCommandQueue();
}

Device* Render::Device()
{
#ifdef _DIRECTX12	
	return mDX12Device;
#endif //_DIRECTX12
#ifdef _DIRECTX11	

#endif //_DIRECTX11
}

void Render::CreateRender(HWND handle, const int w, const int h)
{
	mDX12Device->SetWindowParams(handle, w, h);
}




