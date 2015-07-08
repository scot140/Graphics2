// D3D LAB 1a "Line Land".
// Author: L.Norri CD DRX, FullSail University

// Introduction:
// Welcome to the first lab of the Direct3D Graphics Programming class.
// This is the ONLY guided lab in this course! 
// Future labs will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

// TO BEGIN: Open the word document that acompanies this lab and start from the top.

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <iostream>
#include <ctime>
#include "XTime.h"
#include <Windows.h>
#include "Model.h"
#include "Cube.h"
#include "numbers_test.h"
//#include <atlbase.h>

using namespace std;


// BEGIN PART 1
// TODO: PART 1 STEP 1a
//#include <d3d11.h>
//#pragma comment(lib, "d3d11.lib")
// TODO: PART 1 STEP 1b
#include "DirectX_Helpers.h"
// TODO: PART 2 STEP 6
#include "../VertexShader.csh"
#include "../PixelShader.csh"

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
#define OBJECT				0
#define SCENE				1
#define TIMER				2

#define UP					0
#define DOWN				1
#define RIGHT				2
#define LEFT				3
//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************
IDXGISwapChain* g_snSwapChain;
ID3D11DepthStencilState * g_pDepthState;
ID3D11DepthStencilView* g_DepthView;
ID3D11RenderTargetView* g_rtvRenderTargetView;
ID3D11Texture2D* g_ZBuffer;
ID3D11Device* g_iDevice;
ID3D11DeviceContext* g_dcConext;
ID3D11Texture2D* g_pBackBuffer;
D3D11_VIEWPORT m_vpViewPort;

class DEMO_APP
{
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;

	// TODO: PART 1 STEP 2
	ID3D11Device* m_iDevice;
	IDXGISwapChain* m_snSwapChain;
	ID3D11DeviceContext* m_dcConext;
	ID3D11RenderTargetView* m_rtvRenderTargetView;
	ID3D11InputLayout* m_pVertexInput;
	ID3D11Texture2D* m_pBackBuffer;
	ID3D11RasterizerState* m_pRasterState;
	DXGI_SWAP_CHAIN_DESC m_scDesc;
	// TODO: PART 2 STEP 2
	// BEGIN PART 5
	// TODO: PART 5 STEP 1

	// TODO: PART 2 STEP 4
	ID3D11PixelShader*	m_shaderPS;
	ID3D11VertexShader* m_shaderVS;
	// BEGIN PART 3
	// TODO: PART 3 STEP 1


	//index buffer

	//depth perspective
	ID3D11Texture2D* m_ZBuffer;
	ID3D11DepthStencilView* m_DepthView;
	ID3D11DepthStencilState * m_pDepthState;



	//Blending
	ID3D11BlendState* m_pBlendState;
	ID3D11RasterizerState* m_pRasterStateFrontCull;

	// TODO: PART 3 STEP 2b
	

	// TODO: PART 3 STEP 4a

	//camera


	int CameraUpDown;

	float CameraX = 0, CameraY = 0, CameraZ = 3;

	void MouseMovement(bool& move, float dt);


	ANIMATION animate;
	float delta = 0;

	POINT startPoint;
	POINT CurrentPoint;
	bool LbuttonDown;

	unsigned int length;

public:

	// BEGIN PART 2
	// TODO: PART 2 STEP 1

	unsigned int maxVerts = 0;

	unsigned int maxIndices = 0;
	unsigned int maxGridIndices = 0;

	XMFLOAT4 Cube[776];
	XMFLOAT4 Star[22];
	XMFLOAT4 m_vtGrid_Verts[86];

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	int DistanceFormula(POINT LH, POINT RH);
	bool Run();
	bool ShutDown();
	XTime dt;
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{

#pragma region Beginning Initialization
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 

	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	//&~(WS_THICKFRAME | WS_MAXIMIZEBOX)

	window = CreateWindow(L"DirectXApplication", L"Lab 1a Line Land", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//
#pragma endregion 

	//const D3D_FEATURE_LEVEL Feature = D3D_FEATURE_LEVEL_10_0;

	// TODO: PART 1 STEP 3a
	ZeroMemory(&m_scDesc, sizeof(m_scDesc));
	CreateSwapChainDesc(&m_scDesc, &window, 1, 500, 500);

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#else
	createDeviceFlags |= D3D10_CREATE_DEVICE_SINGLETHREADED;
#endif

	HRESULT hr = DefaultDeviceAndSwapChain(createDeviceFlags, &m_scDesc, &m_snSwapChain, &m_iDevice, &m_dcConext);

	if (hr == E_INVALIDARG)
	{
		return;
	}


	//setting the buffer

	// TODO: PART 1 STEP 4
	hr = m_snSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)(&m_pBackBuffer));

	hr = m_iDevice->CreateRenderTargetView(m_pBackBuffer, NULL, &m_rtvRenderTargetView);

	// TODO: PART 1 STEP 5
	hr = m_snSwapChain->GetDesc(&m_scDesc);




#pragma region Depth Creation

	//setting up the depthbuffer
	D3D11_TEXTURE2D_DESC DepthDesc;
	//zero-ing it out
	ZeroMemory(&DepthDesc, sizeof(DepthDesc));

	DepthDesc.Width = BACKBUFFER_WIDTH;
	DepthDesc.Height = BACKBUFFER_HEIGHT;
	DepthDesc.MipLevels = 1;
	DepthDesc.ArraySize = 1;
	DepthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthDesc.SampleDesc.Count = 1;
	DepthDesc.SampleDesc.Quality = 0;
	DepthDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthDesc.CPUAccessFlags = 0;
	DepthDesc.MiscFlags = 0;

	m_iDevice->CreateTexture2D(&DepthDesc, NULL, &m_ZBuffer);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	DefaultDepthDesc(&depthStencilDesc);

	m_iDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthState);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDepthView;
	ZeroMemory(&descDepthView, sizeof(descDepthView));
	descDepthView.Format = DXGI_FORMAT_D32_FLOAT;
	descDepthView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDepthView.Texture2D.MipSlice = 0;

	m_iDevice->CreateDepthStencilView(m_ZBuffer, &descDepthView, &m_DepthView);

#pragma endregion

#pragma region Creation of the RasterizerState

	D3D11_RECT windowRect[1];
	windowRect[0].left = 0;
	windowRect[0].right = BACKBUFFER_WIDTH;
	windowRect[0].top = 0;
	windowRect[0].bottom = BACKBUFFER_HEIGHT;

	m_dcConext->RSSetScissorRects(1, windowRect);

	D3D11_RASTERIZER_DESC rasterizerState;

	DefaultRasterizerDesc(&rasterizerState);

	m_iDevice->CreateRasterizerState(&rasterizerState, &m_pRasterState);

	rasterizerState.CullMode = D3D11_CULL_FRONT;
	rasterizerState.FrontCounterClockwise = false;

	m_iDevice->CreateRasterizerState(&rasterizerState, &m_pRasterStateFrontCull);

#pragma endregion

#pragma region BlendState

	D3D11_BLEND_DESC BlendDesc;

	DefaultBlendDesc(&BlendDesc);

	m_iDevice->CreateBlendState(&BlendDesc, &m_pBlendState);
#pragma endregion

	//setting up the viewport
	m_vpViewPort.TopLeftX = 0;
	m_vpViewPort.TopLeftY = 0;
	m_vpViewPort.Height = (float)m_scDesc.BufferDesc.Height;
	m_vpViewPort.Width = (float)m_scDesc.BufferDesc.Width;
	m_vpViewPort.MaxDepth = 1;
	m_vpViewPort.MinDepth = 0;

	//Setting up the shaders
	m_iDevice->CreatePixelShader(PixelShader, sizeof(PixelShader), NULL, &m_shaderPS);
	m_iDevice->CreateVertexShader(VertexShader, sizeof(VertexShader), NULL, &m_shaderVS);


	// TODO: PART 2 STEP 8a
	D3D11_INPUT_ELEMENT_DESC element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = m_iDevice->CreateInputLayout(element, 3, VertexShader, sizeof(VertexShader), &m_pVertexInput);

	// setting the Constant variables

	//\//scene Vshader connnection


	//setting the width of each scene

	g_DepthView = m_DepthView;
	g_ZBuffer = m_ZBuffer;
	g_pDepthState = m_pDepthState;
	g_snSwapChain = m_snSwapChain;
	g_rtvRenderTargetView = m_rtvRenderTargetView;
	g_iDevice = m_iDevice;
	g_dcConext = m_dcConext;
	g_pBackBuffer = m_pBackBuffer;

	m_scDesc.BufferCount;
	dt.Restart();
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{

	m_DepthView = g_DepthView;
	m_pDepthState = g_pDepthState;
	m_rtvRenderTargetView = g_rtvRenderTargetView;

	dt.Signal();

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;

	m_dcConext->OMSetBlendState(m_pBlendState, blendFactor, sampleMask);
	//Binding the depthState/ depthBuffer
	m_dcConext->OMSetDepthStencilState(m_pDepthState, 1);

	//Binding the RendertTarget
	m_dcConext->OMSetRenderTargets(1, &m_rtvRenderTargetView, m_DepthView);

	//binding the viewports
	m_dcConext->RSSetViewports(1, &m_vpViewPort);

	// clearing the screen to that color
	float color[4];
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	color[3] = 1;

	m_dcConext->ClearDepthStencilView(m_DepthView, D3D11_CLEAR_DEPTH, 1, 0);

	m_dcConext->ClearRenderTargetView(m_rtvRenderTargetView, color);

#pragma region InputChecker 

	bool moved = false;

	if (GetAsyncKeyState('W'))
	{
		CameraZ += (float)dt.Delta();
		moved = true;
	}
	else if (GetAsyncKeyState('S'))
	{
		CameraZ -= (float)dt.Delta();
		moved = true;
	}
	else if (GetAsyncKeyState('A'))
	{
		CameraX -= (float)dt.Delta();

		moved = true;
	}
	else if (GetAsyncKeyState('D'))
	{
		CameraX += (float)dt.Delta();
		moved = true;
	}

	if (GetAsyncKeyState(VK_LSHIFT))
	{
		CameraY -= (float)dt.Delta();
		moved = true;
	}
	else if (GetAsyncKeyState(VK_SPACE))
	{
		CameraY += (float)dt.Delta();
		moved = true;
	}



#pragma endregion

	m_dcConext->PSSetShader(m_shaderPS, NULL, 0);
	m_dcConext->VSSetShader(m_shaderVS, NULL, 0);

	//Swaping the back buffer info with the front buffer
	m_snSwapChain->Present(1, 0);
	// END OF PART 1
	return true;
}

void DEMO_APP::MouseMovement(bool& move, float dt)
{

}

int DEMO_APP::DistanceFormula(POINT LH, POINT RH)
{
	int x = LH.x - RH.x;
	int y = LH.y - RH.y;
	x *= x;
	y *= y;

	x = x + y;

	x = (int)sqrt(x);
	return x;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6

	m_iDevice->Release();
	m_dcConext->Release();
	m_snSwapChain->Release();
	m_rtvRenderTargetView->Release();
	m_pBackBuffer->Release();
	m_shaderPS->Release();
	m_shaderVS->Release();
	//circle
	m_pVertexInput->Release();



	//depth perspective
	m_ZBuffer->Release();
	m_DepthView->Release();
	m_pDepthState->Release();
	m_pRasterState->Release();



	UnregisterClass(L"DirectXApplication", application);
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && myApp.Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	myApp.ShutDown();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case (WM_DESTROY) : { PostQuitMessage(0); }
						break;
	case(WM_SIZE) :

		if (g_snSwapChain)
		{

			g_dcConext->OMSetRenderTargets(0, 0, 0);

			// Release all outstanding references to the swap chain's buffers.
			g_ZBuffer->Release();
			g_pBackBuffer->Release();
			g_pDepthState->Release();
			g_rtvRenderTargetView->Release();
			g_dcConext->Flush();
			HRESULT hr;
			// Preserve the existing buffer count and format.
			// Automatically choose the width and height to match the client rect for HWNDs.
			hr = g_snSwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

			// Perform error handling here!


			// Get buffer and create a render-target-view.

			hr = g_snSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
				(void**)&g_pBackBuffer);
			// Perform error handling here!
			hr = g_iDevice->CreateRenderTargetView(g_pBackBuffer, NULL,
				&g_rtvRenderTargetView);

			// Perform error handling here!
#pragma region Depth Creation
			D3D11_TEXTURE2D_DESC temp;
			g_pBackBuffer->GetDesc(&temp);
			//setting up the depthbuffer
			D3D11_TEXTURE2D_DESC DepthDesc;
			//zero-ing it out
			ZeroMemory(&DepthDesc, sizeof(DepthDesc));

			DepthDesc.Width = temp.Width;
			DepthDesc.Height = temp.Height;
			DepthDesc.MipLevels = 1;
			DepthDesc.ArraySize = 1;
			DepthDesc.Format = DXGI_FORMAT_D32_FLOAT;
			DepthDesc.SampleDesc.Count = 1;
			DepthDesc.SampleDesc.Quality = 0;
			DepthDesc.Usage = D3D11_USAGE_DEFAULT;
			DepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			DepthDesc.CPUAccessFlags = 0;
			DepthDesc.MiscFlags = 0;

			g_iDevice->CreateTexture2D(&DepthDesc, NULL, &g_ZBuffer);

			D3D11_DEPTH_STENCIL_VIEW_DESC descDepthView;
			ZeroMemory(&descDepthView, sizeof(descDepthView));
			descDepthView.Format = DXGI_FORMAT_D32_FLOAT;
			descDepthView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDepthView.Texture2D.MipSlice = 0;

			g_iDevice->CreateDepthStencilView(g_ZBuffer, &descDepthView, &g_DepthView);

#pragma endregion

#pragma region //Creation of the RasterizerState

			D3D11_RECT windowRect[1];
			windowRect[0].left = 0;
			windowRect[0].right = temp.Width;
			windowRect[0].top = 0;
			windowRect[0].bottom = temp.Height;

			g_dcConext->RSSetScissorRects(1, windowRect);
#pragma endregion


			g_dcConext->OMSetRenderTargets(1, &g_rtvRenderTargetView, g_DepthView);

			// Set up the viewport.

			m_vpViewPort.Width = (float)temp.Width;
			m_vpViewPort.Height = (float)temp.Height;
			m_vpViewPort.MinDepth = 0.0f;
			m_vpViewPort.MaxDepth = 1.0f;
			m_vpViewPort.TopLeftX = 0;
			m_vpViewPort.TopLeftY = 0;
			g_dcConext->RSSetViewports(1, &m_vpViewPort);

		}

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//