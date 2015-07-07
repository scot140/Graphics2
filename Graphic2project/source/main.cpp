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
#include <DirectXMath.h>
#include <vector>
#include "MathHelper.h"
#include "Cube.h"
#include "numbers_test.h"
//#include <atlbase.h>

using namespace std;
using namespace DirectX;

// BEGIN PART 1
// TODO: PART 1 STEP 1a
//#include <d3d11.h>
//#pragma comment(lib, "d3d11.lib")
#include "DirectX_Helpers.h"
// TODO: PART 1 STEP 1b

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
	ID3D11Buffer** m_ConstantBuffer;

	vector<ID3D11Buffer*> GeometryContainer;

	//index buffer
	ID3D11Buffer* IndexBuffer;

	//depth perspective
	ID3D11Texture2D* m_ZBuffer;
	ID3D11DepthStencilView* m_DepthView;
	ID3D11DepthStencilState * m_pDepthState;
	ID3D11SamplerState* SamplerState;

	//Image
	ID3D11Texture2D* m_ImageTexture;
	ID3D11ShaderResourceView* m_ShaderResource;

	//Blending
	ID3D11BlendState* m_pBlendState;
	ID3D11RasterizerState* m_pRasterStateFrontCull;

	// TODO: PART 3 STEP 2b
	struct Object
	{
		Matrix matrix_objectWorld;
	};

	struct Scene
	{
		Matrix matrix_sceneCamera;
		Matrix matrix_Projection;
	};

	// TODO: PART 3 STEP 4a
	Matrix m_mxViewMatrix;
	Matrix m_mxWorldMatrix;
	Matrix m_mxProjectionMatrix;

	//camera
	Matrix CameraRotationMatrix;
	Matrix CameraTranslationMatrix;

	int CameraUpDown;
	float CameraX = 0, CameraY = 0, CameraZ = -3;
	void MouseMovement(bool& move, float dt);

	struct ANIMATION
	{
		float frame = 0;
		float maxFrame = 0;
		float width;
		float padding;
	};

	ANIMATION animate;
	float delta = 0;

	POINT startPoint;
	POINT CurrentPoint;
	bool LbuttonDown;

	unsigned int length;

public:

	// BEGIN PART 2
	// TODO: PART 2 STEP 1

	Object m_objObject;
	Scene m_scnMatrices;

	Object m_Grid;
	Scene m_GridScene;

	unsigned int maxVerts = 0;

	unsigned int maxIndices = 0;
	unsigned int maxGridIndices = 0;

	Vertex Cube[776];
	Vertex m_vtGrid_Verts[86];

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	void SetConstantBuffer(ID3D11Buffer* buffer, D3D11_MAP map, Object _struct, unsigned int start = 0, unsigned int max = 1);
	void Drawing(ID3D11Buffer* buffer, ID3D11InputLayout* layout, ID3D11VertexShader* VS_Shader, ID3D11PixelShader* PS_Shader, D3D_PRIMITIVE_TOPOLOGY topology, int MaxVerts, unsigned int startBuffer = 0, unsigned int maxBuffer = 1);
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
	m_ConstantBuffer = new ID3D11Buffer*[3];

	// TODO: PART 1 STEP 4
	hr = m_snSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)(&m_pBackBuffer));

	hr = m_iDevice->CreateRenderTargetView(m_pBackBuffer, NULL, &m_rtvRenderTargetView);

	// TODO: PART 1 STEP 5
	hr = m_snSwapChain->GetDesc(&m_scDesc);

	m_mxWorldMatrix.TranslateMatrix(0.0f, 0.0f, -3.0f);

#pragma region Camera

	//Making a translation matrix
	CameraTranslationMatrix.TranslateMatrix(0.0f, 0.0f, 0.0f);
	///Making the Rotation
	CameraRotationMatrix.MakeThisIdentity();
	//multiplying the translation on to the rotation
	m_mxViewMatrix = CameraRotationMatrix * CameraTranslationMatrix;
	//inversing the the viewMatrix
	// whether or not the camera is going up or down;
	CameraUpDown = false;

#pragma endregion

#pragma region Setting the Depth Projection

	m_mxProjectionMatrix = CreateProjectionMatrix();

#pragma endregion

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

#pragma region Create Cube
	//Create the geometry here
	ID3D11Buffer* StarBuffer;
	maxVerts = _countof(Cube_data);

	for (unsigned int i = 0; i < maxVerts; i++)
	{
		for (int posIndex = 0; posIndex < 3; posIndex++)
		{
			Cube[i].position[posIndex] = Cube_data[i].pos[posIndex];

			if (posIndex == 2)
			{
				Cube[i].w = 1;
				Cube[i].Setcolor(1, Cube_data[i].uvw[1], Cube_data[i].uvw[0], 0);
			}
		}
	}

	for (unsigned int i = 0; i < maxVerts; i++)
	{
		for (int posIndex = 0; posIndex < 2; posIndex++)
		{
			Cube[i].uv[posIndex] = Cube_data[i].uvw[posIndex];
		}
	}

	//// TODO: PART 2 STEP 3b
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = NULL;
	BufferDesc.ByteWidth = sizeof(Vertex) * maxVerts;
	BufferDesc.MiscFlags = 0;

	//// TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = Cube;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	// TODO: PART 2 STEP 3d
	m_iDevice->CreateBuffer(&BufferDesc, &InitData, &StarBuffer);

	animate.frame = 0;
	animate.maxFrame = 4;
	animate.width = numbers_test_width;

	GeometryContainer.push_back(StarBuffer);
#pragma endregion

#pragma region IndexBuffer
	maxIndices = _countof(Cube_indicies);

	D3D11_BUFFER_DESC IndexBufferDesc;
	IndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = NULL;
	IndexBufferDesc.ByteWidth = sizeof(unsigned int)*maxIndices;
	IndexBufferDesc.MiscFlags = 0;

	//// TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA InitIndexData;
	InitIndexData.pSysMem = Cube_indicies;
	InitIndexData.SysMemPitch = 0;
	InitIndexData.SysMemSlicePitch = 0;
	m_iDevice->CreateBuffer(&IndexBufferDesc, &InitIndexData, &IndexBuffer);
#pragma endregion

	// TODO: PART 2 STEP 8a
	D3D11_INPUT_ELEMENT_DESC element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = m_iDevice->CreateInputLayout(element, 3, VertexShader, sizeof(VertexShader), &m_pVertexInput);
	// setting the Constant variables
	m_objObject.matrix_objectWorld = m_mxWorldMatrix;
	m_Grid.matrix_objectWorld = m_mxWorldMatrix;

#pragma region TextureResource

	//Filtering
	D3D11_SAMPLER_DESC SamplerDesc;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.MinLOD = -FLT_MAX;
	SamplerDesc.MaxLOD = FLT_MAX;
	SamplerDesc.MipLODBias = 0.0f;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.BorderColor[0] = 1.0f;
	SamplerDesc.BorderColor[1] = 1.0f;
	SamplerDesc.BorderColor[2] = 1.0f;
	SamplerDesc.BorderColor[3] = 1.0f;

	m_iDevice->CreateSamplerState(&SamplerDesc, &SamplerState);

	//Texture2D Setup
	length = (numbers_test_width / 4);
	D3D11_TEXTURE2D_DESC TextureDesc;
	//zero-ing it out
	ZeroMemory(&TextureDesc, sizeof(TextureDesc));

	TextureDesc.Width = numbers_test_width;
	TextureDesc.Height = numbers_test_height;
	TextureDesc.MipLevels = numbers_test_numlevels;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;

	// subresource set up
	D3D11_SUBRESOURCE_DATA * numbers_test = new D3D11_SUBRESOURCE_DATA[9];
	ZeroMemory(numbers_test, sizeof(D3D11_SUBRESOURCE_DATA) * 9);


	for (int i = 0; i < numbers_test_numlevels; i++)
	{
		numbers_test[i].pSysMem = &numbers_test_pixels[numbers_test_leveloffsets[i]];
		numbers_test[i].SysMemPitch = (numbers_test_width >> i) * 4;
		numbers_test[i].SysMemSlicePitch = 0;
	}

	hr = m_iDevice->CreateTexture2D(&TextureDesc, numbers_test, &m_ImageTexture);
	delete[] numbers_test;

	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceView_Desc;
	ZeroMemory(&ShaderResourceView_Desc, sizeof(ShaderResourceView_Desc));
	ShaderResourceView_Desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	ShaderResourceView_Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceView_Desc.Texture2D.MipLevels = TextureDesc.MipLevels;
	ShaderResourceView_Desc.Texture2D.MostDetailedMip = 0;

	m_iDevice->CreateShaderResourceView(m_ImageTexture, &ShaderResourceView_Desc, &m_ShaderResource);

#pragma endregion

	// TODO: PART 3 STEP 3
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.ByteWidth = sizeof(Object);
	cBufferDesc.MiscFlags = 0;

	//subData
	D3D11_SUBRESOURCE_DATA ConstantBufferData;
	ConstantBufferData.pSysMem = &m_objObject;
	ConstantBufferData.SysMemPitch = 0;
	ConstantBufferData.SysMemSlicePitch = 0;

	hr = m_iDevice->CreateBuffer(&cBufferDesc, &ConstantBufferData, &m_ConstantBuffer[OBJECT]);

	//\//scene Vshader connnection
	m_scnMatrices.matrix_Projection = m_mxProjectionMatrix;
	Matrix m_mxCopyView;
	m_mxCopyView = m_mxViewMatrix;
	m_mxCopyView.InverseOrthogonalAffinedMatrix();
	m_scnMatrices.matrix_sceneCamera = m_mxViewMatrix;

	//setting the width of each scene
	cBufferDesc.ByteWidth = sizeof(Scene);
	ConstantBufferData.pSysMem = &m_scnMatrices;

	hr = m_iDevice->CreateBuffer(&cBufferDesc, &ConstantBufferData, &m_ConstantBuffer[SCENE]);

	cBufferDesc.ByteWidth = sizeof(ANIMATION);
	ConstantBufferData.pSysMem = &animate;

	m_iDevice->CreateBuffer(&cBufferDesc, &ConstantBufferData, &m_ConstantBuffer[TIMER]);

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

	m_dcConext->PSSetShaderResources(0, 1, &m_ShaderResource);
	//Binding the RendertTarget
	m_dcConext->OMSetRenderTargets(1, &m_rtvRenderTargetView, m_DepthView);

	//binding the viewports
	m_dcConext->RSSetViewports(1, &m_vpViewPort);

	//setting the sampler
	m_dcConext->PSSetSamplers(0, 1, &SamplerState);

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

	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		CameraRotationMatrix = CameraRotationMatrix * RotationMatrixOnY_Axis(DegreesToRadians(1 * (float)dt.Delta()));
		moved = true;
	}
	else if (GetAsyncKeyState(VK_NUMPAD6))
	{
		CameraRotationMatrix = CameraRotationMatrix * RotationMatrixOnY_Axis(DegreesToRadians(-1 * (float)dt.Delta()));
		moved = true;
	}

	if (GetAsyncKeyState(VK_NUMPAD8))
	{
		CameraRotationMatrix = CameraRotationMatrix * RotationMatrixOnX_Axis(DegreesToRadians(1 * (float)dt.Delta()));
		moved = true;
	}
	else if (GetAsyncKeyState(VK_NUMPAD2))
	{
		CameraRotationMatrix = CameraRotationMatrix * RotationMatrixOnX_Axis(DegreesToRadians(-1 * (float)dt.Delta()));
		moved = true;
	}

	//if (GetAsyncKeyState(VK_LBUTTON) && LbuttonDown == true)
	//{
	//	GetCursorPos(&startPoint);
	//	ScreenToClient(window, &startPoint);
	//	CurrentPoint = startPoint;
	//	LbuttonDown = false;
	//}
	//else if (GetAsyncKeyState(VK_LBUTTON))
	//{
	//	MouseMovement(moved, (float)dt.Delta());
	//}
	//if ((GetKeyState(VK_LBUTTON) & 0x80) == 0)
	//{
	//	LbuttonDown = true;
	//}

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

	if ((GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_RETURN)) & 0x1)
	{

	}

	//if ((GetKeyState(VK_LBUTTON) & 0x100) != 0)
	//{
	//	MouseMovement(moved, (float)dt.Delta());

	//}

	if (moved == false)
	{
		CameraRotationMatrix.MakeThisIdentity();
	}

	if (moved)
	{
		m_mxViewMatrix.SetTranslate(0, 0, 0);

		m_mxViewMatrix = m_mxViewMatrix * CameraRotationMatrix;

		m_mxViewMatrix.SetTranslate(CameraX, CameraY, CameraZ);

		Matrix m_mxCopyView = m_mxViewMatrix;

		m_mxCopyView.InverseOrthogonalAffinedMatrix();

		m_scnMatrices.matrix_sceneCamera = m_mxCopyView;


	}

#pragma endregion


#pragma region Animation

	D3D11_MAPPED_SUBRESOURCE TimerResource;
	ZeroMemory(&TimerResource, sizeof(TimerResource));

	m_dcConext->Map(m_ConstantBuffer[TIMER], 0, D3D11_MAP_WRITE_DISCARD, NULL, &TimerResource);

#pragma region FrameUpdate

	delta += (float)dt.Delta();

	if (delta >= 1)
	{
		animate.frame++;

		if (animate.maxFrame <= animate.frame)
		{
			animate.frame = 0;
		}

		delta = 0;
	}

#pragma endregion

	memcpy(TimerResource.pData, &animate, sizeof(animate));

	m_dcConext->Unmap(m_ConstantBuffer[TIMER], 0);

#pragma endregion


#pragma region Cube
	//Constant buffer manipulation
	D3D11_MAPPED_SUBRESOURCE mapResource;

	//\// World Matrix
	m_dcConext->Map(m_ConstantBuffer[OBJECT], 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapResource);


	Matrix YRotation;
	YRotation.MakeThisIdentity();
	YRotation = RotationMatrixOnY_Axis((float)dt.Delta() / 2);

	Matrix XRotation;
	XRotation.MakeThisIdentity();
	XRotation = RotationMatrixOnX_Axis((float)dt.Delta() / 2);

	float translation[3];

	m_objObject.matrix_objectWorld.InverseOrthogonalAffinedMatrix();

	Vertex trans = m_objObject.matrix_objectWorld.GetRow(3);

	translation[0] = trans[0];
	translation[1] = trans[1];
	translation[2] = trans[2];

	m_objObject.matrix_objectWorld.SetTranslate(0, 0, 0);

	m_objObject.matrix_objectWorld = m_objObject.matrix_objectWorld * YRotation;

	m_objObject.matrix_objectWorld = XRotation* m_objObject.matrix_objectWorld;

	m_objObject.matrix_objectWorld.InverseOrthogonalAffinedMatrix();

	m_objObject.matrix_objectWorld.SetTranslate(0, 0.5f, -3);

	memcpy(mapResource.pData, &m_objObject, sizeof(m_objObject));

	m_dcConext->Unmap(m_ConstantBuffer[OBJECT], 0);

	D3D11_MAPPED_SUBRESOURCE SceneResource;
	ZeroMemory(&SceneResource, sizeof(SceneResource));

	m_dcConext->Map(m_ConstantBuffer[SCENE], 0, D3D11_MAP_WRITE_DISCARD, NULL, &SceneResource);

	memcpy(SceneResource.pData, &m_scnMatrices, sizeof(m_scnMatrices));

	m_dcConext->Unmap(m_ConstantBuffer[SCENE], 0);

#pragma endregion 

	m_dcConext->PSSetShader(m_shaderPS, NULL, 0);
	m_dcConext->VSSetShader(m_shaderVS, NULL, 0);

	m_dcConext->VSSetConstantBuffers(0, 2, m_ConstantBuffer);

	m_dcConext->PSSetConstantBuffers(0, 1, &m_ConstantBuffer[TIMER]);

	// Seeting the vertex buffer/shaders/inputlayout/topology
	unsigned int stride = sizeof(Vertex);
	unsigned int zero = 0;

	m_dcConext->IASetVertexBuffers(0, 1, &GeometryContainer[0], &stride, &zero);

	m_dcConext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_dcConext->IASetInputLayout(m_pVertexInput);

	m_dcConext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//setting the renderstate
	m_dcConext->RSSetState(m_pRasterStateFrontCull);
	m_dcConext->DrawIndexed(maxIndices, 0, 0);

	//setting the renderstate
	m_dcConext->RSSetState(m_pRasterState);
	m_dcConext->DrawIndexed(maxIndices, 0, 0);
#pragma region Grid

	D3D11_MAPPED_SUBRESOURCE GridmapResource;
	//\// World Matrix
	m_dcConext->Map(m_ConstantBuffer[OBJECT], 0, D3D11_MAP_WRITE_DISCARD, NULL, &GridmapResource);

	memcpy(GridmapResource.pData, &m_Grid, sizeof(m_Grid));

	m_dcConext->Unmap(m_ConstantBuffer[OBJECT], 0);

#pragma endregion

	m_dcConext->PSSetShader(m_shaderPS, NULL, 0);
	m_dcConext->VSSetShader(m_shaderVS, NULL, 0);

#pragma region Drawing the Grid
	//Constant buffer manipulation
	m_dcConext->VSSetConstantBuffers(0, 2, m_ConstantBuffer);
	// drawing
	m_dcConext->IASetVertexBuffers(0, 1, &GeometryContainer[1], &stride, &zero);

	m_dcConext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	//setting the renderstate
	m_dcConext->RSSetState(m_pRasterStateFrontCull);

	m_dcConext->Draw(maxGridIndices, 0);

	//setting the renderstate

	m_dcConext->RSSetState(m_pRasterState);
	m_dcConext->Draw(maxGridIndices, 0);

#pragma endregion

	//Swaping the back buffer info with the front buffer
	m_snSwapChain->Present(1, 0);
	// END OF PART 1
	return true;
}
void DEMO_APP::MouseMovement(bool& move, float dt)
{
	RECT WinRect;

	GetCursorPos(&CurrentPoint);
	ScreenToClient(window, &CurrentPoint);

	GetClientRect(window, &WinRect);

	if (WinRect.left > CurrentPoint.x || WinRect.right < CurrentPoint.x || WinRect.top > CurrentPoint.y || WinRect.bottom < CurrentPoint.y)
	{
		return;
	}


	if (DistanceFormula(startPoint, CurrentPoint) < 10)
	{
		return;
	}

	move = true;

	TSize Screen;
	Screen.m_nHeight = WinRect.bottom;
	Screen.m_nWidth = WinRect.right;

	TFPoint rotation;
	rotation.x = float(CurrentPoint.x - startPoint.x);

	rotation.y = float(CurrentPoint.y - startPoint.y);

	if (rotation.x > 0)
	{
		CameraRotationMatrix = CameraRotationMatrix * RotationMatrixOnY_Axis(DegreesToRadians((float)(1 * dt)));
	}
	else if (rotation.x < 0)
	{
		CameraRotationMatrix = CameraRotationMatrix * RotationMatrixOnY_Axis(DegreesToRadians((float)(-1 * dt)));
	}

	if (rotation.y > 0)
	{
		CameraRotationMatrix = CameraRotationMatrix * RotationMatrixOnX_Axis(DegreesToRadians((float)(1 * dt)));
	}
	else if (rotation.y < 0)
	{
		CameraRotationMatrix = CameraRotationMatrix * RotationMatrixOnX_Axis(DegreesToRadians((float)(-1 * dt)));
	}
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

	for (unsigned int i = 0; i < GeometryContainer.size(); i++)
	{
		GeometryContainer[i]->Release();
	}

	GeometryContainer.clear();

	m_ConstantBuffer[OBJECT]->Release();
	m_ConstantBuffer[SCENE]->Release();
	delete[] m_ConstantBuffer;

	IndexBuffer->Release();
	//IndexBuffer[1]->Release();

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