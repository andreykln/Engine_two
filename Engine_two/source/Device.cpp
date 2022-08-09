#include "Device.h"

void Device::SetWindowParams(HWND handle, const int w, const int h)
{
	hWnd = handle;
	mClientWidth = w;
	mClientHeight = h;
}

void Device::GetNewWindowSize(std::pair<int, int> p)
{
	mClientWidth = p.first;
	mClientHeight = p.second;
}
