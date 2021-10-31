#pragma once
#include <Windows.h>

class DXWindow
{
public:

private:
	class DXWindowClass
	{
		public:
			static const char* GetName();
			static HINSTANCE GetInstance();

		private:
			DXWindowClass();
			~DXWindowClass();
			DXWindowClass(const DXWindowClass&) = delete;
			DXWindowClass& operator=(const DXWindowClass&) = delete;

		private:
			static constexpr const char* WindowClassName = "DXRendererWindow";
			static DXWindowClass WindowClass;
			HINSTANCE WindowInstance;
	};

public:
	DXWindow(int Width, int Height, const char* Title);
	~DXWindow();
	DXWindow(const DXWindow&) = delete;
	DXWindow& operator=(const DXWindow&) = delete;

private:
	static LRESULT CALLBACK HandleMessageSetup(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
	static LRESULT CALLBACK HandleMessageThunk(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
	LRESULT CALLBACK HandleMessage(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);

private:
	int Width;
	int Height;
	HWND WindowHandle;
};