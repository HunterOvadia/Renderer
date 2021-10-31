#include <Windows.h>

LRESULT CALLBACK WndProc(
	HWND WindowHandle, 
	UINT Message, 
	WPARAM WParam, 
	LPARAM LParam)
{
	switch (Message)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}
	}

	return DefWindowProc(WindowHandle, Message, WParam, LParam);
}

int CALLBACK WinMain(
	HINSTANCE Instance, 
	HINSTANCE PrevInstance, 
	LPSTR CmdLine, 
	int CmdShow)
{
	// NOTE(HO): Register Window Class
	WNDCLASSEX WindowClass = { 0 };
	WindowClass.cbSize = sizeof(WindowClass);
	WindowClass.style = CS_OWNDC;
	WindowClass.lpfnWndProc = WndProc;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = L"3DRendererDX11";
	RegisterClassEx(&WindowClass);
	
	// NOTE(HO): Create and Show Window
	HWND WindowHandle = CreateWindowEx(0, 
		WindowClass.lpszClassName, 
		L"Rendering Window", 
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 
		200, 200, 640, 480, 
		nullptr, nullptr, Instance, nullptr);

	ShowWindow(WindowHandle, SW_SHOW);
	
	// NOTE(HO): Message Pump
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
	else
	{
		return Message.wParam;
	}
}