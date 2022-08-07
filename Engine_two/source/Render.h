#pragma once
#include "Resourse.h"
#include "Win32.h"
#include "dx11.h"
#include "dx12.h"


class Render
{
public:
	Render(HWND handle, const int w, const int h);
	void InitializeD3D();
private:
	
	Device_DirectX12 mDX12Device;
	Device_DirectX11 mDX11Device;

};

