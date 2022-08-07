#include "Device.h"

void Device::SetWindowParams(HWND handle, const int w, const int h)
{
	hWnd = handle;
	width = w;
	heigth = h;
}
