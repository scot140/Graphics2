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
#include <algorithm>
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
#include "../VS_Instancing.csh"
#include "../PS_MultiTexture.csh"

#include "../Light_VertexShader.csh"
#include "../DirectionalLight_PS.csh"
#include "../PShader_SpotLight.csh"
#include "../PointLightShader.csh"

#include "../VS_NormalMapping.csh"
#include "../PS_NormalMapping.csh"
#include "../PS_NormalMapPoint.csh"
#include "../PS_NormalMapSpot.csh"
#include "../PS_SpecularSpotLight.csh"

#define BACKBUFFER_WIDTH	800
#define BACKBUFFER_HEIGHT	800
#define OBJECT				0
#define SCENE				1
#define TIMER				2

#define FIRSTLIGHT			0
#define SECONDLIGHT			1

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

bool LbuttonDown = false;
POINT startPoint;

Scene m_miniScene;
Scene SceneMatrices;

struct BufferInput
{
	ID3D11Buffer** ConstPixel;
	unsigned int numPixelSlot;
	ID3D11Buffer** ConstVertex;
	unsigned int numVertexSlot;
};

struct ThreadStruct
{
	string file;
	wstring filename;
	Model* p_model;
	D3D11_SAMPLER_DESC* p_sampler;
	wstring Secondfilename;
	unsigned int numInstance;
	InstanceType* instances;
	bool* m_lock = false;
	mutex* m_pThreadLock;
	condition_variable* m_pCondition;
};

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
	ID3D11InputLayout* m_pInstanceInput;

	ID3D11Texture2D* m_pBackBuffer;
	ID3D11RasterizerState* m_pRasterState;
	D3D11_VIEWPORT m_vpSecondaryView;
	DXGI_SWAP_CHAIN_DESC m_scDesc;

	// TODO: PART 2 STEP 4

	//misc shaders
	ID3D11PixelShader*	m_shaderPS;
	ID3D11PixelShader* m_DefaultPS;
	ID3D11VertexShader* m_shaderVS;
	ID3D11PixelShader* m_SkyboxPS;
	ID3D11VertexShader* m_SkyboxVS;
	ID3D11PixelShader* m_MultiTexturePS;
	ID3D11VertexShader* m_vsInstancing;

	//Lights
	ID3D11PixelShader*	m_DLightPS;
	ID3D11VertexShader* m_LightVS;
	ID3D11PixelShader* m_PLightPS;
	ID3D11PixelShader* m_SpLightPS;

	//Normal mapping
	ID3D11VertexShader* m_vsNormalMap;
	ID3D11PixelShader* m_psNormalMap;
	ID3D11PixelShader* m_psNormalMapPoint;
	ID3D11PixelShader* m_psNormalMapSpot;
	ID3D11PixelShader* m_psNormalSpecSpot;

	//depth perspective
	ID3D11Texture2D* m_ZBuffer;
	ID3D11DepthStencilView* m_DepthView;
	ID3D11DepthStencilState * m_pDepthState;
	ID3D11PixelShader * m_PScurrentShader;
	ID3D11PixelShader * m_PScurrentNormalShader;

	//Blending
	ID3D11BlendState* m_pBlendState;
	ID3D11BlendState* m_pAlphaCoverage;
	ID3D11RasterizerState* m_pRasterStateFrontCull;
	ID3D11RasterizerState* m_pSkyBoxRasterState;
	ID3D11RasterizerState* m_pCuberaster;
	ID3D11RasterizerState* MSAA;

	ID3D11Debug *m_dgDebug;

	//camera

	//Math
	XMFLOAT4X4 m_mxWorldMatrix;
	XMFLOAT4X4 m_mxViewMatrix;
	XMFLOAT4X4 m_mxProjectonMatrix;

	//view port
	XMFLOAT4X4 m_mxMiniViewMatrix;
	XMFLOAT4X4 m_mxMiniProjectionMatrix;

	//misc
	XMFLOAT4X4 rotation;
	POINT distance;

	void MouseMovement(bool& move, float dt, XMFLOAT4X4* rot);

	bool displayWindow = false;
	bool fullscreen = false;
	bool CursorClientCheck(POINT curr);

	Object m_objMatrix;
	PtLight m_plgLight[2];
	LIGHTING m_lgLight[2];
	SptLight m_sptLight[2];

	Scene m_scnMatrix;
	ANIMATION m_aniAnimation;

	//buffer  
	ID3D11Buffer* m_pConstBufferAnimation_PS;
	ID3D11Buffer* m_pConstBuffer[2];
	ID3D11Buffer* m_pConstBufferLight_PS;
	ID3D11Buffer* m_pCBufferPointLight_PS;
	ID3D11Buffer* m_pCBufferSpotLight_PS;

	ID3D11Buffer* m_InstanceBuffer;

	BufferInput ModelPixelInput;
	BufferInput ModelNormalMap;
	float ptTransX = 0;
	bool flip = false;

	//Render To Texture
	Scene RenderTTexture;
	ID3D11Texture2D*			m_RttCubeRenderTarget;
	ID3D11RenderTargetView*			m_RttrtvToCube;
	ID3D11ShaderResourceView*		m_RTTShaderResource;
	D3D11_VIEWPORT					m_RttvpCubeViewport;

	XMFLOAT4X4 m_RttmxCubeViewMatrix;
	XMFLOAT4X4 m_RttmxCubeProjectonMatrix;

	//depth perspective
	ID3D11Texture2D*			m_RttZBuffer;
	ID3D11DepthStencilView*		m_RttDepthView;
	ID3D11DepthStencilState *	m_RttpDepthState;

	bool ToggleFill = false;
	bool ToggleAliasing = true;
public:
	//Constbuffer manipulators
	LIGHTING DirectionLight;
	PtLight PointLight;
	SptLight SpotLight;

	LIGHTING DirectionLight2;
	PtLight PointLight2;
	SptLight SpotLight2;

	unsigned int InstanceCount;

	float dirX = -1;
	float dirY = -1;

	///models
	Model m_StarModel;
	Model m_StarModel2;
	Model Pyramid;
	Model Dorumon;
	Model SkyBox;
	Model DinoTiger;
	Model Ground;
	Model DoruGreymon;
	Model Dorugamon;
	Model Dorugoramon;
	Model m_multiStarModel[3];
	Model m_mdCube;
	Model Tree;


	DEMO_APP(HINSTANCE hinst, WNDPROC proc);

	void CreateStar(D3D11_SAMPLER_DESC * p_sampler);

	void CreateStar(Model* star, unsigned int numStars, D3D11_SAMPLER_DESC * p_sampler);

	void CreateObj(const char* file, Model& p_model, D3D11_SUBRESOURCE_DATA* p_data = nullptr, D3D11_TEXTURE2D_DESC* p_texture = nullptr, D3D11_SAMPLER_DESC* p_sampler = nullptr);

	void CreateObj(const char* file, Model& p_model, const wchar_t* filename, D3D11_SAMPLER_DESC* p_sampler, const wchar_t* Secondfilename = nullptr, unsigned int numInstance = 0, InstanceType* instances = nullptr);

	void MultiThreadCreateObj(ThreadStruct* p_thread);

	void CreateObjCube(const char* file, Model& p_model, const wchar_t* filename, D3D11_SAMPLER_DESC* p_sampler, const wchar_t* Secondfilename = nullptr);

	void DrawObj(Model* p_model, BufferInput*p_input, ID3D11VertexShader* p_shaderVS, ID3D11PixelShader* p_shaderPS, ID3D11RasterizerState** raster, unsigned int size);
	void DrawInstanceObj(Model* p_model, BufferInput* p_input, ID3D11VertexShader* p_shaderVS, ID3D11PixelShader* p_shaderPS, ID3D11RasterizerState** raster, unsigned int size);
	void DrawStar(Model* p_model);

	void DEMO_APP::CreateThreadStruct(ThreadStruct* p_struct, string p_file, wstring p_filename, Model*	p_pmodel, D3D11_SAMPLER_DESC* p_psampler, wstring	p_Secondfilename = L"\0", unsigned int p_numInstance = 0, InstanceType* p_instances = nullptr);


	void DEMO_APP::DrawStars(Model* p_star); // only use for star models

	void TransparentStarsDraw();

	void UpdateSkyBox(XMFLOAT4X4& rot);
	void CreateSkyBox(D3D11_SAMPLER_DESC * p_sampler);
	void ChangeProjectionMatrix();
	void CreateRenderToTexture();
	void MappingObjMatrix(D3D11_MAPPED_SUBRESOURCE& p_Scene, Object& p_Matrix);
	void MappingAnimation(D3D11_MAPPED_SUBRESOURCE& p_Scene, Model& p_Model);
	void MappingLighting(D3D11_MAPPED_SUBRESOURCE& p_Scene, LIGHTING& p_light, LIGHTING& p_light2);
	void MappingPointLight(D3D11_MAPPED_SUBRESOURCE& p_Scene, PtLight& p_light, PtLight& p_light2);
	void MappingSpotLight(D3D11_MAPPED_SUBRESOURCE& p_Scene, SptLight& p_light, SptLight& p_light2);
	void RedrawSceneBuffer(D3D11_MAPPED_SUBRESOURCE& p_Scene, Scene& p_Matrix);
	void RenderToTextureSceneBuffer(D3D11_MAPPED_SUBRESOURCE& p_Scene, Scene& p_Matrix);
	void CreateConstBuffers();
	void ToggleFillMode(ID3D11RasterizerState** raster, D3D11_FILL_MODE fill = D3D11_FILL_SOLID);
	void BackAndForth(float _comp, float& p_trans);
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
	CreateSwapChainDesc(&m_scDesc, &window, 1, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT);

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
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.AntialiasedLineEnable = true;
	m_iDevice->CreateRasterizerState(&rasterizerState, &m_pRasterState);

	rasterizerState.CullMode = D3D11_CULL_FRONT;
	rasterizerState.FrontCounterClockwise = false;

	hr = m_iDevice->CreateRasterizerState(&rasterizerState, &m_pRasterStateFrontCull);

	if (hr == E_INVALIDARG)
	{
		return;
	}

	rasterizerState.MultisampleEnable = true;
	rasterizerState.AntialiasedLineEnable = true;

	hr = m_iDevice->CreateRasterizerState(&rasterizerState, &MSAA);

	if (hr == E_INVALIDARG)
	{
		return;
	}

#pragma endregion

#pragma region BlendState

	D3D11_BLEND_DESC BlendDesc;

	DefaultBlendDesc(&BlendDesc);

	m_iDevice->CreateBlendState(&BlendDesc, &m_pBlendState);

	BlendDesc.AlphaToCoverageEnable = true;

	m_iDevice->CreateBlendState(&BlendDesc, &m_pAlphaCoverage);

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
	m_iDevice->CreateVertexShader(VS_Instancing, sizeof(VS_Instancing), NULL, &m_vsInstancing);

	m_iDevice->CreatePixelShader(Skybox_PS, sizeof(Skybox_PS), NULL, &m_SkyboxPS);
	m_iDevice->CreateVertexShader(Skybox_VS, sizeof(Skybox_VS), NULL, &m_SkyboxVS);
	m_iDevice->CreatePixelShader(PS_MultiTexture, sizeof(PS_MultiTexture), NULL, &m_MultiTexturePS);

	m_iDevice->CreateVertexShader(Light_VertexShader, sizeof(Light_VertexShader), NULL, &m_LightVS);
	m_iDevice->CreatePixelShader(DirectionalLight_PS, sizeof(DirectionalLight_PS), NULL, &m_DLightPS);
	m_iDevice->CreatePixelShader(PointLightShader, sizeof(PointLightShader), NULL, &m_PLightPS);
	m_iDevice->CreatePixelShader(PShader_SpotLight, sizeof(PShader_SpotLight), NULL, &m_SpLightPS);
	m_iDevice->CreateVertexShader(VS_NormalMapping, sizeof(VS_NormalMapping), NULL, &m_vsNormalMap);


	m_iDevice->CreatePixelShader(PS_NormalMapping, sizeof(PS_NormalMapping), NULL, &m_psNormalMap);
	m_iDevice->CreatePixelShader(PS_NormalMapPoint, sizeof(PS_NormalMapPoint), NULL, &m_psNormalMapPoint);
	m_iDevice->CreatePixelShader(PS_NormalMapSpot, sizeof(PS_NormalMapSpot), NULL, &m_psNormalMapSpot);
	m_iDevice->CreatePixelShader(PS_SpecularSpotLight, sizeof(PS_SpecularSpotLight), NULL, &m_psNormalSpecSpot);

	// TODO: PART 2 STEP 8a
	D3D11_INPUT_ELEMENT_DESC element[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TAN", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "PADDING", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_iDevice->CreateInputLayout(element, 6, VertexShader, sizeof(VertexShader), &m_pVertexInput);

	D3D11_INPUT_ELEMENT_DESC InstanceElement[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TAN", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "IPOSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	m_iDevice->CreateInputLayout(InstanceElement, 6, VS_Instancing, sizeof(VS_Instancing), &m_pInstanceInput);

	//DirectionLight
	DirectionLight.dir = XMFLOAT3(-1, -1, 0);
	DirectionLight.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectionLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	DirectionLight2.dir = XMFLOAT3(1, 1, 0);
	DirectionLight2.ambient = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	DirectionLight2.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	CreateConstBuffers();
	CreateRenderToTexture();

	//cube model
	XMMATRIX temp;

	//Pyramid Creation
	D3D11_SAMPLER_DESC SamplerDesc;
	DefaultSamplerStateDesc(&SamplerDesc);

	CreateStar(&SamplerDesc);

	m_StarModel.ScaleModel(0.5);

	CreateStar(&m_StarModel2, 1, &SamplerDesc);
	m_StarModel2.ScaleModel(0.5);

	CreateStar(m_multiStarModel, 3, &SamplerDesc);



	temp = XMMatrixTranslation(0, 0, 2) * XMLoadFloat4x4(&m_multiStarModel[0].m_objMatrix.m_mxConstMatrix);

	XMStoreFloat4x4(&m_multiStarModel[1].m_objMatrix.m_mxConstMatrix, temp);

	temp = XMMatrixTranslation(0, 0, 1) * XMLoadFloat4x4(&m_multiStarModel[0].m_objMatrix.m_mxConstMatrix);

	XMStoreFloat4x4(&m_multiStarModel[2].m_objMatrix.m_mxConstMatrix, temp);

	CreateSkyBox(&SamplerDesc);

	SkyBox.ScaleModel(50);

	Pyramid.SetAnimation(4, (float)512);



	temp = XMMatrixTranslation(1.0f, 1.0f, 0) * XMLoadFloat4x4(&m_mxWorldMatrix);

	XMStoreFloat4x4(&Pyramid.m_objMatrix.m_mxConstMatrix, temp);

	//Drawing a dorumon
	InstanceType* triangle;
	triangle = new InstanceType[4];
	triangle[0].pos = XMFLOAT4(-2.5f, 2.5f, 0, 1.0f);
	triangle[1].pos = XMFLOAT4(-2.5f, 0, 0, 1.0f);
	triangle[2].pos = XMFLOAT4(2.5f, 2.5f, 0, 1.0f);
	triangle[3].pos = XMFLOAT4(2.5f, 0, 0, 1.0f);

	vector<thread*> vt_ThreadHolder;

	ThreadStruct ModelInfo[6];

	mutex LoaderMutex;
	condition_variable condition;
	bool locked;
	CreateThreadStruct(&ModelInfo[0], "resource/Models/Dorumon.obj", L"resource/Texture/Dorumon.dds", &Dorumon, &SamplerDesc, L"\0", 4, triangle);
	CreateThreadStruct(&ModelInfo[1], "resource/Models/Pyramid.obj", L"resource/Texture/numbers_test.dds", &Pyramid, &SamplerDesc);
	CreateThreadStruct(&ModelInfo[2], "resource/Models/DinoTigermon.obj", L"resource/Texture/DinoTigermon.dds", &DinoTiger, &SamplerDesc);
	CreateThreadStruct(&ModelInfo[3], "resource/Models/DoruGreymon.obj", L"resource/Texture/DoruGreymon.dds", &DoruGreymon, &SamplerDesc);
	CreateThreadStruct(&ModelInfo[4], "resource/Models/Dorugoramon.obj", L"resource/Texture/Dorugoramon.dds", &Dorugoramon, &SamplerDesc, L"resource/Normals/Dorugoramon.dds");
	CreateThreadStruct(&ModelInfo[5], "resource/Models/Dorugamon.obj", L"resource/Texture/Dorugamon.dds", &Dorugamon, &SamplerDesc, L"resource/Normals/Dorugamon2.dds");

	//CreateObj(, Pyramid, L"resource/Texture/numbers_test.dds", &SamplerDesc);
	//CreateObj("resource/Models/Dorumon.obj", Dorumon, L"resource/Texture/Dorumon.dds", &SamplerDesc, nullptr, 4, triangle);
	//CreateObj("resource/Models/DinoTigermon.obj", DinoTiger, L"resource/Texture/DinoTigermon.dds", &SamplerDesc);
	//CreateObj("resource/Models/DoruGreymon.obj", DoruGreymon, L"resource/Texture/DoruGreymon.dds", &SamplerDesc);
	//CreateObj("resource/Models/Dorugoramon.obj", Dorugoramon, L"resource/Texture/Dorugoramon.dds", &SamplerDesc, L"resource/Normals/Dorugoramon.dds");
	//CreateObj("resource/Models/Dorugamon.obj", Dorugamon, L"resource/Texture/Dorugamon.dds", &SamplerDesc, L"resource/Normals/Dorugamon2.dds");


	for (int i = 0; i < 6; i++)
	{
		ModelInfo[i].m_pCondition = &condition;
		ModelInfo[i].m_pThreadLock = &LoaderMutex;
		ModelInfo[i].m_lock = &locked;
		thread * temp = new thread(&DEMO_APP::MultiThreadCreateObj, this, &ModelInfo[i]);
		//temp->detach();
		vt_ThreadHolder.push_back(temp);
	}

	//Looping through the vector to join the main thread
	for each (std::thread* var in vt_ThreadHolder)
	{
		//Joining the thread
		var->join();
	}

#pragma region Cleaning the Vector
	for (int i = vt_ThreadHolder.size() - 1; vt_ThreadHolder.size() > 0;)
	{
		if (vt_ThreadHolder[i])
		{
			//assigning the temp thread to the dynamic thread
			std::thread * temp = vt_ThreadHolder[i];
			//cleaning up
			delete temp;
			//clearing the garbage memory
			vt_ThreadHolder.pop_back();
			//reassigning the index
			i = vt_ThreadHolder.size() - 1;
		}
	}
#pragma endregion


	delete[] triangle;
	triangle = nullptr;

	temp = XMLoadFloat4x4(&m_mxWorldMatrix) * XMMatrixTranslation(0, 0, -3);

	temp = XMMatrixRotationY(XMConvertToRadians(180)) * temp;

	XMStoreFloat4x4(&Dorumon.m_objMatrix.m_mxConstMatrix, temp);

	Dorumon.ScaleModel(0.5f);



	CreateObjCube("resource/Models/Tree.obj", Tree, L"resource/Texture/Tree.dds", &SamplerDesc);
	temp = XMLoadFloat4x4(&m_mxWorldMatrix) * XMMatrixTranslation(2, 0, 2);

	//temp = XMMatrixRotationY(XMConvertToRadians(180)) * temp;

	XMStoreFloat4x4(&Tree.m_objMatrix.m_mxConstMatrix, temp);

	Tree.ScaleModel(0.25f);

#pragma region Drawing a DinoTigermon


	temp = XMLoadFloat4x4(&m_mxWorldMatrix) * XMMatrixTranslation(0, 0, -3);

	temp = XMMatrixRotationY(XMConvertToRadians(180)) * temp;

	XMStoreFloat4x4(&DinoTiger.m_objMatrix.m_mxConstMatrix, temp);

	DinoTiger.ScaleModel(0.5f);

#pragma endregion

	XMFLOAT3 pos = DinoTiger.GetModelPosition();
	m_StarModel.SetModelPosition(pos.x, 3, pos.z);

#pragma region Creating Dorugreymon

	temp = XMLoadFloat4x4(&m_mxWorldMatrix) * XMMatrixTranslation(0, 0, 2.0f);

	temp = XMMatrixRotationY(XMConvertToRadians(180)) * temp;

	XMStoreFloat4x4(&DoruGreymon.m_objMatrix.m_mxConstMatrix, temp);

	DoruGreymon.ScaleModel(0.75f);
#pragma endregion

#pragma region Dorugoramon

	temp = XMLoadFloat4x4(&m_mxWorldMatrix) * XMMatrixTranslation(-1, 0, -1);

	temp = XMMatrixRotationY(XMConvertToRadians(180)) * temp;

	XMStoreFloat4x4(&Dorugoramon.m_objMatrix.m_mxConstMatrix, temp);

	Dorugoramon.ScaleModel(0.5f);

#pragma endregion

#pragma region Creating a Ground

	CreateObjCube("resource/Models/Ground1.obj", Ground, L"resource/Texture/Astral.dds", &SamplerDesc, L"resource/Texture/Night.dds");

	temp = XMLoadFloat4x4(&m_mxWorldMatrix) * XMMatrixTranslation(0.0f, -0.05f, 0.0f);

	XMStoreFloat4x4(&Ground.m_objMatrix.m_mxConstMatrix, temp);

	Ground.ScaleModel(3.0f);

#pragma endregion

#pragma region Creating Dorugamon

	temp = XMLoadFloat4x4(&m_mxWorldMatrix) * XMMatrixTranslation(2, 1.75f, -1);

	temp = XMMatrixRotationY(XMConvertToRadians(180)) * temp;

	XMStoreFloat4x4(&Dorugamon.m_objMatrix.m_mxConstMatrix, temp);

	Dorugamon.ScaleModel(0.25f);
#pragma endregion

	//Getting the cursor pos
	GetCursorPos(&startPoint);

	//Setting the current pixelshader to Direction light
	m_PScurrentShader = m_DLightPS;
	m_PScurrentNormalShader = m_psNormalMap;

	//BufferInput
	ModelPixelInput.numPixelSlot = 1;
	ModelPixelInput.ConstPixel = &m_pConstBufferLight_PS;
	ModelPixelInput.ConstVertex = m_pConstBuffer;
	ModelPixelInput.numVertexSlot = 2;

#pragma region Cube

	CreateObjCube("resource/Models/cube.obj", m_mdCube, L"resource/Texture/Blank.dds", &SamplerDesc);

	//m_mdCube.SetShaderResourceView(m_RTTShaderResource);

	temp = XMLoadFloat4x4(&m_mxWorldMatrix) * XMMatrixTranslation(2, 1.75f, -1);

	XMStoreFloat4x4(&m_mdCube.m_objMatrix.m_mxConstMatrix, temp);

#pragma endregion

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

#pragma region RenderToTexture

	UINT RTTsampleMask = 0xffffffff;

	m_dcConext->OMSetBlendState(nullptr, nullptr, RTTsampleMask);

	//Binding the depthState/ depthBuffer
	m_dcConext->OMSetDepthStencilState(m_RttpDepthState, 1);

	//Binding the RendertTarget
	m_dcConext->OMSetRenderTargets(1, &m_RttrtvToCube, m_RttDepthView);

	//binding the viewports
	m_dcConext->RSSetViewports(1, &m_RttvpCubeViewport);

	// clearing the screen to that color
	float RTTcolor[4];
	RTTcolor[0] = 0;
	RTTcolor[1] = 0;
	RTTcolor[2] = 1;
	RTTcolor[3] = 1;

	m_dcConext->ClearDepthStencilView(m_RttDepthView, D3D11_CLEAR_DEPTH, 1, 0);

	m_dcConext->ClearRenderTargetView(m_RttrtvToCube, RTTcolor);

#pragma region Dorugamon constantBuffer

	D3D11_MAPPED_SUBRESOURCE RTTresource;

	XMMATRIX RTTmatrix;

	RTTmatrix = XMLoadFloat4x4(&Dorugamon.m_objMatrix.m_mxConstMatrix);


	RTTmatrix = XMMatrixRotationY((float)dt.Delta()) * RTTmatrix;


	XMStoreFloat4x4(&Dorugamon.m_objMatrix.m_mxConstMatrix, RTTmatrix);

	MappingObjMatrix(RTTresource, Dorugamon.m_objMatrix);

	//////////////////////////////////////////////////////////////////////
	RedrawSceneBuffer(RTTresource, RenderTTexture);

#pragma endregion

	DrawObj(&Dorugamon, &ModelPixelInput, m_vsNormalMap, m_PScurrentNormalShader, nullptr, 0);

	m_snSwapChain->Present(1, 0);

	m_mdCube.SetShaderResourceView(m_RTTShaderResource);

#pragma endregion

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
	color[0] = 1;
	color[1] = 0;
	color[2] = 0;
	color[3] = 1;

	m_dcConext->ClearDepthStencilView(m_DepthView, D3D11_CLEAR_DEPTH, 1, 0);

	m_dcConext->ClearRenderTargetView(m_rtvRenderTargetView, color);

#pragma region InputChecker 

	float CameraX = 0, CameraY = 0, CameraZ = 0;

	bool moved = false;

	XMStoreFloat4x4(&rotation, XMMatrixIdentity());

	distance.x = 0;
	distance.y = 0;

	if (GetAsyncKeyState('1') & 0x1)
	{
		m_PScurrentShader = m_DLightPS;
		m_PScurrentNormalShader = m_psNormalMap;
		//BufferInput
		ModelPixelInput.numPixelSlot = 1;
		ModelPixelInput.ConstPixel = &m_pConstBufferLight_PS;
		ModelPixelInput.ConstVertex = m_pConstBuffer;
		ModelPixelInput.numVertexSlot = 2;
	}
	else if (GetAsyncKeyState('2') & 0x1)
	{
		m_PScurrentShader = m_PLightPS;
		m_PScurrentNormalShader = m_psNormalMapPoint;

		//BufferInput
		ModelPixelInput.numPixelSlot = 1;
		ModelPixelInput.ConstPixel = &m_pCBufferPointLight_PS;
		ModelPixelInput.ConstVertex = m_pConstBuffer;
		ModelPixelInput.numVertexSlot = 2;

	}
	else if (GetAsyncKeyState('3') & 0x1)
	{
		m_PScurrentShader = m_SpLightPS;
		m_PScurrentNormalShader = m_psNormalMapSpot;

		//BufferInput
		ModelPixelInput.numPixelSlot = 1;
		ModelPixelInput.ConstPixel = &m_pCBufferSpotLight_PS;
		ModelPixelInput.ConstVertex = m_pConstBuffer;
		ModelPixelInput.numVertexSlot = 2;
	}
	else if (GetAsyncKeyState('4') & 0x1)
	{
		m_PScurrentShader = m_SpLightPS;
		m_PScurrentNormalShader = m_psNormalSpecSpot;

		//BufferInput
		ModelPixelInput.numPixelSlot = 1;
		ModelPixelInput.ConstPixel = &m_pCBufferSpotLight_PS;
		ModelPixelInput.ConstVertex = m_pConstBuffer;
		ModelPixelInput.numVertexSlot = 2;
	}


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

	if (GetAsyncKeyState('M') & 0x1)
	{
		displayWindow = !displayWindow;
	}

	///Antialiased
	if (GetAsyncKeyState('T') & 0x1)
	{
		D3D11_RASTERIZER_DESC rasterizerState;
		SAFE_RELEASE(m_pRasterState);
		SAFE_RELEASE(m_pRasterStateFrontCull);
		DefaultRasterizerDesc(&rasterizerState);
		rasterizerState.FillMode = D3D11_FILL_WIREFRAME;

		ToggleAliasing = !ToggleAliasing;

		if (ToggleAliasing)
		{

			rasterizerState.FrontCounterClockwise = false;
			rasterizerState.MultisampleEnable = true;
			m_iDevice->CreateRasterizerState(&rasterizerState, &m_pRasterState);

			rasterizerState.CullMode = D3D11_CULL_FRONT;
			rasterizerState.FrontCounterClockwise = false;

			m_iDevice->CreateRasterizerState(&rasterizerState, &m_pRasterStateFrontCull);
		}
		else
		{
			rasterizerState.FrontCounterClockwise = false;
			rasterizerState.MultisampleEnable = false;
			m_iDevice->CreateRasterizerState(&rasterizerState, &m_pRasterState);

			rasterizerState.CullMode = D3D11_CULL_FRONT;
			rasterizerState.FrontCounterClockwise = false;

			m_iDevice->CreateRasterizerState(&rasterizerState, &m_pRasterStateFrontCull);
		}
	}

	if (GetAsyncKeyState(VK_LBUTTON))
	{
		POINT CurrentPoint;
		GetCursorPos(&CurrentPoint);


		if (CursorClientCheck(CurrentPoint))
		{
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
		XMVECTOR pos = XMLoadFloat3(&XMFLOAT3(0, 2, -4));
		XMVECTOR focus = XMLoadFloat3(&XMFLOAT3(0, 0, 0));
		XMVECTOR height = XMLoadFloat3(&XMFLOAT3(0, 1, 0));

		//Creating the Camera
		view = XMMatrixLookAtLH(pos, focus, height);
		//Creating the Camera
		XMStoreFloat4x4(&m_mxViewMatrix, view);

		SceneMatrices.matrix_sceneCamera = m_mxViewMatrix;

		//Skybox
		view = XMMatrixInverse(nullptr, view);
		XMStoreFloat4x4(&SkyBox.m_objMatrix.m_mxConstMatrix, view);
	}


	if (moved)
	{
		XMMATRIX CopyView, inverseCopy;

		/***************************************************************************************************************/

		CopyView = XMLoadFloat4x4(&m_mxViewMatrix);
		CopyView = XMMatrixInverse(nullptr, CopyView);

		XMVECTOR ViewPos = CopyView.r[3];

		CopyView.r[3] = XMVectorSet(0, 0, 0, 1);

		CopyView = XMLoadFloat4x4(&rotation) * CopyView;

		CopyView.r[3] = ViewPos;

		CopyView = XMMatrixInverse(nullptr, CopyView);

		if (CameraX != 0 || CameraY != 0 || CameraZ != 0)
		{

			CopyView = CopyView * XMMatrixTranslation(CameraX, CameraY, CameraZ);

		}

		XMStoreFloat4x4(&m_mxViewMatrix, CopyView);

		SceneMatrices.matrix_sceneCamera = m_mxViewMatrix;

		/****************************************************************************/
		inverseCopy = XMLoadFloat4x4(&m_mxViewMatrix);

		inverseCopy = XMMatrixInverse(nullptr, inverseCopy);
		SkyBox.m_objMatrix.m_mxConstMatrix.m[3][0] = inverseCopy.r[3].m128_f32[0];
		SkyBox.m_objMatrix.m_mxConstMatrix.m[3][1] = inverseCopy.r[3].m128_f32[1];
		SkyBox.m_objMatrix.m_mxConstMatrix.m[3][2] = inverseCopy.r[3].m128_f32[2];



	}
#pragma endregion

	D3D11_MAPPED_SUBRESOURCE SceneResource;
	RedrawSceneBuffer(SceneResource, SceneMatrices);

	D3D11_MAPPED_SUBRESOURCE resource;
	XMMATRIX matrix;
	BufferInput input;

	// remapping the scene matrix

	UpdateSkyBox(rotation);

#pragma region Ground
	matrix = XMLoadFloat4x4(&Ground.m_objMatrix.m_mxConstMatrix);


	XMStoreFloat4x4(&Ground.m_objMatrix.m_mxConstMatrix, matrix);

	MappingObjMatrix(resource, Ground.m_objMatrix);

	//////////////////////////////////////////////////////////////////////

	MappingAnimation(resource, Ground);
#pragma endregion

	input.ConstPixel = nullptr;
	input.numPixelSlot = 0;

	input.ConstVertex = m_pConstBuffer;
	input.numVertexSlot = 2;

	DrawObj(&Ground, &input, m_shaderVS, m_MultiTexturePS, nullptr, 0);

	DrawStar(&m_StarModel);

	DrawStar(&m_StarModel2);

	XMFLOAT3 pos = m_StarModel.GetModelPosition();
	XMFLOAT3 pos2 = m_StarModel2.GetModelPosition();

#pragma region first light source
	//DirectionLight
	XMFLOAT3 normPos = pos;

	XMVECTOR normal = XMLoadFloat3(&normPos);
	normal /= 2;

	XMStoreFloat3(&normPos, normal);

	DirectionLight.dir = XMFLOAT3(-normPos.x, -normPos.y, 0);

	//POint light
	PointLight.pos = XMFLOAT4(pos.x, pos.y, pos.z, 1);
	PointLight.power = 14;
	PointLight.range = 100;
	PointLight.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	ZERO_OUT(PointLight.padding);

	//Spot Light Set up
	SpotLight.pos = XMFLOAT4(pos.x, pos.y, pos.z, 1);
	SpotLight.power = 64;
	SpotLight.coneDir = XMFLOAT3(0, -1, 0);
	SpotLight.coneWidth = cos(XMConvertToRadians(30.0f));
	SpotLight.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	XMMATRIX temp = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_mxViewMatrix));

	XMStoreFloat3(&SpotLight.CamPos, temp.r[3]);

#pragma endregion

#pragma region SecondLightSource
	//directional lighting

	normPos = pos2;

	normal = XMLoadFloat3(&normPos);

	normal = XMVector3Normalize(normal);

	XMStoreFloat3(&normPos, normal);

	DirectionLight2.dir = XMFLOAT3(-normPos.x, -normPos.y, 0);

	//Point Lighting
	PointLight2.pos = XMFLOAT4(pos2.x, pos2.y, pos2.z, 1);
	PointLight2.power = 14;
	PointLight2.range = 100;
	PointLight2.color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	//SpotLight
	SpotLight2.pos = XMFLOAT4(pos2.x, pos2.y, pos2.z, 1);
	SpotLight2.power = 64;
	SpotLight2.coneDir = XMFLOAT3(0, -1, 0);
	SpotLight2.coneWidth = cos(XMConvertToRadians(30.0f));
	SpotLight2.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	XMStoreFloat3(&SpotLight2.CamPos, temp.r[3]);
#pragma endregion

	MappingLighting(resource, DirectionLight, DirectionLight2);
	MappingPointLight(resource, PointLight, PointLight2);
	MappingSpotLight(resource, SpotLight, SpotLight2);

#pragma region cube constantBuffer
	matrix = XMLoadFloat4x4(&m_mdCube.m_objMatrix.m_mxConstMatrix);


	XMStoreFloat4x4(&m_mdCube.m_objMatrix.m_mxConstMatrix, matrix);

	m_objMatrix.m_mxConstMatrix = m_mdCube.m_objMatrix.m_mxConstMatrix;

	MappingObjMatrix(resource, m_mdCube.m_objMatrix);

	//////////////////////////////////////////////////////////////////////

#pragma endregion

	DrawObj(&m_mdCube, &ModelPixelInput, m_shaderVS, m_shaderPS, nullptr, 0);

#pragma region Dorumon constantBuffer
	matrix = XMLoadFloat4x4(&Dorumon.m_objMatrix.m_mxConstMatrix);


	XMStoreFloat4x4(&Dorumon.m_objMatrix.m_mxConstMatrix, matrix);

	m_objMatrix.m_mxConstMatrix = Dorumon.m_objMatrix.m_mxConstMatrix;

	MappingObjMatrix(resource, Dorumon.m_objMatrix);

	//////////////////////////////////////////////////////////////////////

#pragma endregion

	DrawInstanceObj(&Dorumon, &ModelPixelInput, m_vsInstancing, m_PScurrentShader, nullptr, 0);

#pragma region DinoTiger constantBuffer

	matrix = XMLoadFloat4x4(&DinoTiger.m_objMatrix.m_mxConstMatrix);



	XMStoreFloat4x4(&DinoTiger.m_objMatrix.m_mxConstMatrix, matrix);


	MappingObjMatrix(resource, DinoTiger.m_objMatrix);

	//////////////////////////////////////////////////////////////////////


#pragma endregion

	DrawObj(&DinoTiger, &ModelPixelInput, m_LightVS, m_PScurrentShader, nullptr, 0);

#pragma region DoruGreymon constantBuffer

	matrix = XMLoadFloat4x4(&DoruGreymon.m_objMatrix.m_mxConstMatrix);



	XMStoreFloat4x4(&DoruGreymon.m_objMatrix.m_mxConstMatrix, matrix);


	MappingObjMatrix(resource, DoruGreymon.m_objMatrix);

	//////////////////////////////////////////////////////////////////////
#pragma endregion

	DrawObj(&DoruGreymon, &ModelPixelInput, m_LightVS, m_PScurrentShader, nullptr, 0);

#pragma region Dorugoramon constantBuffer

	matrix = XMLoadFloat4x4(&Dorugoramon.m_objMatrix.m_mxConstMatrix);

	XMStoreFloat4x4(&Dorugoramon.m_objMatrix.m_mxConstMatrix, matrix);

	MappingObjMatrix(resource, Dorugoramon.m_objMatrix);

	//////////////////////////////////////////////////////////////////////

#pragma endregion

	DrawObj(&Dorugoramon, &ModelPixelInput, m_vsNormalMap, m_PScurrentNormalShader, nullptr, 0);

#pragma region Tree

	matrix = XMLoadFloat4x4(&Tree.m_objMatrix.m_mxConstMatrix);

	XMStoreFloat4x4(&Tree.m_objMatrix.m_mxConstMatrix, matrix);

	MappingObjMatrix(resource, Tree.m_objMatrix);
#pragma endregion

	m_dcConext->OMSetBlendState(m_pAlphaCoverage, blendFactor, sampleMask);

	input.ConstPixel = &m_pConstBufferAnimation_PS;
	input.numPixelSlot = 1;
	DrawObj(&Tree, &input, m_shaderVS, m_shaderPS, &MSAA, 1);

	m_dcConext->OMSetBlendState(m_pBlendState, blendFactor, sampleMask);

	ID3D11RasterizerState* rasterArray[2];
	rasterArray[0] = m_pRasterStateFrontCull;
	rasterArray[1] = m_pRasterState;

#pragma region Pyramid constantBuffer

	matrix = XMLoadFloat4x4(&Pyramid.m_objMatrix.m_mxConstMatrix);

	matrix = XMMatrixRotationX((float)dt.Delta()) * matrix;

	XMStoreFloat4x4(&Pyramid.m_objMatrix.m_mxConstMatrix, matrix);

	m_objMatrix.m_mxConstMatrix = Pyramid.m_objMatrix.m_mxConstMatrix;

	MappingObjMatrix(resource, m_objMatrix);

	////////////////////////////////////////////////////////////////////


	MappingAnimation(resource, Pyramid);
#pragma endregion

	input.ConstVertex = m_pConstBuffer;
	input.numVertexSlot = 2;

	DrawObj(&Pyramid, &input, m_shaderVS, m_shaderPS, rasterArray, 2);

	TransparentStarsDraw();

#pragma region Second Draw

	if (displayWindow)
	{
		//RedrawSceneBuffer(SceneResource, m_miniScene);

		m_dcConext->RSSetViewports(1, &m_vpSecondaryView);

		BufferInput Mini_input;
		ZERO_OUT(Mini_input);

		Mini_input.ConstVertex = m_pConstBuffer;
		Mini_input.numVertexSlot = 2;

		DrawObj(&m_StarModel, &Mini_input, m_shaderVS, m_DefaultPS, nullptr, 0);

		Mini_input.ConstPixel = &m_pConstBufferAnimation_PS;
		Mini_input.numPixelSlot = 1;

		Mini_input.ConstVertex = m_pConstBuffer;
		Mini_input.numVertexSlot = 2;

		DrawObj(&Pyramid, &Mini_input, m_shaderVS, m_shaderPS, rasterArray, 2);

		//DrawObj(&Dorumon, &ModelPixelInput, m_LightVS, m_PScurrentShader, nullptr, 0);
	}
#pragma endregion

	//Swaping the back buffer info with the front buffer
	m_snSwapChain->Present(1, 0);
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
	XMVECTOR pos = XMLoadFloat3(&XMFLOAT3(0, 2, -4));
	XMVECTOR focus = XMLoadFloat3(&XMFLOAT3(0, 0, 0));
	XMVECTOR height = XMLoadFloat3(&XMFLOAT3(0, 1, 0));

	//Creating the Camera
	view = XMMatrixLookAtLH(pos, focus, height);

	XMStoreFloat4x4(&m_mxViewMatrix, view);
	SceneMatrices.matrix_sceneCamera = m_mxViewMatrix;

	//miniViewPort Matrix

	//for the Look At Function
	pos = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
	focus = XMLoadFloat3(&XMFLOAT3(0, 0, 0));
	height = XMLoadFloat3(&XMFLOAT3(0, 1, 0));

	//Creating the Camera
	view = XMMatrixIdentity();
	view = XMMatrixLookAtLH(pos, focus, height);

	XMStoreFloat4x4(&m_mxMiniViewMatrix, view);


	/*******************************************************************/
	//Setting the Projection Matrix

	RECT aspect;
	GetClientRect(window, &aspect);

	float ratio = (float)aspect.right / (float)aspect.bottom;

	//Scene Projection Matrix
	XMMATRIX projection = XMMatrixIdentity();
	projection = XMMatrixPerspectiveFovLH(FOV, ratio, ZNEAR, ZFAR);
	XMStoreFloat4x4(&m_mxProjectonMatrix, projection);
	SceneMatrices.matrix_Projection = m_mxProjectonMatrix;

	//ViewPort Projection
	projection = XMMatrixIdentity();
	projection = XMMatrixPerspectiveFovLH(FOV, ratio, ZNEAR, ZFAR);
	XMStoreFloat4x4(&m_mxMiniProjectionMatrix, projection);


	m_miniScene.matrix_Projection = m_mxMiniProjectionMatrix;
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
		CameraRotate = XMMatrixRotationX(XMConvertToRadians((float)distance.y)* dt)*CameraRotate;
	}

	XMStoreFloat4x4(rot, CameraRotate);

	move = true;
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
	Star[1].col = XMFLOAT4(1, 0, 0, 0.5f);

	//red
	Star[2].pos.x = 0.3f;
	Star[2].pos.y = 0.3f;
	Star[2].pos.z = 0;
	Star[2].pos.w = 1;
	Star[2].col = XMFLOAT4(1, 0, 0, 0.5f);

	//blue
	Star[3].pos.x = 0.7f;
	Star[3].pos.y = 0.3f;
	Star[3].pos.z = 0;
	Star[3].pos.w = 1;
	Star[3].col = XMFLOAT4(0, 0, 1, 0.5f);

	//blue
	Star[4].pos.x = 0.4f;
	Star[4].pos.y = -0.2f;
	Star[4].pos.z = 0;
	Star[4].pos.w = 1;
	Star[4].col = XMFLOAT4(0, 0, 1, 0.5f);

	//green
	Star[5].pos.x = 0.4f;
	Star[5].pos.y = -0.7f;
	Star[5].pos.z = 0;
	Star[5].pos.w = 1;
	Star[5].col = XMFLOAT4(0, 0, 1, 0.5f);

	//green
	Star[6].pos.x = 0;
	Star[6].pos.y = -0.4f;
	Star[6].pos.z = 0;
	Star[6].pos.w = 1;
	Star[6].col = XMFLOAT4(0, 0, 1, 0.5f);

	//yellow
	Star[7].pos.x = -0.4f;
	Star[7].pos.y = -0.7f;
	Star[7].pos.z = 0;
	Star[7].pos.w = 1;
	Star[7].col = XMFLOAT4(1, 1, 0, 0.5f);

	//yellow
	Star[8].pos.x = -0.4f;
	Star[8].pos.y = -0.2f;
	Star[8].pos.z = 0;
	Star[8].pos.w = 1;
	Star[8].col = XMFLOAT4(1, 1, 0, 0.5f);

	//cyan
	Star[9].pos.x = -0.7f;
	Star[9].pos.y = 0.3f;
	Star[9].pos.z = 0;
	Star[9].pos.w = 1;
	Star[9].col = XMFLOAT4(0, 1, 1, 0.5f);

	//cyan
	Star[10].pos.x = -0.3f;
	Star[10].pos.y = 0.3f;
	Star[10].pos.z = 0;
	Star[10].pos.w = 1;
	Star[10].col = XMFLOAT4(0, 1, 1, 0.5f);

	//white
	Star[11].pos.x = 0;
	Star[11].pos.y = 0;
	Star[11].pos.z = 0.5f;
	Star[11].pos.w = 1;
	Star[11].col = XMFLOAT4(1, 1, 1, 0.5f);

	//red
	Star[12].pos.x = 0;
	Star[12].pos.y = 1;
	Star[12].pos.z = 0.5f;
	Star[12].pos.w = 1;
	Star[12].col = XMFLOAT4(1, 0, 0, 0.5f);

	//cyan
	Star[13].pos.x = -0.3f;
	Star[13].pos.y = 0.3f;
	Star[13].pos.z = 0.5f;
	Star[13].pos.w = 1;
	Star[13].col = XMFLOAT4(0, 1, 1, 0.5f);

	//cyan
	Star[14].pos.x = -0.7f;
	Star[14].pos.y = 0.3f;
	Star[14].pos.z = 0.5f;
	Star[14].pos.w = 1;
	Star[14].col = XMFLOAT4(0, 1, 1, 0.5f);

	//yelloe
	Star[15].pos.x = -0.4f;
	Star[15].pos.y = -0.2f;
	Star[15].pos.z = 0.5f;
	Star[15].pos.w = 1;
	Star[15].col = XMFLOAT4(1, 1, 0, 0.5f);

	//yellow
	Star[16].pos.x = -0.4f;
	Star[16].pos.y = -0.7f;
	Star[16].pos.z = 0.5f;
	Star[16].pos.w = 1;
	Star[16].col = XMFLOAT4(1, 1, 0, 0.5f);

	//green
	Star[17].pos.x = 0;
	Star[17].pos.y = -0.4f;
	Star[17].pos.z = 0.5f;
	Star[17].pos.w = 1;
	Star[17].col = XMFLOAT4(0, 1, 0, 0.5f);

	//green
	Star[18].pos.x = 0.4f;
	Star[18].pos.y = -0.7f;
	Star[18].pos.z = 0.5f;
	Star[18].pos.w = 1;
	Star[18].col = XMFLOAT4(0, 1, 0, 0.5f);


	//wblue
	Star[19].pos.x = 0.4f;
	Star[19].pos.y = -0.2f;
	Star[19].pos.z = 0.5f;
	Star[19].pos.w = 1;
	Star[19].col = XMFLOAT4(0, 0, 1, 0.5f);

	//blue
	Star[20].pos.x = 0.7f;
	Star[20].pos.y = 0.3f;
	Star[20].pos.z = 0.5f;
	Star[20].pos.w = 1;
	Star[20].col = XMFLOAT4(0, 0, 1, 0.5f);

	//red

	Star[21].pos.x = 0.3f;
	Star[21].pos.y = 0.3f;
	Star[21].pos.z = 0.5f;
	Star[21].pos.w = 1;
	Star[21].col = XMFLOAT4(0, 1, 0, 0.5f);



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
	m_StarModel.CreateBuffers(m_iDevice, maxIndices, indices);

	XMMATRIX temp = XMLoadFloat4x4(&m_mxWorldMatrix) * XMMatrixTranslation(0, 3.5f, -1);

	XMStoreFloat4x4(&m_StarModel.m_objMatrix.m_mxConstMatrix, temp);

}

void DEMO_APP::CreateStar(Model* star, unsigned int numStars, D3D11_SAMPLER_DESC * p_sampler)
{
	for (unsigned int i = 0; i < numStars; i++)
	{
		INPUT_VERTEX* Star = new INPUT_VERTEX[22];
#pragma region //Star
		//white
		Star[0].pos.x = 0;
		Star[0].pos.y = 0;
		Star[0].pos.z = 0;
		Star[0].pos.w = 1;
		Star[0].col = XMFLOAT4(1, 1, 1, 0.3f);
		//red
		Star[1].pos.x = 0;
		Star[1].pos.y = 1;
		Star[1].pos.z = 0;
		Star[1].pos.w = 1;
		Star[1].col = XMFLOAT4(1, 0, 0, 0.3f);

		//red
		Star[2].pos.x = 0.3f;
		Star[2].pos.y = 0.3f;
		Star[2].pos.z = 0;
		Star[2].pos.w = 1;
		Star[2].col = XMFLOAT4(1, 0, 0, 0.3f);

		//blue
		Star[3].pos.x = 0.7f;
		Star[3].pos.y = 0.3f;
		Star[3].pos.z = 0;
		Star[3].pos.w = 1;
		Star[3].col = XMFLOAT4(0, 0, 1, 0.3f);

		//blue
		Star[4].pos.x = 0.4f;
		Star[4].pos.y = -0.2f;
		Star[4].pos.z = 0;
		Star[4].pos.w = 1;
		Star[4].col = XMFLOAT4(0, 0, 1, 0.3f);

		//green
		Star[5].pos.x = 0.4f;
		Star[5].pos.y = -0.7f;
		Star[5].pos.z = 0;
		Star[5].pos.w = 1;
		Star[5].col = XMFLOAT4(0, 0, 1, 0.3f);

		//green
		Star[6].pos.x = 0;
		Star[6].pos.y = -0.4f;
		Star[6].pos.z = 0;
		Star[6].pos.w = 1;
		Star[6].col = XMFLOAT4(0, 0, 1, 0.3f);

		//yellow
		Star[7].pos.x = -0.4f;
		Star[7].pos.y = -0.7f;
		Star[7].pos.z = 0;
		Star[7].pos.w = 1;
		Star[7].col = XMFLOAT4(1, 1, 0, 0.3f);

		//yellow
		Star[8].pos.x = -0.4f;
		Star[8].pos.y = -0.2f;
		Star[8].pos.z = 0;
		Star[8].pos.w = 1;
		Star[8].col = XMFLOAT4(1, 1, 0, 0.3f);

		//cyan
		Star[9].pos.x = -0.7f;
		Star[9].pos.y = 0.3f;
		Star[9].pos.z = 0;
		Star[9].pos.w = 1;
		Star[9].col = XMFLOAT4(0, 1, 1, 0.3f);

		//cyan
		Star[10].pos.x = -0.3f;
		Star[10].pos.y = 0.3f;
		Star[10].pos.z = 0;
		Star[10].pos.w = 1;
		Star[10].col = XMFLOAT4(0, 1, 1, 0.3f);

		//white
		Star[11].pos.x = 0;
		Star[11].pos.y = 0;
		Star[11].pos.z = 0.5f;
		Star[11].pos.w = 1;
		Star[11].col = XMFLOAT4(1, 1, 1, 0.5f);

		//red
		Star[12].pos.x = 0;
		Star[12].pos.y = 1;
		Star[12].pos.z = 0.5f;
		Star[12].pos.w = 1;
		Star[12].col = XMFLOAT4(1, 0, 0, 0.5f);

		//cyan
		Star[13].pos.x = -0.3f;
		Star[13].pos.y = 0.3f;
		Star[13].pos.z = 0.5f;
		Star[13].pos.w = 1;
		Star[13].col = XMFLOAT4(0, 1, 1, 0.5f);

		//cyan
		Star[14].pos.x = -0.7f;
		Star[14].pos.y = 0.3f;
		Star[14].pos.z = 0.5f;
		Star[14].pos.w = 1;
		Star[14].col = XMFLOAT4(0, 1, 1, 0.5f);

		//yelloe
		Star[15].pos.x = -0.4f;
		Star[15].pos.y = -0.2f;
		Star[15].pos.z = 0.5f;
		Star[15].pos.w = 1;
		Star[15].col = XMFLOAT4(1, 1, 0, 0.5f);

		//yellow
		Star[16].pos.x = -0.4f;
		Star[16].pos.y = -0.7f;
		Star[16].pos.z = 0.5f;
		Star[16].pos.w = 1;
		Star[16].col = XMFLOAT4(1, 1, 0, 0.5f);

		//green
		Star[17].pos.x = 0;
		Star[17].pos.y = -0.4f;
		Star[17].pos.z = 0.5f;
		Star[17].pos.w = 1;
		Star[17].col = XMFLOAT4(0, 1, 0, 0.5f);

		//green
		Star[18].pos.x = 0.4f;
		Star[18].pos.y = -0.7f;
		Star[18].pos.z = 0.5f;
		Star[18].pos.w = 1;
		Star[18].col = XMFLOAT4(0, 1, 0, 0.5f);


		//wblue
		Star[19].pos.x = 0.4f;
		Star[19].pos.y = -0.2f;
		Star[19].pos.z = 0.5f;
		Star[19].pos.w = 1;
		Star[19].col = XMFLOAT4(0, 0, 1, 0.5f);

		//blue
		Star[20].pos.x = 0.7f;
		Star[20].pos.y = 0.3f;
		Star[20].pos.z = 0.5f;
		Star[20].pos.w = 1;
		Star[20].col = XMFLOAT4(0, 0, 1, 0.5f);

		//red

		Star[21].pos.x = 0.3f;
		Star[21].pos.y = 0.3f;
		Star[21].pos.z = 0.5f;
		Star[21].pos.w = 1;
		Star[21].col = XMFLOAT4(0, 1, 0, 0.5f);



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
		star[i].loadVerts(22, Star);

		star[i].CreateBuffers(m_iDevice, maxIndices, indices);

		XMMATRIX temp = XMLoadFloat4x4(&m_mxWorldMatrix) * XMMatrixTranslation(0, 3, -1);

		XMStoreFloat4x4(&star[i].m_objMatrix.m_mxConstMatrix, temp);
	}

}

void DEMO_APP::MultiThreadCreateObj(ThreadStruct* p_thread)
{
	INPUT_VERTEX* ObjVerts = nullptr;
	unsigned int * ObjectIndices = nullptr;
	unsigned int max = 0;
	unsigned int maxIndex = 0;

	MultithreadLoader(p_thread->m_lock, p_thread->m_pThreadLock, p_thread->m_pCondition, p_thread->file.c_str(), &ObjVerts, &ObjectIndices, max, maxIndex);

	p_thread->p_model->loadVerts(max, ObjVerts);

	p_thread->p_model->CreateTexture(m_iDevice, p_thread->filename.c_str(), p_thread->Secondfilename.c_str(), p_thread->p_sampler);

	p_thread->p_model->CreateBuffers(m_iDevice, maxIndex, ObjectIndices, p_thread->numInstance, p_thread->instances);

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

	p_model.CreateBuffers(m_iDevice, maxIndex, ObjectIndices);

}

void DEMO_APP::CreateObj(const char* file, Model& p_model, const wchar_t* filename, D3D11_SAMPLER_DESC* p_sampler, const wchar_t* Secondfilename, unsigned int numInstance, InstanceType* instances)
{
	INPUT_VERTEX* ObjVerts = nullptr;
	unsigned int * ObjectIndices = nullptr;
	unsigned int max = 0;
	unsigned int maxIndex = 0;

	ObjectLoader(file, &ObjVerts, &ObjectIndices, max, maxIndex);

	p_model.loadVerts(max, ObjVerts);

	p_model.CreateTexture(m_iDevice, filename, Secondfilename, p_sampler);

	p_model.CreateBuffers(m_iDevice, maxIndex, ObjectIndices, numInstance, instances);
}

void DEMO_APP::CreateObjCube(const char* file, Model& p_model, const wchar_t* filename, D3D11_SAMPLER_DESC* p_sampler, const wchar_t* Secondfilename)
{
	INPUT_VERTEX* ObjVerts = nullptr;
	unsigned int * ObjectIndices = nullptr;
	unsigned int max = 0;
	unsigned int maxIndex = 0;

	CubeObjectLoader(file, &ObjVerts, &ObjectIndices, max, maxIndex);

	p_model.loadVerts(max, ObjVerts);

	p_model.CreateTexture(m_iDevice, filename, Secondfilename, p_sampler);

	p_model.CreateBuffers(m_iDevice, maxIndex, ObjectIndices);
}

void DEMO_APP::DrawInstanceObj(Model* p_model, BufferInput* p_input, ID3D11VertexShader* p_shaderVS, ID3D11PixelShader* p_shaderPS, ID3D11RasterizerState** raster, unsigned int size)
{
	m_dcConext->PSSetShader(p_shaderPS, NULL, 0);
	m_dcConext->VSSetShader(p_shaderVS, NULL, 0);

	m_dcConext->VSSetConstantBuffers(0, p_input->numVertexSlot, p_input->ConstVertex);
	m_dcConext->PSSetConstantBuffers(0, p_input->numPixelSlot, p_input->ConstPixel);

	p_model->Draw(m_dcConext, m_pInstanceInput, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, raster, size);
}

void DEMO_APP::DrawObj(Model* p_model, BufferInput* p_input, ID3D11VertexShader* p_shaderVS, ID3D11PixelShader* p_shaderPS, ID3D11RasterizerState** raster, unsigned int size)
{
	m_dcConext->PSSetShader(p_shaderPS, NULL, 0);
	m_dcConext->VSSetShader(p_shaderVS, NULL, 0);

	m_dcConext->VSSetConstantBuffers(0, p_input->numVertexSlot, p_input->ConstVertex);
	m_dcConext->PSSetConstantBuffers(0, p_input->numPixelSlot, p_input->ConstPixel);

	p_model->Draw(m_dcConext, m_pVertexInput, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, raster, size);

}

void DEMO_APP::DrawStar(Model* p_model)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	//D3D11_MAPPED_SUBRESOURCE SceneResource;
	XMMATRIX matrix;

#pragma region Star constantBuffer


	matrix = XMLoadFloat4x4(&p_model->m_objMatrix.m_mxConstMatrix);

	matrix = XMMatrixRotationY((float)dt.Delta()) * matrix;

	BackAndForth(matrix.r[3].m128_f32[0], ptTransX);

	matrix *= XMMatrixTranslation(ptTransX, 0, 0);

	XMStoreFloat4x4(&p_model->m_objMatrix.m_mxConstMatrix, matrix);

	MappingObjMatrix(resource, p_model->m_objMatrix);


	//////////////////////////////////////////////////////////////////////
#pragma endregion

	BufferInput input;
	ZERO_OUT(input);
	input.ConstVertex = m_pConstBuffer;
	input.numVertexSlot = 2;


	DrawObj(&m_StarModel, &input, m_shaderVS, m_DefaultPS, nullptr, 0);

	//	m_dcConext->PSSetShader(m_DefaultPS, NULL, 0);
	//m_dcConext->VSSetShader(m_shaderVS, NULL, 0);

	//m_StarModel.Draw(m_dcConext, m_pVertexInput, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, nullptr, 0, (float)dt.Delta());

}

void DEMO_APP::DrawStars(Model* p_star)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	//D3D11_MAPPED_SUBRESOURCE SceneResource;
	XMMATRIX matrix;

#pragma region Star constantBuffer


	matrix = XMLoadFloat4x4(&p_star->m_objMatrix.m_mxConstMatrix);

	//matrix = XMMatrixRotationY((float)dt.Delta()) * matrix;

	//ptTransX += BackAndForth(matrix.r[3].m128_f32[0]);

	//matrix *= XMMatrixTranslation(ptTransX, 0, 0);

	XMStoreFloat4x4(&p_star->m_objMatrix.m_mxConstMatrix, matrix);

	MappingObjMatrix(resource, p_star->m_objMatrix);


	//////////////////////////////////////////////////////////////////////

#pragma endregion

	BufferInput input;
	ZERO_OUT(input);
	input.ConstVertex = m_pConstBuffer;
	input.numVertexSlot = 2;
	DrawObj(p_star, &input, m_shaderVS, m_DefaultPS, nullptr, 0);
}

void DEMO_APP::TransparentStarsDraw()
{
	XMMATRIX inverseCopy = XMLoadFloat4x4(&m_mxViewMatrix);

	//inverseCopy = XMMatrixInverse(nullptr, inverseCopy);

	XMVECTOR camPos = inverseCopy.r[3];

	XMVECTOR Starpos[3];

	///
	XMVECTOR CameraZaxis = inverseCopy.r[2];

	//getting the models position
	Starpos[0] = camPos - XMLoadFloat3(&m_multiStarModel[0].GetModelPosition());
	Starpos[1] = camPos - XMLoadFloat3(&m_multiStarModel[1].GetModelPosition());
	Starpos[2] = camPos - XMLoadFloat3(&m_multiStarModel[2].GetModelPosition());

	XMFLOAT3 result[3];
	vector<float> resultList;

	XMStoreFloat3(&result[0], XMVector3Dot(Starpos[0], CameraZaxis));
	resultList.push_back(result[0].x);

	XMStoreFloat3(&result[1], XMVector3Dot(Starpos[1], CameraZaxis));
	resultList.push_back(result[1].x);

	XMStoreFloat3(&result[2], XMVector3Dot(Starpos[2], CameraZaxis));
	resultList.push_back(result[2].x);

	unsigned int oldIndex = 0;

	vector<unsigned int> store;

#pragma region Sorting Needs work

	std::sort(resultList.begin(), resultList.end());

	for (int arrayIndex = 0; arrayIndex < 3; arrayIndex++)
	{
		for (unsigned int i = 0; i < resultList.size(); i++)
		{
			if (resultList[arrayIndex] == result[i].z)
			{
				store.push_back(i);
				break;
			}
		}
	}

#pragma endregion

	for (int i = 0; i < 3; i++)
	{
		DrawStars(&m_multiStarModel[store[i]]);
	}
}

void DEMO_APP::MappingObjMatrix(D3D11_MAPPED_SUBRESOURCE& p_Scene, Object& p_Matrix)
{
	////////////////////////////////////////////////////////////////////

	ZeroMemory(&p_Scene, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_dcConext->Map(m_pConstBuffer[OBJECT], 0, D3D11_MAP_WRITE_DISCARD, NULL, &p_Scene);

	m_objMatrix = p_Matrix;

	memcpy(p_Scene.pData, &m_objMatrix, sizeof(Object));

	m_dcConext->Unmap(m_pConstBuffer[OBJECT], 0);
}

void DEMO_APP::MappingAnimation(D3D11_MAPPED_SUBRESOURCE& p_Scene, Model& p_Model)
{
#pragma region Animation

	ZeroMemory(&p_Scene, sizeof(p_Scene));

	m_dcConext->Map(m_pConstBufferAnimation_PS, 0, D3D11_MAP_WRITE_DISCARD, NULL, &p_Scene);

#pragma region FrameUpdate

	p_Model.delta += (float)dt.Delta();

	if (p_Model.delta >= 1)
	{
		p_Model.m_aniAnimaiton.frame++;

		if (p_Model.m_aniAnimaiton.maxFrame <= p_Model.m_aniAnimaiton.frame)
		{
			p_Model.m_aniAnimaiton.frame = 0;
		}

		p_Model.delta = 0;
	}

#pragma endregion

	memcpy(p_Scene.pData, &p_Model.m_aniAnimaiton, sizeof(ANIMATION));

	m_dcConext->Unmap(m_pConstBufferAnimation_PS, 0);

#pragma endregion
}

void DEMO_APP::MappingLighting(D3D11_MAPPED_SUBRESOURCE& p_Scene, LIGHTING& p_light, LIGHTING& p_light2)
{
	ZeroMemory(&p_Scene, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_dcConext->Map(m_pConstBufferLight_PS, 0, D3D11_MAP_WRITE_DISCARD, NULL, &p_Scene);

	m_lgLight[FIRSTLIGHT] = p_light;
	m_lgLight[SECONDLIGHT] = p_light2;

	memcpy(p_Scene.pData, &m_lgLight, sizeof(LIGHTING) * 2);

	m_dcConext->Unmap(m_pConstBufferLight_PS, 0);
}

void DEMO_APP::MappingPointLight(D3D11_MAPPED_SUBRESOURCE& p_Scene, PtLight& p_light, PtLight& p_light2)
{
	ZeroMemory(&p_Scene, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_dcConext->Map(m_pCBufferPointLight_PS, 0, D3D11_MAP_WRITE_DISCARD, NULL, &p_Scene);

	m_plgLight[FIRSTLIGHT] = p_light;
	m_plgLight[SECONDLIGHT] = p_light2;

	memcpy(p_Scene.pData, &m_plgLight, sizeof(PtLight) * 2);
	m_dcConext->Unmap(m_pCBufferPointLight_PS, 0);
}

void DEMO_APP::MappingSpotLight(D3D11_MAPPED_SUBRESOURCE& p_Scene, SptLight& p_light, SptLight& p_light2)
{
	ZeroMemory(&p_Scene, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_dcConext->Map(m_pCBufferSpotLight_PS, 0, D3D11_MAP_WRITE_DISCARD, NULL, &p_Scene);

	m_sptLight[FIRSTLIGHT] = p_light;
	m_sptLight[SECONDLIGHT] = p_light2;

	memcpy(p_Scene.pData, &m_sptLight, sizeof(SptLight) * 2);
	m_dcConext->Unmap(m_pCBufferSpotLight_PS, 0);
}

void DEMO_APP::RedrawSceneBuffer(D3D11_MAPPED_SUBRESOURCE& p_Scene, Scene& p_Matrix)
{
	////////////////////////////////////////////////////////////////////

	ZeroMemory(&p_Scene, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_dcConext->Map(m_pConstBuffer[SCENE], 0, D3D11_MAP_WRITE_DISCARD, NULL, &p_Scene);

	m_scnMatrix = p_Matrix;

	memcpy(p_Scene.pData, &m_scnMatrix, sizeof(Scene));

	m_dcConext->Unmap(m_pConstBuffer[SCENE], 0);
}

void DEMO_APP::BackAndForth(float _comp, float& p_trans)
{
	bool lastFrame = flip;

	if (_comp <= -2)
	{
		flip = true;
	}
	else if (_comp >= 2)
	{
		flip = false;
	}

	if (lastFrame != flip)
	{
		p_trans = 0.000f;
	}
	else if (flip)
	{

		p_trans += (float)dt.Delta() / 50;
	}
	else
	{
		p_trans += (float)-dt.Delta() / 50;
	}
}

void DEMO_APP::UpdateSkyBox(XMFLOAT4X4& rot)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	//	D3D11_MAPPED_SUBRESOURCE SceneResource;

	XMMATRIX matrix = XMLoadFloat4x4(&SkyBox.m_objMatrix.m_mxConstMatrix);

#pragma region SkyBox ConstantBuffer

	//inversing the camera to set the position of the cube to the camera

	//camera = XMMatrixInverse(nullptr, camera);

	XMVECTOR SkyPos = matrix.r[3];
	matrix = XMMatrixInverse(nullptr, matrix);
	matrix.r[3] = XMVectorSet(0, 0, 0, 1);

	if (fabsf((float)distance.x) > fabsf((float)distance.y))
	{
		matrix = matrix*XMMatrixRotationY(XMConvertToRadians((float)distance.x) *0.5f);
	}
	else if (fabsf((float)distance.x) < fabsf((float)distance.y))
	{
		matrix = XMMatrixRotationX(XMConvertToRadians((float)distance.y)* 0.5f)*matrix;
	}
	matrix = XMMatrixInverse(nullptr, matrix);

	matrix.r[3] = SkyPos;

	XMStoreFloat4x4(&SkyBox.m_objMatrix.m_mxConstMatrix, matrix);

	MappingObjMatrix(resource, SkyBox.m_objMatrix);

	//////////////////////////////////////////////////////////////////////

#pragma endregion

	BufferInput input;
	input.numPixelSlot = 0;
	input.ConstPixel = nullptr;
	input.numVertexSlot = 2;
	input.ConstVertex = m_pConstBuffer;

	DrawObj(&SkyBox, &input, m_SkyboxVS, m_SkyboxPS, &m_pSkyBoxRasterState, 1);

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

	temp = XMMatrixInverse(nullptr, temp);

	XMStoreFloat4x4(&SkyBox.m_objMatrix.m_mxConstMatrix, temp);

	//SkyBox.ScaleModel(1.0f);
}

void DEMO_APP::CreateThreadStruct(ThreadStruct* p_struct, string p_file, wstring p_filename, Model*	p_pmodel, D3D11_SAMPLER_DESC* p_psampler, wstring	p_Secondfilename, unsigned int p_numInstance, InstanceType* p_instances)
{
	p_struct->file = p_file;
	p_struct->filename = p_filename;
	p_struct->p_model = p_pmodel;
	p_struct->p_sampler = p_psampler;
	p_struct->Secondfilename = p_Secondfilename;
	p_struct->numInstance = p_numInstance;
	p_struct->instances = p_instances;
}

void DEMO_APP::CreateConstBuffers()
{
	//Constant buffer
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.ByteWidth = sizeof(Object);
	cBufferDesc.MiscFlags = 0;

	//subData
	D3D11_SUBRESOURCE_DATA ConstantBufferData;
	ConstantBufferData.pSysMem = &m_objMatrix;
	ConstantBufferData.SysMemPitch = 0;
	ConstantBufferData.SysMemSlicePitch = 0;

	m_iDevice->CreateBuffer(&cBufferDesc, &ConstantBufferData, &m_pConstBuffer[OBJECT]);

	//Setting the scene buffer
	cBufferDesc.ByteWidth = sizeof(Scene);
	ConstantBufferData.pSysMem = &m_scnMatrix;

	m_iDevice->CreateBuffer(&cBufferDesc, &ConstantBufferData, &m_pConstBuffer[SCENE]);


	cBufferDesc.ByteWidth = sizeof(ANIMATION);
	ConstantBufferData.pSysMem = &m_aniAnimation;
	m_iDevice->CreateBuffer(&cBufferDesc, &ConstantBufferData, &m_pConstBufferAnimation_PS);


	cBufferDesc.ByteWidth = sizeof(LIGHTING) * 2;
	ConstantBufferData.pSysMem = m_lgLight;

	m_iDevice->CreateBuffer(&cBufferDesc, &ConstantBufferData, &m_pConstBufferLight_PS);

	cBufferDesc.ByteWidth = sizeof(PtLight) * 2;
	ConstantBufferData.pSysMem = m_plgLight;

	m_iDevice->CreateBuffer(&cBufferDesc, &ConstantBufferData, &m_pCBufferPointLight_PS);

	cBufferDesc.ByteWidth = sizeof(SptLight) * 2;
	ConstantBufferData.pSysMem = m_sptLight;

	m_iDevice->CreateBuffer(&cBufferDesc, &ConstantBufferData, &m_pCBufferSpotLight_PS);
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

void DEMO_APP::CreateRenderToTexture()
{
	//Render To Texture
	D3D11_TEXTURE2D_DESC RTTDesc;
	ZeroMemory(&RTTDesc, sizeof(RTTDesc));
	RTTDesc.Width = 400;
	RTTDesc.Height = 400;
	RTTDesc.MipLevels = 1;
	RTTDesc.ArraySize = 1;
	RTTDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	RTTDesc.SampleDesc.Count = 1;
	RTTDesc.Usage = D3D11_USAGE_DEFAULT;
	RTTDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	RTTDesc.CPUAccessFlags = 0;
	RTTDesc.MiscFlags = 0;

	//Creating the image
	m_iDevice->CreateTexture2D(&RTTDesc, NULL, &m_RttCubeRenderTarget);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;

	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	m_iDevice->CreateRenderTargetView(m_RttCubeRenderTarget, &rtvDesc, &m_RttrtvToCube);

	D3D11_SHADER_RESOURCE_VIEW_DESC RTTViewDesc;
	RTTViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	RTTViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	RTTViewDesc.Texture2D.MostDetailedMip = 0;
	RTTViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	m_iDevice->CreateShaderResourceView(m_RttCubeRenderTarget, &RTTViewDesc, &m_RTTShaderResource);

#pragma region Depth Creation

	//setting up the depthbuffer
	D3D11_TEXTURE2D_DESC DepthDesc;
	//zero-ing it out
	ZeroMemory(&DepthDesc, sizeof(DepthDesc));

	DepthDesc.Width = 400;
	DepthDesc.Height = 400;
	DepthDesc.MipLevels = 1;
	DepthDesc.ArraySize = 1;
	DepthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthDesc.SampleDesc.Count = 1;
	DepthDesc.SampleDesc.Quality = 0;
	DepthDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthDesc.CPUAccessFlags = 0;
	DepthDesc.MiscFlags = 0;

	m_iDevice->CreateTexture2D(&DepthDesc, NULL, &m_RttZBuffer);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	DefaultDepthDesc(&depthStencilDesc);

	m_iDevice->CreateDepthStencilState(&depthStencilDesc, &m_RttpDepthState);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDepthView;
	ZeroMemory(&descDepthView, sizeof(descDepthView));
	descDepthView.Format = DXGI_FORMAT_D32_FLOAT;
	descDepthView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDepthView.Texture2D.MipSlice = 0;

	m_iDevice->CreateDepthStencilView(m_RttZBuffer, &descDepthView, &m_RttDepthView);

#pragma endregion

	//m_vpCubeViewport;
	m_RttvpCubeViewport.TopLeftX = 0;
	m_RttvpCubeViewport.TopLeftY = 0;
	m_RttvpCubeViewport.Height = 400;
	m_RttvpCubeViewport.Width = 400;
	m_RttvpCubeViewport.MaxDepth = 1;
	m_RttvpCubeViewport.MinDepth = 0;

	//m_mxCubeViewMatrix;
	XMMATRIX view = XMMatrixIdentity();

	//for the Look At Function
	XMVECTOR pos = XMLoadFloat3(&XMFLOAT3(2, 2.5, 1));
	XMVECTOR focus = XMLoadFloat3(&XMFLOAT3(2, 0, -1));
	XMVECTOR height = XMLoadFloat3(&XMFLOAT3(0, 2, 0));

	//Creating the Camera
	view = XMMatrixLookAtLH(pos, focus, height);
	//Saving the camera
	XMStoreFloat4x4(&m_RttmxCubeViewMatrix, view);
	RenderTTexture.matrix_sceneCamera = m_RttmxCubeViewMatrix;

	//m_mxCubeProjectonMatrix;
	XMMATRIX projection = XMMatrixIdentity();
	projection = XMMatrixPerspectiveFovLH(FOV, 1, ZNEAR, ZFAR);
	XMStoreFloat4x4(&m_RttmxCubeProjectonMatrix, projection);
	RenderTTexture.matrix_Projection = m_RttmxCubeProjectonMatrix;
}

void DEMO_APP::ToggleFillMode(ID3D11RasterizerState** raster, D3D11_FILL_MODE fill)
{
	SAFE_RELEASE((*raster));

	D3D11_RASTERIZER_DESC rasterizerState;

	DefaultRasterizerDesc(&rasterizerState);

	rasterizerState.FillMode = fill;

	m_iDevice->CreateRasterizerState(&rasterizerState, raster);
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6
	SAFE_RELEASE(m_shaderPS);
	SAFE_RELEASE(m_MultiTexturePS);
	SAFE_RELEASE(m_vsInstancing);

	SAFE_RELEASE(m_PLightPS);
	SAFE_RELEASE(m_SpLightPS);

	SAFE_RELEASE(m_vsNormalMap);
	SAFE_RELEASE(m_psNormalMap);
	SAFE_RELEASE(m_psNormalMapPoint);
	SAFE_RELEASE(m_psNormalMapSpot);

	//SAFE_RELEASE(m_PScurrentShader);
	//SAFE_RELEASE(m_PScurrentNormalShader);

	SAFE_RELEASE(m_pCuberaster);
	SAFE_RELEASE(m_pInstanceInput);

	SAFE_RELEASE(m_pConstBufferAnimation_PS);
	SAFE_RELEASE(m_pConstBufferLight_PS);
	SAFE_RELEASE(m_pCBufferPointLight_PS);
	SAFE_RELEASE(m_pCBufferSpotLight_PS);

	SAFE_RELEASE(m_RttCubeRenderTarget);
	SAFE_RELEASE(m_RttrtvToCube);
	SAFE_RELEASE(m_shaderVS);
	SAFE_RELEASE(m_DefaultPS);
	SAFE_RELEASE(m_SkyboxPS);
	SAFE_RELEASE(m_SkyboxVS);

	SAFE_RELEASE(m_DLightPS);
	SAFE_RELEASE(m_LightVS);

	SAFE_RELEASE(m_pDepthState);
	SAFE_RELEASE(m_DepthView);
	SAFE_RELEASE(m_ZBuffer);

	SAFE_RELEASE(m_pSkyBoxRasterState);

	SAFE_RELEASE(m_pConstBuffer[0]);
	SAFE_RELEASE(m_pConstBuffer[1]);
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


	SAFE_RELEASE(m_pAlphaCoverage);
	SAFE_RELEASE(MSAA);
	SAFE_RELEASE(m_psNormalSpecSpot);

		
	SAFE_RELEASE(m_RTTShaderResource);
	SAFE_RELEASE(m_RttZBuffer);
	SAFE_RELEASE(m_RttDepthView);
	SAFE_RELEASE(m_RttpDepthState);

	//Releasing the Models
	m_StarModel2.Release();
	Dorugamon.Release();
	Tree.Release();
	m_StarModel.Release();
	Pyramid.Release();
	Dorumon.Release();
	SkyBox.Release();
	DinoTiger.Release();
	Ground.Release();
	DoruGreymon.Release();
	Dorugoramon.Release();

	m_multiStarModel[0].Release();
	m_multiStarModel[1].Release();
	m_multiStarModel[2].Release();

	m_mdCube.Release();


#if _DEBUG
	m_dgDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
#endif

	SAFE_RELEASE(m_dgDebug);
	SAFE_RELEASE(m_iDevice);


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
			SAFE_RELEASE(g_DepthView);
			SAFE_RELEASE(g_ZBuffer);
			SAFE_RELEASE(g_pBackBuffer);
			SAFE_RELEASE(g_pDepthState);
			SAFE_RELEASE(g_rtvRenderTargetView);

			g_dcConext->Flush();

			HRESULT hr;
			// Preserve the existing buffer count and format.
			// Automatically choose the width and height to match the client rect for HWNDs.
			hr = g_snSwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
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
			RECT window;
			GetClientRect(hWnd, &window);


			D3D11_RECT windowRect[1];
			windowRect[0].left = 0;
			windowRect[0].right = window.right;
			windowRect[0].top = 0;
			windowRect[0].bottom = window.bottom;

			g_dcConext->RSSetScissorRects(1, windowRect);
#pragma endregion

			g_dcConext->OMSetRenderTargets(1, &g_rtvRenderTargetView, g_DepthView);

			// Set up the viewport.
			m_vpViewPort.Width = (float)window.right;
			m_vpViewPort.Height = (float)window.bottom;
			m_vpViewPort.MinDepth = 0.0f;
			m_vpViewPort.MaxDepth = 1.0f;
			m_vpViewPort.TopLeftX = 0;
			m_vpViewPort.TopLeftY = 0;
			g_dcConext->RSSetViewports(1, &m_vpViewPort);

			XMMATRIX projection;
			projection = XMMatrixPerspectiveFovLH(FOV, (float)window.right / (float)window.bottom, ZNEAR, ZFAR);
			XMStoreFloat4x4(&SceneMatrices.matrix_Projection, projection);

			break;
		}

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//