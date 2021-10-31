#pragma once
#include "DXCore.h"
#include "DXException.h"
#include <d3d11.h>
#include <wrl.h>
#include "DXGIInfoManager.h"

class DXGraphics
{
public:
	class Exception : public DXException
	{
		using DXException::DXException;
	};

	class HrException : public Exception
	{
	public:
		HrException(int Line, const char* File, HRESULT Result, std::vector<std::string> InfoMessages = {});
		const char* what() const override;
		virtual const char* GetType() const override;
		HRESULT GetErrorCode() const;
		std::string GetErrorString() const;
		std::string GetErrorDescription() const;
		std::string GetErrorInfo() const;

	private:
		HRESULT Result;
		std::string Info;
	};

	class InfoException : public Exception
	{
	public:
		InfoException(int Line, const char* File, std::vector<std::string> InfoMessages);
		const char* what() const override;
		virtual const char* GetType() const override;
		std::string GetErrorInfo() const;

	private:
		std::string Info;
	};


	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;

	public:
		const char* GetType() const override;
	private:
		std::string Reason;
	};

public:
	DXGraphics(HWND WindowHandle);
	DXGraphics(const DXGraphics&) = delete;
	DXGraphics& operator=(const DXGraphics&) = delete;
	~DXGraphics() = default;

	void EndFrame();
	void ClearBuffer(float Red, float Green, float Blue);
	void DrawTestTriangle();

private:
#ifndef NDEBUG
	DXGIInfoManager InfoManager;
#endif

private:
	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> Target;
};

