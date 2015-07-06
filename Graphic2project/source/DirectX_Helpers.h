#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

//Funtion headers

//Input is only for the Buffer Count, Buffer Width, Buffer Height, and Window to render to
//Output is the first parameter for a DXGI_SWAP_CHAIN_DESC
void CreateSwapChainDesc(DXGI_SWAP_CHAIN_DESC *m_scDesc, HWND* window, int bufferCount = 1, int bufferWidth = 500, int bufferHeight = 500);

//You only need to insert the first 5 parameters
HRESULT DefaultDeviceAndSwapChain(unsigned int deviceFlag,  DXGI_SWAP_CHAIN_DESC* m_scDesc, IDXGISwapChain** m_snSwapChain, ID3D11Device** m_iDevice, ID3D11DeviceContext** m_dcConext, IDXGIAdapter* adapter = nullptr, D3D_DRIVER_TYPE _type = D3D_DRIVER_TYPE_HARDWARE, HMODULE* _software = nullptr, D3D_FEATURE_LEVEL* FeatureLevel = nullptr, unsigned int LevelCount = 0, unsigned int version = D3D10_SDK_VERSION, D3D_FEATURE_LEVEL* output = nullptr);

//Function Bodies
void CreateSwapChainDesc(DXGI_SWAP_CHAIN_DESC *m_scDesc, HWND* window, int bufferCount, int bufferWidth, int bufferHeight)
{
	ZeroMemory(m_scDesc, sizeof(*m_scDesc));
	m_scDesc->BufferCount = bufferCount;
	m_scDesc->BufferDesc.Width = bufferWidth;
	m_scDesc->BufferDesc.Height = bufferHeight;
	m_scDesc->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_scDesc->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	m_scDesc->BufferDesc.RefreshRate.Numerator = 60;
	m_scDesc->BufferDesc.RefreshRate.Denominator = 1;
	m_scDesc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_scDesc->OutputWindow = *window;
	m_scDesc->SampleDesc.Count = 1;
	m_scDesc->SampleDesc.Quality = 0;
	m_scDesc->Windowed = true;
}

//You only need to insert the first 5 parameters
HRESULT DefaultDeviceAndSwapChain(unsigned int deviceFlag, DXGI_SWAP_CHAIN_DESC* m_scDesc, IDXGISwapChain** m_snSwapChain, ID3D11Device** m_iDevice, ID3D11DeviceContext** m_dcConext, IDXGIAdapter * adapter, D3D_DRIVER_TYPE _type, HMODULE* _software, D3D_FEATURE_LEVEL* FeatureLevel, unsigned int LevelCount, unsigned int version, D3D_FEATURE_LEVEL* output)
{
	HRESULT hr;
	if (FeatureLevel != nullptr || adapter != nullptr)
	{
		hr = D3D11CreateDeviceAndSwapChain(adapter, _type, nullptr, deviceFlag, FeatureLevel, LevelCount, version, m_scDesc, m_snSwapChain, m_iDevice, NULL, m_dcConext);
	}
	else
	{
		hr = D3D11CreateDeviceAndSwapChain(nullptr, _type, nullptr, deviceFlag, NULL, 0, D3D11_SDK_VERSION, m_scDesc, m_snSwapChain, m_iDevice, NULL, m_dcConext);
	}

	return hr;
}

void DefaultBlendDesc(D3D11_BLEND_DESC* BlendDesc)
{

	ZeroMemory(BlendDesc, sizeof(*BlendDesc));
	BlendDesc->RenderTarget[0].BlendEnable = true;
	BlendDesc->RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc->RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc->RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

}

void DefaultDepthDesc(D3D11_DEPTH_STENCIL_DESC* depthStencilDesc)
{
	ZeroMemory(depthStencilDesc, sizeof(*depthStencilDesc));

	depthStencilDesc->DepthEnable = true;
	depthStencilDesc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc->DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc->StencilEnable = true;
	depthStencilDesc->StencilReadMask = 0xFF;
	depthStencilDesc->StencilWriteMask = 0xFF;

	depthStencilDesc->FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc->FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc->FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc->FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc->BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc->BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc->BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc->BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
}

void DefaultRasterizerDesc(D3D11_RASTERIZER_DESC* rasterizerState)
{
	ZeroMemory(rasterizerState, sizeof(*rasterizerState));
	rasterizerState->FillMode = D3D11_FILL_SOLID;
	rasterizerState->CullMode = D3D11_CULL_BACK;
	rasterizerState->FrontCounterClockwise = false;
	rasterizerState->AntialiasedLineEnable = true;
}