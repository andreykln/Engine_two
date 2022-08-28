#pragma once
#include "Utility.h"
#include "Render.h"
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
	const bool IsPaused() const;
	std::pair<LONG, LONG> GetMousePosition();
public:
	void OnMouseMove(WPARAM state, int x, int y);
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
	Timer   mTimer{};
	POINT mNewMousePosition{};
	static std::pair<LONG, LONG> mMousePosition;
	//TODO this is overwritten by change window title
	//after adding iMGui make sure it works
#ifdef _DIRECTX12
	const std::wstring mMainWndCaption{ L"DirectX 12" };
#endif // _DIRECTX12
#ifdef _DIRECTX11
	const std::wstring mMainWndCaption{ L"DirectX 11" };
#endif // _DIRECTX11
};

