#include "DXGIInfoManager.h"
#include "DXWindow.h"
#include "DXGraphics.h"
#include <dxgidebug.h>
#include <memory>

#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw DXGraphics::HrException(__LINE__, __FILE__, hr)

DXGIInfoManager::DXGIInfoManager()
{
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);
	const HMODULE hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		throw DXWND_LAST_EXCEPT();
	}

	const DXGIGetDebugInterface DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));
	if (DxgiGetDebugInterface == nullptr)
	{
		throw DXWND_LAST_EXCEPT();
	}

	HRESULT hr;
	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &DXGIInfoQueue));
}


void DXGIInfoManager::Set()
{
	Next = DXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DXGIInfoManager::GetMessages() const
{
	std::vector<std::string> Messages;
	const auto End = DXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = Next; i < End; i++)
	{
		HRESULT hr;
		SIZE_T MessageLength;
		GFX_THROW_NOINFO(DXGIInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &MessageLength));

		std::unique_ptr<byte[]> Bytes = std::make_unique<byte[]>(MessageLength);
		DXGI_INFO_QUEUE_MESSAGE* Message = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(Bytes.get());

		GFX_THROW_NOINFO(DXGIInfoQueue->GetMessageA(DXGI_DEBUG_ALL, i, Message, &MessageLength));
		Messages.emplace_back(Message->pDescription);
	}

	return Messages;
}

