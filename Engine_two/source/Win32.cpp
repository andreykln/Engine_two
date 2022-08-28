#include "Win32.h"

Win32* Win32::mWindowInstance = nullptr;
std::pair<LONG, LONG> Win32::mMousePosition;

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return Win32::GetHandle()->MsgProc(hwnd, msg, wParam, lParam);
}

void Win32::OnMouseMove(WPARAM state, int x, int y)
{

	if(GetCursorPos(&mNewMousePosition))
	ScreenToClient(mhMainWnd, &mNewMousePosition);
	else
	{
#ifdef _DEBUG
		spdlog::error("Error in getting cursor position");
#endif // _DEBUG
	}
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
		Render::GetNewWindowSize(mClientWidth, mClientHeight);
		if (Render::DeviceInitialized())
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
				Render::OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					Render::OnResize();
				}
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					Render::OnResize();
				}
				else if (mResizing)
				{
					//this block kept empty in order to prevent
					//continuos swap chain creating during resizing.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					Render::OnResize();
				}
			}
		}
		return DEF_WIND_PROC;
	}
	case WM_MOUSEMOVE:
	{
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return DEF_WIND_PROC;
	}
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return DEF_WIND_PROC;
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		Render::OnResize();
		return 0;
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

	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	const int width = R.right - R.left;
	const int height = R.bottom - R.top;
	mhMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mInstance, 0);
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

void Win32::SetWindowTitle(const std::string& s) const
{
	
	SetWindowTextA(mhMainWnd, s.c_str());
}

bool Win32::IsPaused() const
{
	return mAppPaused;
}


std::pair<LONG, LONG> Win32::GetMousePosition()
{
	mMousePosition.first = mNewMousePosition.x;
	mMousePosition.second = mNewMousePosition.y;

	return mMousePosition;
}


