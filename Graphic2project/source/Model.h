#pragma once
#include <DirectXMath.h>
using namespace DirectX;

#include "DefineLibrary.h"


class Model
{
	XMFLOAT3 *					m_pNormals;
	XMFLOAT4 *					m_pVerts;
	XMFLOAT3 *					m_pUVs;
	ANIMATION					m_aniAnimaiton;
	float						delta;

public:

	XMFLOAT4X4					m_mxMatrix;
	Object						m_objMatrix;
	ID3D11Buffer *				m_pBuffer;
	ID3D11Buffer *				m_pConstBuffer[2]; //vertex Shader's constant buffers
	ID3D11Buffer *				m_pConstBuffer_PS;
	ID3D11Buffer *				m_pIndexBuffer;
	ID3D11Texture2D *			m_pTexture;
	ID3D11ShaderResourceView *	m_pShaderResource;
	ID3D11SamplerState *		m_pSamplerState;
	unsigned int				m_nMaxVerts;
	unsigned int				m_nMaxIndices;

	//constructor
	Model();

	//functions
	void ScaleModel(float size);
	void SetModelPosition(float x, float y, float z);
	void SetAnimation(float maxframe, float width);
	void loadVerts(unsigned int numVerts, XMFLOAT4* Verts, XMFLOAT3* UV = nullptr);
	void loadVerts(unsigned int numVerts, float* Verts, float* UV = nullptr);
	void CreateBuffers(ID3D11Device* device, unsigned int numIndices, float* Indices, Scene* scnMatrix);
	void CreateTexture(ID3D11Device* device, D3D11_SAMPLER_DESC* SamplerDesc, D3D11_TEXTURE2D_DESC* Texture, D3D11_SUBRESOURCE_DATA* SubResource);
	void UpdateAnimation(ID3D11DeviceContext* p_dcContext, float p_delta);
	void Draw(ID3D11DeviceContext* p_dcContext, ID3D11InputLayout*p_pVertexInput, D3D11_PRIMITIVE_TOPOLOGY p_Topology, ID3D11RasterizerState** p_rasterArray = nullptr, unsigned int numRaster = 0, float p_delta = 0);
	
	//destructor
	virtual ~Model();
};

