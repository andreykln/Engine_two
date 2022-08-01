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
	void SetTimer(Timer* t);
	void SetWindowTitle(const std::string& s) const;
	bool IsPaused() const;
public:
	static Win32* GetHandle();
	HINSTANCE mInstance{};
	HWND      mhMainWnd{ nullptr };
	static Win32* mWindowInstance;
	int		mClientWidth{ 1280 };
	int		mClientHeight{ 720 };
	bool	mAppPaused{ false };
	bool    mResizing{ false };
	bool	mMinimized{ false };
	bool	mMaximized{ false };
	Timer* pTimer{};
#ifdef _DIRECTX12
	const std::wstring mMainWndCaption{ L"DirectX 12" };
#endif // _DIRECTX12
#ifdef _DIRECTX11
	const std::wstring mMainWndCaption{ L"DirectX 11" };
#endif // _DIRECTX11
};

