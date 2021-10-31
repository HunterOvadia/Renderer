#include "App.h"

App::App() : Window(800, 600, "DXRenderer")
{
	
}

int App::PumpMessages()
{
	MSG Message;
	BOOL Result;
	while ((Result = GetMessage(&Message, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
		Update();
	}

	if (Result == -1)
	{
		throw DXWND_LAST_EXCEPT();
	}

	return Message.wParam;
}

void App::Update()
{
}
