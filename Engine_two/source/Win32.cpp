#include "Win32.h"

Win32* Win32::mWindowInstance = nullptr;

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return Win32::GetHandle()->MsgProc(hwnd, msg, wParam, lParam);
}

Win32* Win32::GetHandle()
{
	return mWindowInstance;
}

Win32::Win32(HINSTANCE hInstance) : mInstance(hInstance)
{
    assert(mWindowInstance == nullptr);
    mWindowInstance = this;
}

LRESULT Win32::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#define DEF_WIND_PROC ::DefWindowProc(hwnd, msg, wParam, lParam);
	switch (msg)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return DEF_WIND_PROC;
	}
	case WM_SIZE:
	{
		mClientWidth = LOWORD(lParam);
		mClientHeight = LOWORD(lParam);
		//TODO Resize fucntion is used here
		if (true)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				//Render::OnResize();
				//OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					//OnResize();
				}

				// Restoring from maximized state?
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					//OnResize();
				}
				else if (mResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					//OnResize();
				}
			}
		}
		return DEF_WIND_PROC;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return DEF_WIND_PROC;
	default:
		break;
	}

	return DEF_WIND_PROC;
}

bool Win32::InitializeMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}
	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	const int width = R.right - R.left;
	const int height = R.bottom - R.top;
	//HWND t;
	mhMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mInstance, 0);
	//mhMainWnd = &t;
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);
#ifdef _DEBUG
	AllocConsole();
#endif // _DEBUG
	return true;
}

void Win32::SetTimer(Timer& t)
{
	mTimer = t;
}

HWND Win32::GetMainWindow() const
{
	return mhMainWnd;
}

void Win32::SetWindowParams(int w, int h)
{
	mClientWidth = w;
	mClientHeight = h;
}

std::pair<int, int> Win32::GetNewWindowParams() const
{
	return std::pair<int, int>(mClientWidth, mClientHeight);
}

void Win32::SetWindowTitle(const std::string& s) const
{
	
	SetWindowTextA(mhMainWnd, s.c_str());
}

bool Win32::IsPaused() const
{
	return mAppPaused;
}


