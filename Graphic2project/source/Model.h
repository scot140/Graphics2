#pragma once
#include "DefineLibrary.h"



class Model
{
	INPUT_VERTEX *				m_vsInput;

public:

	ANIMATION					m_aniAnimaiton;
	Object						m_objMatrix;
	ID3D11Buffer *				m_pBuffer;
	ID3D11Buffer *				m_pInstanceBuffer;
	ID3D11Buffer *				m_pIndexBuffer;
	ID3D11Texture2D *			m_pTexture;
	ID3D11ShaderResourceView *	m_pSecondShaderResource;
	ID3D11ShaderResourceView *	m_pShaderResource;
	ID3D11SamplerState *		m_pSamplerState;
	unsigned int				m_nMaxVerts;
	unsigned int				m_nMaxIndices;
	float						delta;
	unsigned int				instCount;
	//constructor
	Model();

	//functions
	void ScaleModel(float size);
	void SetModelPosition(float x, float y, float z);
	XMFLOAT3 GetModelPosition();
	void SetAnimation(float maxframe, float width);
	void loadVerts(unsigned int numVerts, XMFLOAT4* Verts, XMFLOAT2* UV = nullptr);
	void loadVerts(unsigned int numVerts, INPUT_VERTEX* p_verts);
	void CreateBuffers(ID3D11Device* device, unsigned int numIndices, const unsigned int* Indices, unsigned int p_num = 0, InstanceType* p_data = nullptr);
	void CreateTexture(ID3D11Device* device, const wchar_t* filename, const wchar_t* Secondfilename, D3D11_SAMPLER_DESC* p_sampler);
	void CreateTexture(ID3D11Device* device, D3D11_SAMPLER_DESC* SamplerDesc, D3D11_TEXTURE2D_DESC* Texture, D3D11_SUBRESOURCE_DATA* SubResource);
	void Draw(ID3D11DeviceContext* p_dcContext, ID3D11InputLayout*p_pVertexInput, D3D11_PRIMITIVE_TOPOLOGY p_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, ID3D11RasterizerState** p_rasterArray = nullptr, unsigned int numRaster = 0);
	void SetShaderResourceView(ID3D11ShaderResourceView* main = nullptr, ID3D11ShaderResourceView* second = nullptr);
	//destructor
	void Release();
	virtual ~Model();
};

