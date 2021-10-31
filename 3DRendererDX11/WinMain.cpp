#include <Windows.h>
#include "DXWindow.h"

int CALLBACK WinMain(
	HINSTANCE Instance, 
	HINSTANCE PrevInstance, 
	LPSTR CmdLine, 
	int CmdShow)
{
	DXWindow Window(800, 300, "DXRenderer");
	
	MSG Message;
	BOOL Result;
	while ((Result = GetMessage(&Message, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	if (Result == -1)
	{
		return -1;
	}

	return Message.wParam;
}
