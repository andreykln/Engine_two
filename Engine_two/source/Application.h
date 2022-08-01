#pragma once
#include "Render.h"
class Application
{
public:
	Application(HINSTANCE hInstance);
	Application(const Application& o) = delete;
	Application& operator=(const Application& o) = delete;
	bool Initialize() const;
	int Run() const;
	Application* GetApplication() const;
private:
	bool InitializeMainWindow() const;

	std::unique_ptr<Win32> pWin32Handle;
	Timer* pTimer;
	static Application* instance;
	HINSTANCE mhAppInst{ nullptr };
	HWND      mhMainWnd{ nullptr };



	bool    mFullscreenState{ false };
	bool    m4xMsaaState{ false };
	UINT    m4xMsaaQuality{ 0 };

};

