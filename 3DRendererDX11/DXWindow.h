#pragma once
#include "DXCore.h"
#include "DXException.h"
#include "DXKeyboard.h"
#include "DXMouse.h"
#include "DXGraphics.h"
#include <optional>
#include <memory>

class DXWindow
{
public:
	class Exception : public DXException
	{
		using DXException::DXException;

	public:
		static std::string TranslateErrorCode(HRESULT Result);
	};

	class HrException : public Exception
	{
	public:
		HrException(int Line, const char* File, HRESULT Result);
		const char* what() const override;
		virtual const char* GetType() const override;
		HRESULT GetErrorCode() const;
		std::string GetErrorDescription() const;

	private:
		HRESULT Result;
	};


	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		virtual const char* GetType() const override;
	};


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
	void SetTitle(const std::string& Title);
	static std::optional<int> ProcessMessages();
	DXGraphics& GetGraphics();

private:
	static LRESULT CALLBACK HandleMessageSetup(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
	static LRESULT CALLBACK HandleMessageThunk(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
	LRESULT CALLBACK HandleMessage(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);

public:
	DXKeyboard Keyboard;
	DXMouse Mouse;

private:
	int Width;
	int Height;
	HWND WindowHandle;
	std::unique_ptr<DXGraphics> Graphics;
};

#define DXWND_EXCEPTION(Handle) DXWindow::HrException(__LINE__, __FILE__, Handle)
#define DXWND_LAST_EXCEPT() DXWindow::HrException(__LINE__, __FILE__, GetLastError())
#define DXWND_NOGFX_EXCEPT() DXWindow::NoGfxException(__LINE__, __FILE__)