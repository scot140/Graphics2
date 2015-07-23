#pragma once

#include "DefineLibrary.h"
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <map>

#pragma warning(disable : 4996)

using namespace std;


//Funtion headers
void TangentCreation(INPUT_VERTEX** test, unsigned int** p_Indices, unsigned int& size, unsigned int& maxIndices);

//Input is only for the Buffer Count, Buffer Width, Buffer Height, and Window to render to
//Output is the first parameter for a DXGI_SWAP_CHAIN_DESC
void CreateSwapChainDesc(DXGI_SWAP_CHAIN_DESC *m_scDesc, HWND* window, int bufferCount = 1, int bufferWidth = 500, int bufferHeight = 500);

//You only need to insert the first 5 parameters
HRESULT DefaultDeviceAndSwapChain(unsigned int deviceFlag, DXGI_SWAP_CHAIN_DESC* m_scDesc, IDXGISwapChain** m_snSwapChain, ID3D11Device** m_iDevice, ID3D11DeviceContext** m_dcConext, IDXGIAdapter* adapter = nullptr, D3D_DRIVER_TYPE _type = D3D_DRIVER_TYPE_HARDWARE, HMODULE* _software = nullptr, D3D_FEATURE_LEVEL* FeatureLevel = nullptr, unsigned int LevelCount = 0, unsigned int version = D3D10_SDK_VERSION, D3D_FEATURE_LEVEL* output = nullptr);

//Function Bodies
void CreateSwapChainDesc(DXGI_SWAP_CHAIN_DESC *m_scDesc, HWND* window, int bufferCount, int bufferWidth, int bufferHeight)
{
	ZeroMemory(m_scDesc, sizeof(*m_scDesc));
	m_scDesc->BufferCount = bufferCount;
	m_scDesc->BufferDesc.Width = bufferWidth;
	m_scDesc->BufferDesc.Height = bufferHeight;
	m_scDesc->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
	BlendDesc->AlphaToCoverageEnable = false;
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

void DefaultSamplerStateDesc(D3D11_SAMPLER_DESC* SamplerDesc, D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP)
{
	ZeroMemory(SamplerDesc, sizeof(*SamplerDesc));

	SamplerDesc->Filter = filter;
	SamplerDesc->AddressU = mode;
	SamplerDesc->AddressV = mode;
	SamplerDesc->AddressW = mode;
	SamplerDesc->MinLOD = -FLT_MAX;
	SamplerDesc->MaxLOD = FLT_MAX;
	SamplerDesc->MipLODBias = 0.0f;
	SamplerDesc->MaxAnisotropy = 1;
	SamplerDesc->ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc->BorderColor[0] = 1.0f;
	SamplerDesc->BorderColor[1] = 1.0f;
	SamplerDesc->BorderColor[2] = 1.0f;
	SamplerDesc->BorderColor[3] = 1.0f;
}

void DefaultTextureDesc(D3D11_TEXTURE2D_DESC* TextureDesc, unsigned int width, unsigned int height, unsigned int numlevels)
{
	ZeroMemory(TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc->Width = width;
	TextureDesc->Height = height;
	TextureDesc->MipLevels = numlevels;
	TextureDesc->ArraySize = 1;
	TextureDesc->Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	TextureDesc->BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc->SampleDesc.Count = 1;
	TextureDesc->SampleDesc.Quality = 0;
}

void DefaultTextureSubresource(D3D11_SUBRESOURCE_DATA* resource, const unsigned int* pixels, const unsigned int* offset, unsigned int width, unsigned int numlevels)
{
	ZeroMemory(resource, sizeof(D3D11_SUBRESOURCE_DATA) * numlevels);

	for (unsigned int i = 0; i < numlevels; i++)
	{
		resource[i].pSysMem = &pixels[offset[i]];
		resource[i].SysMemPitch = (width >> i) * 4;
		resource[i].SysMemSlicePitch = 0;
	}
}

void MultithreadLoader(bool* p_locked, mutex* m_pThreadLock, condition_variable* m_pCondition, const char * filepath, INPUT_VERTEX** test, unsigned int** p_Indices, unsigned int& size, unsigned int& maxIndices)
{
	struct Objhelper
	{
		int vertsIndex;
		int uvIndex;
		int normIndex;
		string mapIndex;
	};

	vector<XMFLOAT4> verts;
	vector<XMFLOAT2> uvs;
	vector<XMFLOAT3> normals;

	vector<Objhelper> objectIndices;
	vector<Objhelper> StoragetIndices;
	vector<INPUT_VERTEX> sampleVerts;
	vector<unsigned int>  normalIndices, Indices;

	FILE* myfile;

	fopen_s(&myfile, filepath, "r");

	if (myfile == nullptr)
	{
		printf_s("Wrong File Path");
		return;
	}

#pragma region reading in the file
	while (true)
	{
		char lineHeader[500];

		int res = fscanf(myfile, "%s", lineHeader);
		if (res == EOF)
		{
			break;
		}

		if (strcmp(lineHeader, "v") == 0)
		{
			XMFLOAT4 vertex;
			fscanf(myfile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertex.w = 1;
			verts.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			XMFLOAT2 uv;
			fscanf(myfile, "%f %f\n", &uv.x, &uv.y);
			uv.y = 1 - uv.y;

			uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			XMFLOAT3 norm;
			fscanf(myfile, "%f %f %f\n", &norm.x, &norm.y, &norm.z);

			normals.push_back(norm);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

			//fscanf(myfile, "%[^\n]s", &mapIndex[0]);

			int matches = fscanf(myfile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);



			//sprintf_s(mapIndex[0].c_str(), 100, "%d/%d/%d", &vertexIndex[0], &uvIndex[0], &normalIndex[0]);

			if (matches != 9){
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return;
			}

			Objhelper helper;

			stringstream output;
			for (unsigned int i = 0; i < 3; i++)
			{

				output << vertexIndex[i] << uvIndex[i] << normalIndex[i]<<"\0";
				helper.uvIndex = uvIndex[i] - 1;
				helper.vertsIndex = vertexIndex[i] - 1;
				helper.normIndex = normalIndex[i] - 1;
				helper.mapIndex = output.str();
				objectIndices.push_back(helper);

			}
		}
	}

	fclose(myfile);

#pragma endregion

	bool push = true;

	//index finder
	for (unsigned int i = 0, ind = 0; i < objectIndices.size(); i++, ind = 0)
	{
		for (unsigned int index = 0; index < StoragetIndices.size(); index++, ind++)
		{
			if (objectIndices[i].vertsIndex == StoragetIndices[index].vertsIndex)
			{
				push = false;
				Indices.push_back(ind);
				break;
			}
		}

		if (push)
		{
			Indices.push_back(ind);
			StoragetIndices.push_back(objectIndices[i]);
		}
		push = true;
	}


	(*p_Indices) = new unsigned int[Indices.size()];
	maxIndices = Indices.size();
	for (unsigned int i = 0; i < Indices.size(); i++)
	{
		unsigned int index = Indices[i];
		(*p_Indices)[i] = index;
	}

	// Creating the vertexes
	size = StoragetIndices.size();
	(*test) = new INPUT_VERTEX[size];

	for (unsigned int i = 0; i < size; i++)
	{
		Objhelper finder = StoragetIndices[i];
		XMFLOAT4 foundVert = verts[finder.vertsIndex];
		XMFLOAT2 foundUV = uvs[finder.uvIndex];
		XMFLOAT3 foundNorm = normals[finder.normIndex];
		(*test)[i].pos = foundVert;
		(*test)[i].uv = foundUV;
		(*test)[i].normals = foundNorm;
		(*test)[i].col = XMFLOAT4(1, 1, 0, 0);
	}
	TangentCreation(test, p_Indices, size, maxIndices);
}

bool ObjectLoader(const char * filepath, INPUT_VERTEX** test, unsigned int** p_Indices, unsigned int& size, unsigned int& maxIndices)
{
	struct Objhelper
	{
		int vertsIndex;
		int uvIndex;
		int normIndex;
	};

	vector<XMFLOAT4> verts;
	vector<XMFLOAT2> uvs;
	vector<XMFLOAT3> normals;

	vector<Objhelper> objectIndices;
	vector<Objhelper> StoragetIndices;
	vector<INPUT_VERTEX> sampleVerts;
	vector<unsigned int>  normalIndices, Indices;

	FILE* myfile;

	fopen_s(&myfile, filepath, "r");

	if (myfile == nullptr)
	{
		printf_s("Wrong File Path");
		return false;
	}

#pragma region reading in the file
	while (true)
	{
		char lineHeader[500];

		int res = fscanf(myfile, "%s", lineHeader);
		if (res == EOF)
		{
			break;
		}

		if (strcmp(lineHeader, "v") == 0)
		{
			XMFLOAT4 vertex;
			fscanf(myfile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertex.w = 1;
			verts.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			XMFLOAT2 uv;
			fscanf(myfile, "%f %f\n", &uv.x, &uv.y);
			uv.y = 1 - uv.y;

			uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			XMFLOAT3 norm;
			fscanf(myfile, "%f %f %f\n", &norm.x, &norm.y, &norm.z);

			normals.push_back(norm);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(myfile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			if (matches != 9){
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}

			Objhelper helper;

			for (unsigned int i = 0; i < 3; i++)
			{

				helper.uvIndex = uvIndex[i] - 1;
				helper.vertsIndex = vertexIndex[i] - 1;
				helper.normIndex = normalIndex[i] - 1;
				objectIndices.push_back(helper);

			}
		}
	}

	fclose(myfile);
#pragma endregion

	bool push = true;

	//index finder
	for (unsigned int i = 0, ind = 0; i < objectIndices.size(); i++, ind = 0)
	{
		for (unsigned int index = 0; index < StoragetIndices.size(); index++, ind++)
		{
			if (objectIndices[i].vertsIndex == StoragetIndices[index].vertsIndex)
			{
				push = false;
				Indices.push_back(ind);
				break;
			}
		}

		if (push)
		{
			Indices.push_back(ind);
			StoragetIndices.push_back(objectIndices[i]);
		}
		push = true;
	}


	(*p_Indices) = new unsigned int[Indices.size()];
	maxIndices = Indices.size();
	for (unsigned int i = 0; i < Indices.size(); i++)
	{
		unsigned int index = Indices[i];
		(*p_Indices)[i] = index;
	}

	// Creating the vertexes
	size = StoragetIndices.size();
	(*test) = new INPUT_VERTEX[size];

	for (unsigned int i = 0; i < size; i++)
	{
		Objhelper finder = StoragetIndices[i];
		XMFLOAT4 foundVert = verts[finder.vertsIndex];
		XMFLOAT2 foundUV = uvs[finder.uvIndex];
		XMFLOAT3 foundNorm = normals[finder.normIndex];
		(*test)[i].pos = foundVert;
		(*test)[i].uv = foundUV;
		(*test)[i].normals = foundNorm;
		(*test)[i].col = XMFLOAT4(1, 1, 0, 0);
	}
	TangentCreation(test, p_Indices, size, maxIndices);
	return true;
}

bool CubeObjectLoader(const char * filepath, INPUT_VERTEX** test, unsigned int** p_Indices, unsigned int& size, unsigned int& maxIndices)
{
	struct Objhelper
	{
		int vertsIndex;
		int uvIndex;
		int normIndex;
		string mapIndex;
	};

	vector<XMFLOAT4> verts;
	vector<XMFLOAT2> uvs;
	vector<XMFLOAT3> normals;

	vector<Objhelper> objectIndices;
	vector<Objhelper> StoragetIndices;
	vector<INPUT_VERTEX> sampleVerts;
	vector<unsigned int>  normalIndices, Indices;

	FILE* myfile;

	fopen_s(&myfile, filepath, "r");

	if (myfile == nullptr)
	{
		printf_s("Wrong File Path");
		return false;
	}

#pragma region reading in the file
	while (true)
	{
		char lineHeader[500];

		int res = fscanf(myfile, "%s", lineHeader);
		if (res == EOF)
		{
			break;
		}

		if (strcmp(lineHeader, "v") == 0)
		{
			XMFLOAT4 vertex;
			fscanf(myfile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertex.w = 1;
			verts.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			XMFLOAT2 uv;
			fscanf(myfile, "%f %f\n", &uv.x, &uv.y);
			uv.y = 1 - uv.y;

			uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			XMFLOAT3 norm;
			fscanf(myfile, "%f %f %f\n", &norm.x, &norm.y, &norm.z);

			normals.push_back(norm);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

			int matches = fscanf(myfile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			if (matches != 9){
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}

			Objhelper helper;

			for (unsigned int i = 0; i < 3; i++)
			{

				helper.uvIndex = uvIndex[i] - 1;
				helper.vertsIndex = vertexIndex[i] - 1;
				helper.normIndex = normalIndex[i] - 1;

				objectIndices.push_back(helper);

			}
		}
	}

	fclose(myfile);
#pragma endregion

	bool push = true;

	//index finder
	for (unsigned int i = 0, ind = 0; i < objectIndices.size(); i++, ind++)
	{
		Indices.push_back(ind);
		StoragetIndices.push_back(objectIndices[i]);
	}


	(*p_Indices) = new unsigned int[Indices.size()];
	maxIndices = Indices.size();
	for (unsigned int i = 0; i < Indices.size(); i++)
	{
		unsigned int index = Indices[i];
		(*p_Indices)[i] = index;
	}

	// Creating the vertexes
	size = StoragetIndices.size();
	(*test) = new INPUT_VERTEX[size];

	for (unsigned int i = 0; i < size; i++)
	{
		Objhelper finder = StoragetIndices[i];
		XMFLOAT4 foundVert = verts[finder.vertsIndex];
		XMFLOAT2 foundUV = uvs[finder.uvIndex];
		XMFLOAT3 foundNorm = normals[finder.normIndex];
		(*test)[i].pos = foundVert;
		(*test)[i].uv = foundUV;
		(*test)[i].normals = foundNorm;
		(*test)[i].col = XMFLOAT4(1, 1, 0, 0);
	}
	TangentCreation(test, p_Indices, size, maxIndices);

	return true;
}

void TangentCreation(INPUT_VERTEX** test, unsigned int** p_Indices, unsigned int& size, unsigned int& maxIndices)
{

	if (size < 3)
	{
		return;
	}

	unsigned int Index = 0;

	struct tangHlpr
	{
		XMFLOAT4 pos[3];
		XMFLOAT2 uv[3];
	};


	float ratio;
	tangHlpr helper;
	XMVECTOR texEdges[2];
	XMVECTOR vertEdges[2];
	XMVECTOR u_Direction;
	XMVECTOR v_Direction;
	unsigned int LastIndex = 0;

	do
	{

		ZERO_OUT(texEdges);
		ZERO_OUT(vertEdges);

		for (; (Index % 3) != 0 || Index == LastIndex; Index++)
		{
			helper.pos[(Index % 3)] = (*test)[(*p_Indices)[Index]].pos;
			helper.uv[(Index % 3)] = (*test)[(*p_Indices)[Index]].uv;
		}

		////////////////////////////////////////////////////////////////////////////////////////
		vertEdges[0] = XMLoadFloat4(&helper.pos[1]) - XMLoadFloat4(&helper.pos[0]);

		vertEdges[1] = XMLoadFloat4(&helper.pos[2]) - XMLoadFloat4(&helper.pos[0]);
		////////////////////////////////////////////////////////////////////////////////////////
		texEdges[0] = XMLoadFloat2(&helper.uv[1]) - XMLoadFloat2(&helper.uv[0]);

		texEdges[1] = XMLoadFloat2(&helper.uv[2]) - XMLoadFloat2(&helper.uv[0]);
		////////////////////////////////////////////////////////////////////////////////////////

		ratio = 1.0f / (texEdges[0].m128_f32[0] * texEdges[1].m128_f32[1] - texEdges[1].m128_f32[0] * texEdges[0].m128_f32[1]);

		//tangent Vector
		float	compX = (texEdges[1].m128_f32[1] * vertEdges[0].m128_f32[0]);
		compX = compX - (texEdges[0].m128_f32[1] * vertEdges[1].m128_f32[0]);
		compX = compX * ratio;


		float	compY = (texEdges[1].m128_f32[1] * vertEdges[0].m128_f32[1]);
		compY = compY - (texEdges[0].m128_f32[1] * vertEdges[1].m128_f32[1]);
		compY = compY * ratio;

		float	compZ = (texEdges[1].m128_f32[1] * vertEdges[0].m128_f32[2]);
		compZ = compZ - (texEdges[0].m128_f32[1] * vertEdges[1].m128_f32[2]);
		compZ = compZ * ratio;

		u_Direction = XMVectorSet(compX, compY, compZ, 1);


		//Handness vector
		v_Direction = XMVectorSet(
			(texEdges[0].m128_f32[0] * vertEdges[1].m128_f32[0] - texEdges[1].m128_f32[0] * vertEdges[0].m128_f32[0]) * ratio,
			(texEdges[0].m128_f32[0] * vertEdges[1].m128_f32[1] - texEdges[1].m128_f32[0] * vertEdges[0].m128_f32[1]) * ratio,
			(texEdges[0].m128_f32[0] * vertEdges[1].m128_f32[2] - texEdges[1].m128_f32[0] * vertEdges[0].m128_f32[2]) * ratio, 1);

		///Calulating each of the normals
		u_Direction = XMVector3Normalize(u_Direction);
		v_Direction = XMVector3Normalize(v_Direction);

		Index -= 3;
		for (; (Index % 3) != 0 || Index == LastIndex; Index++)
		{
			INPUT_VERTEX* temp = &(*test)[(*p_Indices)[Index]];
			///finding the tangent without the handedness
			XMVECTOR normal = XMVectorSet(temp->normals.x, temp->normals.y, temp->normals.z, 1);

			float dotResult = XMVector3Dot(normal, u_Direction).m128_f32[0];

			XMVECTOR tangent = u_Direction - normal * dotResult;

			tangent = XMVector3Normalize(tangent);

			///handedness
			XMVECTOR cross = XMVector3Cross(normal, u_Direction);

			dotResult = XMVector3Dot(cross, v_Direction).m128_f32[0];

			tangent.m128_f32[3] = (dotResult > 0.0f) ? -1.0f : 1.0f;

			XMStoreFloat4(&temp->tangents, tangent);
		}




		LastIndex = Index;
	} while (Index < maxIndices);
}
