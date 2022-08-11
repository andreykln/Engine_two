#include "Device.h"

void Device::SetWindowParams(HWND handle, const int w, const int h)
{
	hWnd = handle;
	mClientWidth = w;
	mClientHeight = h;
}

void Device::GetNewWindowSize(int w, int h)
{
	mClientWidth = w;
	mClientHeight = h;
}

