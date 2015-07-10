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
#include "../DefaultPS.csh"
#include "../Skybox_VS.csh"
#include "../Skybox_PS.csh"

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
#define OBJECT				0
#define SCENE				1
#define TIMER				2

#define UP					0
#define DOWN				1
#define RIGHT				2
#define LEFT				3
#define SAFE_RELEASE(x) if(x) { x->Release(); x = nullptr; } 
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

bool LbuttonDown = false;
POINT startPoint;

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
	D3D11_VIEWPORT m_vpSecondaryView;
	DXGI_SWAP_CHAIN_DESC m_scDesc;

	// TODO: PART 2 STEP 4
	ID3D11PixelShader*	m_shaderPS;

	ID3D11PixelShader* m_DefaultPS;
	ID3D11VertexShader* m_shaderVS;

	ID3D11PixelShader* m_SkyboxPS;
	ID3D11VertexShader* m_SkyboxVS;

	//depth perspective
	ID3D11Texture2D* m_ZBuffer;
	ID3D11DepthStencilView* m_DepthView;
	ID3D11DepthStencilState * m_pDepthState;

	//Blending
	ID3D11BlendState* m_pBlendState;
	ID3D11RasterizerState* m_pRasterStateFrontCull;
	ID3D11RasterizerState* m_pSkyBoxRasterState;
	ID3D11Debug *m_dgDebug;
	//camera
	//Math
	XMFLOAT4X4 m_mxWorldMatrix;
	XMFLOAT4X4 m_mxViewMatrix;
	XMFLOAT4X4 m_mxProjectonMatrix;
	Scene SceneMatrices;

	Scene m_miniScene;
	XMFLOAT4X4 m_mxMiniViewMatrix;

	//misc

	XMFLOAT4X4 rotation;
	POINT distance;


	void MouseMovement(bool& move, float dt, XMFLOAT4X4* rot);

	bool displayWindow = false;
	bool CursorClientCheck(POINT curr);
public:
	Model m_StarModel;
	Model Pyramid;
	Model Dorumon;
	Model SkyBox;

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);

	void CreateStar(D3D11_SAMPLER_DESC * p_sampler);

	void CreateObj(const char* file, Model& p_model, D3D11_SUBRESOURCE_DATA* p_data = nullptr, D3D11_TEXTURE2D_DESC* p_texture = nullptr, D3D11_SAMPLER_DESC* p_sampler = nullptr);

	void CreateObj(const char* file, Model& p_model, const wchar_t* filename, D3D11_SAMPLER_DESC * p_sampler);

	void DrawObj(Model* p_model, ID3D11RasterizerState** raster, unsigned int size, float delta);
	void DrawStar();
	void UpdateSkyBox();
	void CreateSkyBox(D3D11_SAMPLER_DESC * p_sampler);
	int DistanceFormula(POINT LH, POINT RH);
	void WorldCameraProjectionSetup();
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

	// TODO: PART 1 STEP 4
	m_snSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)(&m_pBackBuffer));
	m_iDevice->CreateRenderTargetView(m_pBackBuffer, NULL, &m_rtvRenderTargetView);

	// TODO: PART 1 STEP 5
	hr = m_snSwapChain->GetDesc(&m_scDesc);

	//Setting up the World/Camera/Projection Matrices
	WorldCameraProjectionSetup();

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

	hr = m_iDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthState);

	if (hr == E_INVALIDARG)
	{
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDepthView;
	ZeroMemory(&descDepthView, sizeof(descDepthView));
	descDepthView.Format = DXGI_FORMAT_D32_FLOAT;
	descDepthView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDepthView.Texture2D.MipSlice = 0;

	hr = m_iDevice->CreateDepthStencilView(m_ZBuffer, &descDepthView, &m_DepthView);

	if (hr == E_INVALIDARG)
	{
		return;
	}
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

	hr = m_iDevice->CreateRasterizerState(&rasterizerState, &m_pRasterStateFrontCull);

	if (hr == E_INVALIDARG)
	{
		return;
	}
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

	//setting up the viewport
	m_vpSecondaryView.TopLeftX = 0;
	m_vpSecondaryView.TopLeftY = 0;
	m_vpSecondaryView.Height = 200;
	m_vpSecondaryView.Width = 200;
	m_vpSecondaryView.MaxDepth = 1;
	m_vpSecondaryView.MinDepth = 0;

	//Setting up the shaders
	m_iDevice->CreatePixelShader(PixelShader, sizeof(PixelShader), NULL, &m_shaderPS);
	m_iDevice->CreateVertexShader(VertexShader, sizeof(VertexShader), NULL, &m_shaderVS);

	m_iDevice->CreatePixelShader(DefaultPS, sizeof(DefaultPS), NULL, &m_DefaultPS);

	m_iDevice->CreatePixelShader(Skybox_PS, sizeof(Skybox_PS), NULL, &m_SkyboxPS);
	m_iDevice->CreateVertexShader(Skybox_VS, sizeof(Skybox_VS), NULL, &m_SkyboxVS);

	// TODO: PART 2 STEP 8a
	D3D11_INPUT_ELEMENT_DESC element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	m_iDevice->CreateInputLayout(element, 4, VertexShader, sizeof(VertexShader), &m_pVertexInput);

	GetCursorPos(&startPoint);

	//cube model



	XMMATRIX temp;

	//Pyramid Creation
	D3D11_SAMPLER_DESC SamplerDesc;
	DefaultSamplerStateDesc(&SamplerDesc);

	CreateStar(&SamplerDesc);

	CreateSkyBox(&SamplerDesc);

	Pyramid.SetAnimation(4, (float)numbers_test_width);

	CreateObj("resource/Models/test pyramid.obj", Pyramid, L"resource/Texture/numbers_test.dds", &SamplerDesc);

	temp = XMMatrixTranslation(0, 2, 0) * XMLoadFloat4x4(&m_mxWorldMatrix);

	XMStoreFloat4x4(&Pyramid.m_objMatrix.m_mxConstMatrix, temp);



	//Drawing a dorumon
	CreateObj("resource/Models/Dorumon.obj", Dorumon, L"resource/Texture/Dorumon.dds", &SamplerDesc);

	temp = XMLoadFloat4x4(&m_mxWorldMatrix) * XMMatrixTranslation(0, 0, 0);

	temp = XMMatrixRotationY(XMConvertToRadians(180)) * temp;

	XMStoreFloat4x4(&Dorumon.m_objMatrix.m_mxConstMatrix, temp);

	Dorumon.ScaleModel(0.5f);



	/// debug
#if _DEBUG
	m_iDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&m_dgDebug));
#endif

#pragma region Global setup
	g_DepthView = m_DepthView;
	g_ZBuffer = m_ZBuffer;
	g_pDepthState = m_pDepthState;
	g_snSwapChain = m_snSwapChain;
	g_rtvRenderTargetView = m_rtvRenderTargetView;
	g_iDevice = m_iDevice;
	g_dcConext = m_dcConext;
	g_pBackBuffer = m_pBackBuffer;
#pragma endregion


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
	color[1] = 1;
	color[2] = 0;
	color[3] = 1;

	m_dcConext->ClearDepthStencilView(m_DepthView, D3D11_CLEAR_DEPTH, 1, 0);

	m_dcConext->ClearRenderTargetView(m_rtvRenderTargetView, color);


#pragma region InputChecker 

	float CameraX = 0, CameraY = 0, CameraZ = 0;

	bool moved = false;

	XMStoreFloat4x4(&rotation, XMMatrixIdentity());

	if (GetAsyncKeyState('W'))
	{
		CameraZ -= (float)dt.Delta();
		moved = true;
	}
	else if (GetAsyncKeyState('S'))
	{
		CameraZ += (float)dt.Delta();
		moved = true;
	}
	else if (GetAsyncKeyState('A'))
	{
		CameraX += (float)dt.Delta();

		moved = true;
	}
	else if (GetAsyncKeyState('D'))
	{
		CameraX -= (float)dt.Delta();
		moved = true;
	}

	if (GetAsyncKeyState(VK_LSHIFT))
	{
		CameraY += (float)dt.Delta();
		moved = true;
	}
	else if (GetAsyncKeyState(VK_SPACE))
	{
		CameraY -= (float)dt.Delta();
		moved = true;
	}
	if (GetAsyncKeyState('M'))
	{
		displayWindow = true;
	}

	if (GetAsyncKeyState(VK_LBUTTON))
	{
		POINT CurrentPoint;
		GetCursorPos(&CurrentPoint);


		if (CursorClientCheck(CurrentPoint))
		{
			distance.x = 0;
			distance.y = 0;

			distance.x = CurrentPoint.x - startPoint.x;
			distance.y = CurrentPoint.y - startPoint.y;

			MouseMovement(moved, (float)0.5f, &rotation);

			startPoint.x = CurrentPoint.x;
			startPoint.y = CurrentPoint.y;
		}

	}

	if (GetAsyncKeyState('0') & 0x1)
	{
		XMMATRIX view = XMMatrixIdentity();
		//for the Look At Function
		XMVECTOR pos = XMLoadFloat3(&XMFLOAT3(0, 0, -1));
		XMVECTOR focus = XMLoadFloat3(&XMFLOAT3(0, 0, 0));
		XMVECTOR height = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
		//Creating the Camera
		view = XMMatrixLookAtLH(pos, focus, height);
		//Saving the camera
		XMStoreFloat4x4(&m_mxViewMatrix, view);
		SceneMatrices.matrix_sceneCamera = m_mxViewMatrix;

		XMStoreFloat4x4(&SkyBox.m_objMatrix.m_mxConstMatrix, XMMatrixIdentity());
	}



	if (moved)
	{
		XMMATRIX CopyView, inverseCopy;

		CopyView = inverseCopy = XMLoadFloat4x4(&m_mxViewMatrix);

		XMMATRIX inverseRotation = XMMatrixInverse(nullptr, XMLoadFloat4x4(&rotation));

		inverseCopy = XMMatrixInverse(nullptr, inverseCopy);

		inverseCopy = inverseRotation * inverseCopy;

		inverseCopy = XMMatrixInverse(nullptr, inverseCopy);

		XMStoreFloat4x4(&SkyBox.m_objMatrix.m_mxConstMatrix, inverseCopy);



		//XMVECTOR pos = CopyView.r[3];

		//CopyView.r[3] = XMVectorSet(0, 0, 0, 1);

		CopyView = XMMatrixInverse(nullptr, CopyView);

		CopyView = XMLoadFloat4x4(&rotation) * CopyView;

		CopyView = XMMatrixInverse(nullptr, CopyView);

		//CopyView.r[3] = pos;



		CopyView = CopyView * XMMatrixTranslation(CameraX, CameraY, CameraZ);

		XMStoreFloat4x4(&m_mxViewMatrix, CopyView);

		SceneMatrices.matrix_sceneCamera = m_mxViewMatrix;

		/****************************************************************************/
	}
#pragma endregion

	UpdateSkyBox();

	D3D11_MAPPED_SUBRESOURCE resource;
	D3D11_MAPPED_SUBRESOURCE SceneResource;
	XMMATRIX matrix;

	ID3D11RasterizerState* rasterArray[2];
	rasterArray[0] = m_pRasterStateFrontCull;
	rasterArray[1] = m_pRasterState;

#pragma region Pyramid constantBuffer


	ZeroMemory(&resource, sizeof(resource));

	m_dcConext->Map(Pyramid.m_pConstBuffer[OBJECT], 0, D3D11_MAP_WRITE_DISCARD, NULL, &resource);

	matrix = XMLoadFloat4x4(&Pyramid.m_objMatrix.m_mxConstMatrix);

	matrix = XMMatrixRotationX((float)dt.Delta()) * matrix;

	matrix.r[3] = XMVectorSet(SceneMatrices.matrix_sceneCamera._41, SceneMatrices.matrix_sceneCamera._42, SceneMatrices.matrix_sceneCamera._43, 1);

	XMStoreFloat4x4(&Pyramid.m_objMatrix.m_mxConstMatrix, matrix);

	memcpy(resource.pData, &Pyramid.m_objMatrix, sizeof(Pyramid.m_objMatrix));

	m_dcConext->Unmap(Pyramid.m_pConstBuffer[OBJECT], 0);

	////////////////////////////////////////////////////////////////////

	ZeroMemory(&SceneResource, sizeof(SceneResource));

	m_dcConext->Map(Pyramid.m_pConstBuffer[SCENE], 0, D3D11_MAP_WRITE_DISCARD, NULL, &SceneResource);

	memcpy(SceneResource.pData, &SceneMatrices, sizeof(SceneMatrices));

	m_dcConext->Unmap(Pyramid.m_pConstBuffer[SCENE], 0);

#pragma endregion

	DrawObj(&Pyramid, rasterArray, 2, (float)dt.Delta());

	DrawStar();

#pragma region Dorumon constantBuffer

	ZeroMemory(&resource, sizeof(resource));

	m_dcConext->Map(Dorumon.m_pConstBuffer[OBJECT], 0, D3D11_MAP_WRITE_DISCARD, NULL, &resource);

	memcpy(resource.pData, &Dorumon.m_objMatrix, sizeof(Dorumon.m_objMatrix));

	m_dcConext->Unmap(Dorumon.m_pConstBuffer[OBJECT], 0);

	//////////////////////////////////////////////////////////////////////

	ZeroMemory(&SceneResource, sizeof(SceneResource));

	m_dcConext->Map(Dorumon.m_pConstBuffer[SCENE], 0, D3D11_MAP_WRITE_DISCARD, NULL, &SceneResource);

	memcpy(SceneResource.pData, &SceneMatrices, sizeof(SceneMatrices));

	m_dcConext->Unmap(Dorumon.m_pConstBuffer[SCENE], 0);

#pragma endregion

	DrawObj(&Dorumon, nullptr, 0, (float)dt.Delta());

#pragma region Second Draw

	if (displayWindow)
	{

#pragma region Dorumon Redraw constantBuffer

		//////////////////////////////////////////////////////////////////////

		ZeroMemory(&SceneResource, sizeof(SceneResource));

		m_dcConext->Map(Dorumon.m_pConstBuffer[SCENE], 0, D3D11_MAP_WRITE_DISCARD, NULL, &SceneResource);

		memcpy(SceneResource.pData, &m_miniScene, sizeof(m_miniScene));

		m_dcConext->Unmap(Dorumon.m_pConstBuffer[SCENE], 0);

#pragma endregion

		float reColor[4];
		reColor[0] = 0;
		reColor[1] = 0;
		reColor[2] = 0;
		reColor[3] = 0;
		//m_dcConext->ClearRenderTargetView(m_rtvRenderTargetView, reColor);

		m_dcConext->RSSetViewports(1, &m_vpSecondaryView);
		DrawObj(&Dorumon, nullptr, 0, (float)dt.Delta());


	}
#pragma endregion 



	//Swaping the back buffer info with the front buffer
	m_snSwapChain->Present(0, 0);
	// END OF PART 1

	return true;
}

void DEMO_APP::WorldCameraProjectionSetup()
{
	//Setting the World matrix
	XMMATRIX world = XMMatrixIdentity();
	world = XMMatrixTranslation(0, 0, 1.0f);
	XMStoreFloat4x4(&m_mxWorldMatrix, world);

	/*******************************************************************/
	//\//Setting the Camera Matrix

	XMMATRIX view = XMMatrixIdentity();
	//for the Look At Function
	XMVECTOR pos = XMLoadFloat3(&XMFLOAT3(0, 0, -1));
	XMVECTOR focus = XMLoadFloat3(&XMFLOAT3(0, 0, 0));
	XMVECTOR height = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
	//Creating the Camera
	view = XMMatrixLookAtLH(pos, focus, height);
	//Saving the camera
	XMStoreFloat4x4(&m_mxViewMatrix, view);


	//for the Look At Function
	pos = XMLoadFloat3(&XMFLOAT3(0, 1, -0.15f));
	focus = XMLoadFloat3(&XMFLOAT3(0, 0, 0));
	height = XMLoadFloat3(&XMFLOAT3(0, 1, -0.25));
	//Creating the Camera
	view = XMMatrixLookAtLH(pos, focus, height);

	XMStoreFloat4x4(&m_mxMiniViewMatrix, view);


	/*******************************************************************/
	//Setting the Projection Matrix

	XMMATRIX projection = XMMatrixIdentity();
	projection = XMMatrixPerspectiveFovLH(FOV, ASPECT_RATIO, ZNEAR, ZFAR);
	XMStoreFloat4x4(&m_mxProjectonMatrix, projection);

	SceneMatrices.matrix_sceneCamera = m_mxViewMatrix;
	SceneMatrices.matrix_Projection = m_mxProjectonMatrix;

	m_miniScene.matrix_Projection = m_mxProjectonMatrix;
	m_miniScene.matrix_sceneCamera = m_mxMiniViewMatrix;
}

void DEMO_APP::MouseMovement(bool& move, float dt, XMFLOAT4X4* rot)
{

	XMMATRIX CameraRotate = XMLoadFloat4x4(rot);

	if (fabsf((float)distance.x) > fabsf((float)distance.y))
	{
		CameraRotate = CameraRotate* XMMatrixRotationY(XMConvertToRadians((float)distance.x) *dt);
	}
	else if (fabsf((float)distance.x) < fabsf((float)distance.y))
	{
		CameraRotate = XMMatrixRotationX(XMConvertToRadians((float)distance.y)* dt)* CameraRotate;
	}

	XMStoreFloat4x4(rot, CameraRotate);

	move = true;
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

void DEMO_APP::CreateStar(D3D11_SAMPLER_DESC * p_sampler)
{
	INPUT_VERTEX* Star = new INPUT_VERTEX[22];

#pragma region //Star
	//white
	Star[0].pos.x = 0;
	Star[0].pos.y = 0;
	Star[0].pos.z = 0;
	Star[0].pos.w = 1;
	Star[0].col = XMFLOAT4(1, 1, 1, 1);
	//red
	Star[1].pos.x = 0;
	Star[1].pos.y = 1;
	Star[1].pos.z = 0;
	Star[1].pos.w = 1;
	Star[1].col = XMFLOAT4(1, 0, 0, 1);

	//red
	Star[2].pos.x = 0.3f;
	Star[2].pos.y = 0.3f;
	Star[2].pos.z = 0;
	Star[2].pos.w = 1;
	Star[2].col = XMFLOAT4(1, 0, 0, 1);

	//blue
	Star[3].pos.x = 0.7f;
	Star[3].pos.y = 0.3f;
	Star[3].pos.z = 0;
	Star[3].pos.w = 1;
	Star[3].col = XMFLOAT4(0, 0, 1, 1);

	//blue
	Star[4].pos.x = 0.4f;
	Star[4].pos.y = -0.2f;
	Star[4].pos.z = 0;
	Star[4].pos.w = 1;
	Star[4].col = XMFLOAT4(0, 0, 1, 1);

	//green
	Star[5].pos.x = 0.4f;
	Star[5].pos.y = -0.7f;
	Star[5].pos.z = 0;
	Star[5].pos.w = 1;
	Star[5].col = XMFLOAT4(0, 0, 1, 1);

	//green
	Star[6].pos.x = 0;
	Star[6].pos.y = -0.4f;
	Star[6].pos.z = 0;
	Star[6].pos.w = 1;
	Star[6].col = XMFLOAT4(0, 0, 1, 1);

	//yellow
	Star[7].pos.x = -0.4f;
	Star[7].pos.y = -0.7f;
	Star[7].pos.z = 0;
	Star[7].pos.w = 1;
	Star[7].col = XMFLOAT4(1, 1, 0, 1);

	//yellow
	Star[8].pos.x = -0.4f;
	Star[8].pos.y = -0.2f;
	Star[8].pos.z = 0;
	Star[8].pos.w = 1;
	Star[8].col = XMFLOAT4(1, 1, 0, 1);

	//cyan
	Star[9].pos.x = -0.7f;
	Star[9].pos.y = 0.3f;
	Star[9].pos.z = 0;
	Star[9].pos.w = 1;
	Star[9].col = XMFLOAT4(0, 1, 1, 1);

	//cyan
	Star[10].pos.x = -0.3f;
	Star[10].pos.y = 0.3f;
	Star[10].pos.z = 0;
	Star[10].pos.w = 1;
	Star[10].col = XMFLOAT4(0, 1, 1, 1);

	//white
	Star[11].pos.x = 0;
	Star[11].pos.y = 0;
	Star[11].pos.z = 0.5f;
	Star[11].pos.w = 1;
	Star[11].col = XMFLOAT4(1, 1, 1, 1);

	//red
	Star[12].pos.x = 0;
	Star[12].pos.y = 1;
	Star[12].pos.z = 0.5f;
	Star[12].pos.w = 1;
	Star[12].col = XMFLOAT4(1, 0, 0, 1);

	//cyan
	Star[13].pos.x = -0.3f;
	Star[13].pos.y = 0.3f;
	Star[13].pos.z = 0.5f;
	Star[13].pos.w = 1;
	Star[13].col = XMFLOAT4(0, 1, 1, 1);

	//cyan
	Star[14].pos.x = -0.7f;
	Star[14].pos.y = 0.3f;
	Star[14].pos.z = 0.5f;
	Star[14].pos.w = 1;
	Star[14].col = XMFLOAT4(0, 1, 1, 1);

	//yelloe
	Star[15].pos.x = -0.4f;
	Star[15].pos.y = -0.2f;
	Star[15].pos.z = 0.5f;
	Star[15].pos.w = 1;
	Star[15].col = XMFLOAT4(1, 1, 0, 1);

	//yellow
	Star[16].pos.x = -0.4f;
	Star[16].pos.y = -0.7f;
	Star[16].pos.z = 0.5f;
	Star[16].pos.w = 1;
	Star[16].col = XMFLOAT4(1, 1, 0, 1);

	//green
	Star[17].pos.x = 0;
	Star[17].pos.y = -0.4f;
	Star[17].pos.z = 0.5f;
	Star[17].pos.w = 1;
	Star[17].col = XMFLOAT4(0, 1, 0, 1);

	//green
	Star[18].pos.x = 0.4f;
	Star[18].pos.y = -0.7f;
	Star[18].pos.z = 0.5f;
	Star[18].pos.w = 1;
	Star[18].col = XMFLOAT4(0, 1, 0, 1);


	//wblue
	Star[19].pos.x = 0.4f;
	Star[19].pos.y = -0.2f;
	Star[19].pos.z = 0.5f;
	Star[19].pos.w = 1;
	Star[19].col = XMFLOAT4(0, 0, 1, 1);

	//blue
	Star[20].pos.x = 0.7f;
	Star[20].pos.y = 0.3f;
	Star[20].pos.z = 0.5f;
	Star[20].pos.w = 1;
	Star[20].col = XMFLOAT4(0, 0, 1, 1);

	//red

	Star[21].pos.x = 0.3f;
	Star[21].pos.y = 0.3f;
	Star[21].pos.z = 0.5f;
	Star[21].pos.w = 1;
	Star[21].col = XMFLOAT4(0, 1, 0, 1);



#pragma endregion
#pragma region IndexBuffer
	int maxIndices = 120;
	unsigned int indices[120] =
	{
		//front
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		0, 7, 8,
		0, 8, 9,
		0, 9, 10,
		0, 10, 1,
		//back
		11, 12, 13,
		11, 13, 14,
		11, 14, 15,
		11, 15, 16,
		11, 16, 17,
		11, 17, 18,
		11, 18, 19,
		11, 19, 20,
		11, 20, 21,
		11, 21, 12,
		//sides

		//top
		1, 12, 2,
		12, 21, 2,
		12, 1, 13,
		1, 10, 13,
		//left upper
		2, 21, 3,
		21, 20, 3,
		// right upper
		13, 10, 14,
		10, 9, 14,
		//B_Right upper
		14, 9, 15,
		9, 8, 15,
		//B_Left upper
		3, 20, 4,
		20, 19, 4,

		//Left bottom
		4, 19, 5,
		19, 18, 5,
		//Right bottom
		15, 8, 7,
		16, 15, 7,
		//B_right bottom
		6, 17, 16,
		7, 6, 16,
		//B_left bottom
		17, 6, 5,
		17, 5, 18,
	};
#pragma endregion

	m_StarModel.loadVerts(22, Star);
	//m_StarModel.CreateTexture(m_iDevice, L"resource/Texture/Star.dds");
	m_StarModel.CreateBuffers(m_iDevice, maxIndices, indices, &SceneMatrices);

	XMMATRIX temp = XMMatrixTranslation(0, 3, 0) * XMLoadFloat4x4(&m_mxWorldMatrix);

	XMStoreFloat4x4(&m_StarModel.m_objMatrix.m_mxConstMatrix, temp);

}

void DEMO_APP::CreateObj(const char* file, Model& p_model, D3D11_SUBRESOURCE_DATA* p_data, D3D11_TEXTURE2D_DESC* p_texture, D3D11_SAMPLER_DESC* p_sampler)
{
	INPUT_VERTEX* ObjVerts = nullptr;
	unsigned int * ObjectIndices = nullptr;
	unsigned int max = 0;
	unsigned int maxIndex = 0;

	ObjectLoader(file, &ObjVerts, &ObjectIndices, max, maxIndex);

	p_model.loadVerts(max, ObjVerts);

	if (p_sampler != nullptr && p_texture != nullptr || p_data != nullptr)
	{
		p_model.CreateTexture(m_iDevice, p_sampler, p_texture, p_data);
	}

	p_model.CreateBuffers(m_iDevice, maxIndex, ObjectIndices, &SceneMatrices);

}

void DEMO_APP::CreateObj(const char* file, Model& p_model, const wchar_t* filename, D3D11_SAMPLER_DESC* p_sampler)
{
	INPUT_VERTEX* ObjVerts = nullptr;
	unsigned int * ObjectIndices = nullptr;
	unsigned int max = 0;
	unsigned int maxIndex = 0;

	ObjectLoader(file, &ObjVerts, &ObjectIndices, max, maxIndex);

	p_model.loadVerts(max, ObjVerts);

	p_model.CreateTexture(m_iDevice, filename, p_sampler);

	p_model.CreateBuffers(m_iDevice, maxIndex, ObjectIndices, &SceneMatrices);

}

void DEMO_APP::DrawObj(Model* p_model, ID3D11RasterizerState** raster, unsigned int size, float delta)
{
	m_dcConext->PSSetShader(m_shaderPS, NULL, 0);
	m_dcConext->VSSetShader(m_shaderVS, NULL, 0);

	p_model->Draw(m_dcConext, m_pVertexInput, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, raster, size, delta);
}

void DEMO_APP::DrawStar()
{
	D3D11_MAPPED_SUBRESOURCE resource;
	D3D11_MAPPED_SUBRESOURCE SceneResource;
	XMMATRIX matrix;

#pragma region Star constantBuffer


	ZeroMemory(&resource, sizeof(resource));

	m_dcConext->Map(m_StarModel.m_pConstBuffer[OBJECT], 0, D3D11_MAP_WRITE_DISCARD, NULL, &resource);

	matrix = XMLoadFloat4x4(&m_StarModel.m_objMatrix.m_mxConstMatrix);

	matrix = XMMatrixRotationY((float)dt.Delta()) * matrix;

	XMStoreFloat4x4(&m_StarModel.m_objMatrix.m_mxConstMatrix, matrix);

	memcpy(resource.pData, &m_StarModel.m_objMatrix, sizeof(m_StarModel.m_objMatrix));

	m_dcConext->Unmap(m_StarModel.m_pConstBuffer[OBJECT], 0);

	//////////////////////////////////////////////////////////////////////

	ZeroMemory(&SceneResource, sizeof(SceneResource));

	m_dcConext->Map(m_StarModel.m_pConstBuffer[SCENE], 0, D3D11_MAP_WRITE_DISCARD, NULL, &SceneResource);

	memcpy(SceneResource.pData, &SceneMatrices, sizeof(SceneMatrices));

	m_dcConext->Unmap(m_StarModel.m_pConstBuffer[SCENE], 0);

#pragma endregion

	m_dcConext->PSSetShader(m_DefaultPS, NULL, 0);
	m_dcConext->VSSetShader(m_shaderVS, NULL, 0);

	m_StarModel.Draw(m_dcConext, m_pVertexInput, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, nullptr, 0, (float)dt.Delta());
}

void DEMO_APP::UpdateSkyBox()
{
	D3D11_MAPPED_SUBRESOURCE resource;
	D3D11_MAPPED_SUBRESOURCE SceneResource;
	XMMATRIX matrix;


#pragma region SkyBox ConstantBuffer
	ZeroMemory(&resource, sizeof(resource));

	m_dcConext->Map(SkyBox.m_pConstBuffer[OBJECT], 0, D3D11_MAP_WRITE_DISCARD, NULL, &resource);

	matrix = XMLoadFloat4x4(&SkyBox.m_objMatrix.m_mxConstMatrix);

	//	matrix 

	matrix.r[3] = XMVectorSet(-m_mxViewMatrix._41, -m_mxViewMatrix._42, -m_mxViewMatrix._43, 1);

	XMStoreFloat4x4(&SkyBox.m_objMatrix.m_mxConstMatrix, matrix);

	memcpy(resource.pData, &SkyBox.m_objMatrix, sizeof(SkyBox.m_objMatrix));

	m_dcConext->Unmap(SkyBox.m_pConstBuffer[OBJECT], 0);

	//////////////////////////////////////////////////////////////////////

	ZeroMemory(&SceneResource, sizeof(SceneResource));

	m_dcConext->Map(SkyBox.m_pConstBuffer[SCENE], 0, D3D11_MAP_WRITE_DISCARD, NULL, &SceneResource);

	memcpy(SceneResource.pData, &SceneMatrices, sizeof(SceneMatrices));

	m_dcConext->Unmap(SkyBox.m_pConstBuffer[SCENE], 0);
#pragma endregion

	m_dcConext->PSSetShader(m_SkyboxPS, NULL, 0);
	m_dcConext->VSSetShader(m_SkyboxVS, NULL, 0);

	SkyBox.Draw(m_dcConext, m_pVertexInput, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &m_pSkyBoxRasterState, 1, (float)dt.Delta());

	m_dcConext->ClearDepthStencilView(m_DepthView, D3D11_CLEAR_DEPTH, 1, 0);
}

void DEMO_APP::CreateSkyBox(D3D11_SAMPLER_DESC * p_sampler)
{
	CreateObj("resource/Models/Cube.obj", SkyBox, L"resource/Texture/Star.dds", p_sampler);

	D3D11_RASTERIZER_DESC skyboxRasterDesc;
	ZeroMemory(&skyboxRasterDesc, sizeof(skyboxRasterDesc));
	skyboxRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	skyboxRasterDesc.FillMode = D3D11_FILL_SOLID;
	m_iDevice->CreateRasterizerState(&skyboxRasterDesc, &m_pSkyBoxRasterState);

	XMMATRIX temp = XMLoadFloat4x4(&m_mxViewMatrix);

	XMStoreFloat4x4(&SkyBox.m_objMatrix.m_mxConstMatrix, temp);

	//SkyBox.ScaleModel(1.0f);
}

bool DEMO_APP::CursorClientCheck(POINT curr)
{
	POINT current = curr;
	RECT client;
	ScreenToClient(window, &current);
	GetClientRect(window, &client);

	if (client.bottom < current.y || client.top > current.y ||
		client.right < current.x || client.left > current.x)
	{
		return false;
	}

	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6


	SAFE_RELEASE(m_shaderPS);
	SAFE_RELEASE(m_shaderVS);
	SAFE_RELEASE(m_DefaultPS);
	SAFE_RELEASE(m_SkyboxPS);
	SAFE_RELEASE(m_SkyboxVS);

	SAFE_RELEASE(m_pDepthState);
	SAFE_RELEASE(m_DepthView);
	SAFE_RELEASE(m_ZBuffer);

	SAFE_RELEASE(m_pSkyBoxRasterState);
	//depth perspective						 

	// TODO: PART 1 STEP 2
	SAFE_RELEASE(m_pVertexInput);

	SAFE_RELEASE(m_pRasterState);
	SAFE_RELEASE(m_pRasterStateFrontCull);

	SAFE_RELEASE(m_pBlendState);
	SAFE_RELEASE(m_rtvRenderTargetView);
	SAFE_RELEASE(m_pBackBuffer);
	SAFE_RELEASE(m_snSwapChain);
	SAFE_RELEASE(m_dcConext);

#if _DEBUG
	m_dgDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
#endif

	SAFE_RELEASE(m_dgDebug);

	m_iDevice->Release();

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
	case (WM_LBUTTONDOWN) :
	{
		GetCursorPos(&startPoint);
		LbuttonDown = true;
		break;
	}
	case (WM_LBUTTONUP) :
	{
		LbuttonDown = false;
		break;
	}
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