#pragma once
#include "Win32.h"
class Application
{
public:
	Application(HINSTANCE hInstance);
	Application(const Application& o) = delete;
	Application& operator=(const Application& o) = delete;
	bool Initialize();
	int Run();
private:
	void DoFrame();
	bool InitializeMainWindow() const;
	std::unique_ptr<Win32> pWin32Handle;
	Timer mTimer;
	static Application* instance;
	HINSTANCE mhAppInst{ nullptr };
	HWND      mhMainWnd;
	int mWidth = 640;
	int mHeigth = 480;


	//bool    mFullscreenState{ false };
	bool    m4xMsaaState{ false };
	UINT    m4xMsaaQuality{ 0 };

};

