#pragma once
#include "Utility.h"
#include <cassert>
#include <memory>
#include <string>
class Win32
{
public:
	Win32(HINSTANCE hInstance);
	Win32 (const Win32& o) = delete;
	Win32& operator=(const Win32& o) = delete;
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool InitializeMainWindow();
	void SetTimer(Timer& t);
	HWND GetMainWindow() const;
	void SetWindowParams(int w, int h);
	void SetWindowTitle(const std::string& s) const;
	bool IsPaused() const;
public:
	static Win32* GetHandle();
	HINSTANCE mInstance{};
	HWND      mhMainWnd{};
	static Win32* mWindowInstance;
	int		mClientWidth{};
	int		mClientHeight{};
	bool	mAppPaused{ false };
	bool    mResizing{ false };
	bool	mMinimized{ false };
	bool	mMaximized{ false };
	Timer   mTimer;
#ifdef _DIRECTX12
	const std::wstring mMainWndCaption{ L"DirectX 12" };
#endif // _DIRECTX12
#ifdef _DIRECTX11
	const std::wstring mMainWndCaption{ L"DirectX 11" };
#endif // _DIRECTX11
};

