#include "DXWindow.h"
#include <sstream>

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

void DXWindow::SetTitle(const std::string& Title)
{
	if (SetWindowText(WindowHandle, Title.c_str()) == 0)
	{
		throw DXWND_LAST_EXCEPT();
	}
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
	this->Width = Width;
	this->Height = Height;

	RECT WindowRect;
	WindowRect.left = 100;
	WindowRect.right = Width + WindowRect.left;
	WindowRect.top = 100;
	WindowRect.bottom = Height + WindowRect.top;
	if (AdjustWindowRect(&WindowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw DXWND_LAST_EXCEPT();
	}

	WindowHandle = CreateWindow(DXWindowClass::GetName(), Title,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		nullptr, nullptr, DXWindowClass::GetInstance(), this);

	if (WindowHandle == nullptr)
	{
		throw DXWND_LAST_EXCEPT();
	}

	ShowWindow(WindowHandle, SW_SHOWDEFAULT);
}

DXWindow::~DXWindow()
{
	DestroyWindow(WindowHandle);
}

LRESULT CALLBACK DXWindow::HandleMessageSetup(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
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

LRESULT CALLBACK DXWindow::HandleMessageThunk(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
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

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			if (!(LParam & 0x40000000) || Keyboard.IsAutorepeatEnabled())
			{
				Keyboard.OnKeyPressed(static_cast<unsigned char>(WParam));
			}
			break;
		}

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			Keyboard.OnKeyReleased(static_cast<unsigned char>(WParam));
			break;
		}

		case WM_CHAR:
		{
			Keyboard.OnChar(static_cast<unsigned char>(WParam));
			break;
		}

		case WM_KILLFOCUS:
		{
			Keyboard.ClearState();
			break;
		}

		case WM_MOUSEMOVE:
		{
			const POINTS Point = MAKEPOINTS(LParam);
			if (Point.x >= 0 && Point.x < Width && Point.y >= 0 && Point.y < Height)
			{
				Mouse.OnMouseMove(Point.x, Point.y);
				if (!Mouse.IsInWindow())
				{
					SetCapture(WindowHandle);
					Mouse.OnMouseEnter();
				}
			}
			else
			{
				if (WParam & (MK_LBUTTON | MK_RBUTTON))
				{
					Mouse.OnMouseMove(Point.x, Point.y);
				}
				else
				{
					ReleaseCapture();
					Mouse.OnMouseExit();
				}
			}
			break;
		}

		case WM_LBUTTONDOWN:
		{
			const POINTS Point = MAKEPOINTS(LParam);
			Mouse.OnLeftPressed(Point.x, Point.y);
			break;
		}

		case WM_RBUTTONDOWN:
		{
			const POINTS Point = MAKEPOINTS(LParam);
			Mouse.OnRightPressed(Point.x, Point.y);
			break;
		}

		case WM_LBUTTONUP:
		{
			const POINTS Point = MAKEPOINTS(LParam);
			Mouse.OnLeftReleased(Point.x, Point.y);
			if (Point.x < 0 || Point.x >= Width || Point.y < 0 || Point.y >= Height)
			{
				ReleaseCapture();
				Mouse.OnMouseExit();
			}
			break;
		}

		case WM_RBUTTONUP:
		{
			const POINTS Point = MAKEPOINTS(LParam);
			Mouse.OnRightReleased(Point.x, Point.y);
			if (Point.x < 0 || Point.x >= Width || Point.y < 0 || Point.y >= Height)
			{
				ReleaseCapture();
				Mouse.OnMouseExit();
			}
			break;
		}

		case WM_MOUSEWHEEL:
		{
			const POINTS Point = MAKEPOINTS(LParam);
			const int Delta = GET_WHEEL_DELTA_WPARAM(WParam);
			Mouse.OnWheelDelta(Point.x, Point.y, Delta);
			break;
		}
	}

	return DefWindowProc(WindowHandle, Message, WParam, LParam);
}

DXWindow::DXWindowException::DXWindowException(int Line, const char* File, HRESULT Result) :
	DXException(Line, File)
{
	this->Result = Result;
}

const char* DXWindow::DXWindowException::what() const
{
	std::ostringstream StringStream;
	StringStream << GetType() << std::endl
		<< "[Error Code]: " << GetErrorCode() << std::endl
		<< "[Description]: " << GetErrorString() << std::endl
		<< GetOriginString();
	WhatBuffer = StringStream.str();
	return WhatBuffer.c_str();
}

const char* DXWindow::DXWindowException::GetType() const
{
	return "DXWindow Exception";
}

std::string DXWindow::DXWindowException::TranslateErrorCode(HRESULT Result)
{
	char* MessageBuffer = nullptr;
	DWORD MessageLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
										nullptr, Result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
										reinterpret_cast<LPSTR>(&MessageBuffer), 0, nullptr);
	if (MessageLength == 0)
	{
		return "Unidentified Error";
	}

	std::string ErrorString = MessageBuffer;
	LocalFree(MessageBuffer);
	return ErrorString;
}

HRESULT DXWindow::DXWindowException::GetErrorCode() const
{
	return Result;
}

std::string DXWindow::DXWindowException::GetErrorString() const
{
	return TranslateErrorCode(Result);
}
