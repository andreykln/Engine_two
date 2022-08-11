#include "Application.h"

Application *Application::instance = nullptr;

Application::Application(HINSTANCE hInstance) : mhAppInst(hInstance)
{
	assert(instance == nullptr);
	instance = this;
	pWin32Handle = std::make_unique<Win32>(hInstance);
	pWin32Handle->SetTimer(mTimer);
	pWin32Handle->SetWindowParams(mWidth, mHeigth);

}

bool Application::Initialize()
{
	if(!InitializeMainWindow())
		return false;
	mhMainWnd = pWin32Handle->GetMainWindow();
	Render::Initialize(mhMainWnd, mWidth, mHeigth);
	Render::InitializeD3D();
	//initial resize run
	Render::OnResize();

	return true;
}

int Application::Run()
{
	MSG msg{};
	mTimer.Reset();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mTimer.Tick();
			if (!pWin32Handle->IsPaused())
			{
				std::string t = std::to_string(mTimer.TotalTime());
				pWin32Handle->SetWindowTitle(t);
				DoFrame();
			}
			else
			{
				//called when the D3D app is not in the focus
				Sleep(100);
			}
		}
	}
	return static_cast<int>(msg.wParam);
}

Application* Application::GetApplication() const
{
	return instance;
}

void Application::DoFrame()
{
	Render::DrawEmptyScreen();
}



bool Application::InitializeMainWindow() const
{
	return pWin32Handle->InitializeMainWindow();
}
