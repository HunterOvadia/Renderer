#include "App.h"

int CALLBACK WinMain(
	HINSTANCE Instance, 
	HINSTANCE PrevInstance, 
	LPSTR CmdLine, 
	int CmdShow)
{
	try 
	{
		App MyApp;
		return MyApp.PumpMessages();
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
