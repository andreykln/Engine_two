#include "Application.h"

int APIENTRY wWinMain(_In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     LPWSTR    lpCmdLine,
    _In_     int       nCmdShow)
{

	try
	{
		Application app(hInstance);
		if (!app.Initialize())
		{
			MessageBox(nullptr, L"App failed to initialize", L"HR Failed", MB_OK);
			return 0;
		}
			
		app.Run();
	}
	catch (const DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}

	return 0;
}