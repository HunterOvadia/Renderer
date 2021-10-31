#include "DXWindow.h"

int CALLBACK WinMain(
	HINSTANCE Instance, 
	HINSTANCE PrevInstance, 
	LPSTR CmdLine, 
	int CmdShow)
{
	try 
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
			throw DXWND_LAST_EXCEPT();
		}

		return Message.wParam;
	}
	catch (const DXException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No Details", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

}
