#pragma once
#include "Resourse.h"
#include "Win32.h"
#include "dx11.h"
#include "dx12.h"


class Render
{
public:
	static Render& Initialize(HWND handle, const int w, const int h);
	static void InitializeD3D();
	static void OnResize();
	static void DrawEmptyScreen();

private:
	Render() {};
	static Device* Device();
	static void CreateRender(HWND handle, const int w, const int h);
	static Device_DirectX12* mDX12Device;
	static Device_DirectX11* mDX11Device;
	static DescriptorHeapMap mdhMapNames;
};

