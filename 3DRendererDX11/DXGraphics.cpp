#include "DXGraphics.h"
#include "DirectXError.h"
#include <sstream>
#include <d3dcompiler.h>
#include <cmath>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#define GFX_EXCEPT_NOINFO(hr) DXGraphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw DXGraphics::HrException(__LINE__, __FILE__, hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) DXGraphics::HrException(__LINE__, __FILE__, (hr), InfoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) InfoManager.Set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DXGraphics::DeviceRemovedException(__LINE__, __FILE__, (hr), InfoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) InfoManager.Set(); (call); {auto v = InfoManager.GetMessages(); if(!v.empty()) {throw DXGraphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) DXGraphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DXGraphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(hrcall) (hrcall)
#endif

DXGraphics::DXGraphics(HWND WindowHandle)
{
	// NOTE(HO): Initialize the Device and SwapChain
	DXGI_SWAP_CHAIN_DESC SwapChainDesc = { 0 };
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow = WindowHandle;
	SwapChainDesc.Windowed = TRUE;
	SwapChainDesc.BufferCount = 1;
	//SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	UINT SwapCreateFlags = 0u;
#ifndef NDEBUG
	SwapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr;

	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		SwapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&SwapChainDesc,
		&SwapChain,
		&Device,
		nullptr,
		&Context));

	Microsoft::WRL::ComPtr<ID3D11Resource> BackBuffer;
	GFX_THROW_INFO(SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &BackBuffer));
	GFX_THROW_INFO(Device->CreateRenderTargetView(BackBuffer.Get(), nullptr, &Target));

	// NOTE(HO): Create Depth Stencil/Buffer
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = { 0 };
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;
	GFX_THROW_INFO(Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState));

	Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilTex;
	D3D11_TEXTURE2D_DESC DepthStencilTexDesc = { 0 };
	DepthStencilTexDesc.Width = 800u;
	DepthStencilTexDesc.Height = 600u;
	DepthStencilTexDesc.MipLevels = 1u;
	DepthStencilTexDesc.ArraySize = 1u;
	DepthStencilTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthStencilTexDesc.SampleDesc.Count = 1;
	DepthStencilTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(Device->CreateTexture2D(&DepthStencilTexDesc, nullptr, &DepthStencilTex));

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
	DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;
	GFX_THROW_INFO(Device->CreateDepthStencilView(DepthStencilTex.Get(), &DepthStencilViewDesc, &DepthStencilView));

	Context->OMSetRenderTargets(1u, Target.GetAddressOf(), DepthStencilView.Get());
}

void DXGraphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	InfoManager.Set();
#endif
	if (FAILED(hr = SwapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(Device->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void DXGraphics::ClearBuffer(float Red, float Green, float Blue)
{
	const float Color[] = { Red, Green, Blue, 1.0f };
	if (Context != nullptr)
	{
		Context->ClearRenderTargetView(Target.Get(), Color);
		Context->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}
}

void DXGraphics::DrawTestTriangle(float Angle, float X, float Z)
{
	HRESULT hr;

	struct Vertex 
	{
		struct
		{
			float X;
			float Y;
			float Z;
		} Position;
	};

	// NOTE(HO): Vertex Buffer
	Vertex Vertices[] =
	{
		{ -1.0f,-1.0f,-1.0f	 },
		{ 1.0f,-1.0f,-1.0f	 },
		{ -1.0f,1.0f,-1.0f	 },
		{ 1.0f,1.0f,-1.0f	  },
		{ -1.0f,-1.0f,1.0f	 },
		{ 1.0f,-1.0f,1.0f	  },
		{ -1.0f,1.0f,1.0f	 },
		{ 1.0f,1.0f,1.0f	 },
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	D3D11_BUFFER_DESC VertBufferDesc = { 0 };
	VertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertBufferDesc.ByteWidth = sizeof(Vertices);
	VertBufferDesc.StructureByteStride = sizeof(Vertex);
	
	D3D11_SUBRESOURCE_DATA VertData = { 0 };
	VertData.pSysMem = Vertices;

	GFX_THROW_INFO(Device->CreateBuffer(&VertBufferDesc, &VertData, &VertexBuffer));

	const UINT Stride = sizeof(Vertex);
	const UINT Offset = 0u;
	Context->IASetVertexBuffers(0u, 1u, VertexBuffer.GetAddressOf(), &Stride, &Offset);

	// NOTE(HO): Index Buffer
	const unsigned short Indices[] =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
	D3D11_BUFFER_DESC IndexBufferDesc = { 0 };
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.ByteWidth = sizeof(Indices);
	IndexBufferDesc.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA IndexData = { 0 };
	IndexData.pSysMem = Indices;
	
	GFX_THROW_INFO(Device->CreateBuffer(&IndexBufferDesc, &IndexData, &IndexBuffer));
	Context->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// NOTE(HO): Constant Buffer
	struct ConstantBufferData
	{
		DirectX::XMMATRIX Transform;
	};

	const ConstantBufferData Buffers =
	{
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixRotationX(Angle) * 
				DirectX::XMMatrixTranslation(X, 0.0f, Z + 4.0f) * 
				DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f))
		}
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
	D3D11_BUFFER_DESC ConstantBufferDesc = { 0 };
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBufferDesc.ByteWidth = sizeof(Buffers);

	D3D11_SUBRESOURCE_DATA ConstantBufferData = { 0 };
	ConstantBufferData.pSysMem = &Buffers;

	GFX_THROW_INFO(Device->CreateBuffer(&ConstantBufferDesc, &ConstantBufferData, &ConstantBuffer));
	Context->VSSetConstantBuffers(0u, 1u, ConstantBuffer.GetAddressOf());

	struct ConstantBuffer2
	{
		struct
		{
			float Red, Green, Blue, Alpha;
		} face_colors[6];
	};

	const ConstantBuffer2 Buffers2 =
	{
		{
			{1.0f,0.0f,1.0f},
			{1.0f,0.0f,0.0f},
			{0.0f,1.0f,0.0f},
			{0.0f,0.0f,1.0f},
			{1.0f,1.0f,0.0f},
			{0.0f,1.0f,1.0f},
		}
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer2;
	D3D11_BUFFER_DESC ConstantBufferDesc2 = { 0 };
	ConstantBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc2.ByteWidth = sizeof(Buffers2);

	D3D11_SUBRESOURCE_DATA ConstantBufferData2 = { 0 };
	ConstantBufferData2.pSysMem = &Buffers2;

	GFX_THROW_INFO(Device->CreateBuffer(&ConstantBufferDesc2, &ConstantBufferData2, &ConstantBuffer2));
	Context->PSSetConstantBuffers(0u, 1u, ConstantBuffer2.GetAddressOf());

	// NOTE(HO): Pixel Shader
	Microsoft::WRL::ComPtr<ID3DBlob> Blob;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &Blob));
	GFX_THROW_INFO(Device->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), nullptr, &PixelShader));
	Context->PSSetShader(PixelShader.Get(), nullptr, 0u);

	// NOTE(HO): Vertex Shader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &Blob));
	GFX_THROW_INFO(Device->CreateVertexShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), nullptr, &VertexShader));
	Context->VSSetShader(VertexShader.Get(), nullptr, 0u);

	// NOTE(HO): Vertex Layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
	const D3D11_INPUT_ELEMENT_DESC InputElementDesc[] =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	GFX_THROW_INFO(Device->CreateInputLayout(InputElementDesc, (sizeof(InputElementDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC)), Blob->GetBufferPointer(), Blob->GetBufferSize(), &InputLayout));
	Context->IASetInputLayout(InputLayout.Get());
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT Viewport = { 0 };
	Viewport.Width = 800;
	Viewport.Height = 600;
	Viewport.MaxDepth = 1;

	Context->RSSetViewports(1u, &Viewport);
	GFX_THROW_INFO_ONLY(Context->DrawIndexed((sizeof(Indices) / sizeof(unsigned short)), 0u, 0u));
}

DXGraphics::HrException::HrException(int Line, const char* File, HRESULT Result, std::vector<std::string> InfoMessages)
	: Exception(Line, File)
{
	this->Result = Result;
	for (const std::string& Message : InfoMessages)
	{
		Info += Message;
		Info.push_back('\n');
	}

	// Remove final new line
	if (!Info.empty())
	{
		Info.pop_back();
	}
}

const char* DXGraphics::HrException::what() const
{
	std::ostringstream StringStream;

	StringStream << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;

	if (!Info.empty())
	{
		StringStream << "\n[Error Info]:\n" << GetErrorInfo() << std::endl << std::endl;
	}

	StringStream << GetOriginString();
	WhatBuffer = StringStream.str();
	return WhatBuffer.c_str();
}

const char* DXGraphics::HrException::GetType() const
{
	return "DX Graphics Exception";
}

HRESULT DXGraphics::HrException::GetErrorCode() const
{
	return Result;
}

std::string DXGraphics::HrException::GetErrorString() const
{
	return DXGetErrorString(Result);
}

std::string DXGraphics::HrException::GetErrorDescription() const
{
	char Buffer[512];
	DXGetErrorDescription(Result, Buffer, sizeof(Buffer));
	return Buffer;
}

std::string DXGraphics::HrException::GetErrorInfo() const
{
	return Info;
}

const char* DXGraphics::DeviceRemovedException::GetType() const
{
	return "DX Graphics Exception (DXGI_ERROR_DEVICE_REMOVED)";
}

DXGraphics::InfoException::InfoException(int Line, const char* File, std::vector<std::string> InfoMessages)
	: Exception(Line, File)
{
	for (const std::string& Message : InfoMessages)
	{
		Info += Message;
		Info.push_back('\n');
	}

	// Remove last empty line
	if (!Info.empty())
	{
		Info.pop_back();
	}
}

const char* DXGraphics::InfoException::what() const
{
	std::ostringstream StringStream;

	StringStream << GetType() << std::endl << "\n[Error Info]:\n" << GetErrorInfo() << std::endl << std::endl;
	StringStream << GetOriginString();

	WhatBuffer = StringStream.str();
	return WhatBuffer.c_str();
}

const char* DXGraphics::InfoException::GetType() const
{
	return "DX Graphics Info Exception";
}

std::string DXGraphics::InfoException::GetErrorInfo() const
{
	return Info;
}
