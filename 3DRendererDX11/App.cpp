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
	const float Color = sin(Timer.Peek()) / 2.0f + 0.5f;
	Window.GetGraphics().ClearBuffer(Color, Color, 1.0f);
	Window.GetGraphics().DrawTestTriangle();
	Window.GetGraphics().EndFrame();
}
