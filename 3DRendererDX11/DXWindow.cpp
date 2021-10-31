#include "DXWindow.h"

DXWindow::DXWindowClass DXWindow::DXWindowClass::WindowClass;


DXWindow::DXWindowClass::DXWindowClass()
	: WindowInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX WindowClass = { 0 };
	WindowClass.cbSize = sizeof(WindowClass);
	WindowClass.style = CS_OWNDC;
	WindowClass.lpfnWndProc = HandleMessageSetup;
	WindowClass.hInstance = GetInstance();
	WindowClass.lpszClassName = GetName();
	RegisterClassEx(&WindowClass);
}

DXWindow::DXWindowClass::~DXWindowClass()
{
	UnregisterClass(WindowClassName, GetInstance());
}

const char* DXWindow::DXWindowClass::GetName()
{
	return WindowClassName;
}

HINSTANCE DXWindow::DXWindowClass::GetInstance()
{
	return WindowClass.WindowInstance;
}

DXWindow::DXWindow(int Width, int Height, const char* Title)
{
	RECT WindowRect;
	WindowRect.left = 100;
	WindowRect.right = Width + WindowRect.left;
	WindowRect.top = 100;
	WindowRect.bottom = Height + WindowRect.top;
	AdjustWindowRect(&WindowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false);

	WindowHandle = CreateWindow(DXWindowClass::GetName(), Title,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		nullptr, nullptr, DXWindowClass::GetInstance(), this);

	ShowWindow(WindowHandle, SW_SHOWDEFAULT);
}

DXWindow::~DXWindow()
{
	DestroyWindow(WindowHandle);
}

LRESULT DXWindow::HandleMessageSetup(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
	switch (Message)
	{
		case WM_NCCREATE:
		{
			const CREATESTRUCTW* const Create = reinterpret_cast<CREATESTRUCTW*>(LParam);
			if (Create != nullptr)
			{
				DXWindow* const Window = static_cast<DXWindow*>(Create->lpCreateParams);
				if (Window != nullptr)
				{
					SetWindowLongPtr(WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(Window));
					SetWindowLongPtr(WindowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&DXWindow::HandleMessageThunk));
					return Window->HandleMessage(WindowHandle, Message, WParam, LParam);
				}
			}
			break;
		}
	}

	return DefWindowProc(WindowHandle, Message, WParam, LParam);
}

LRESULT DXWindow::HandleMessageThunk(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
	DXWindow* const Window = reinterpret_cast<DXWindow*>(GetWindowLongPtr(WindowHandle, GWLP_USERDATA));
	if (Window != nullptr)
	{
		return Window->HandleMessage(WindowHandle, Message, WParam, LParam);
	}
}

LRESULT DXWindow::HandleMessage(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
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

