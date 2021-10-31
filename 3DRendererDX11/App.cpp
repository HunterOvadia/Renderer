#include "App.h"

App::App() : Window(800, 600, "DXRenderer")
{
	
}

int App::PumpMessages()
{
	while (true)
	{
		if (const std::optional<int> ErrorCode = DXWindow::ProcessMessages())
		{
			return *ErrorCode;
		}

		Update();
	}
}

void App::Update()
{

}
