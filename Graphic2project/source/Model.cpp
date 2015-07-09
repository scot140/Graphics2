#include "Model.h"

#define OBJECT 0
#define SCENE  1

Model::Model()
{
	XMStoreFloat4x4(&m_objMatrix.m_mxConstMatrix, XMMatrixIdentity());


	m_pBuffer = nullptr;
	m_pNormals = nullptr;
	m_pTexture = nullptr;
	m_pIndexBuffer = nullptr;
	m_pShaderResource = nullptr;

	m_nMaxVerts = 0;
	m_nMaxIndices = 0;

	ZeroMemory(&m_aniAnimaiton, sizeof(ANIMATION));
}

void Model::ScaleModel(float size)
{

	XMMATRIX scale = XMMatrixScaling(size, size, size);
	XMMATRIX math = XMLoadFloat4x4(&m_objMatrix.m_mxConstMatrix);
	XMVECTOR pos = math.r[3];
	math *= scale;
	XMStoreFloat4x4(&m_objMatrix.m_mxConstMatrix, math);

	SetModelPosition(pos.m128_f32[0], pos.m128_f32[1], pos.m128_f32[2]);

}

void Model::SetModelPosition(float x, float y, float z)
{
	m_objMatrix.m_mxConstMatrix(3, 0) = x;
	m_objMatrix.m_mxConstMatrix(3, 1) = y;
	m_objMatrix.m_mxConstMatrix(3, 2) = z;
}

void Model::SetAnimation(float maxframe, float width)
{
	m_aniAnimaiton.frame = 0;
	m_aniAnimaiton.maxFrame = maxframe;
	m_aniAnimaiton.width = width;
}

void Model::loadVerts(unsigned int numVerts, XMFLOAT4* Verts, XMFLOAT2* UV)
{
	m_nMaxVerts = numVerts;

	m_vsInput = new INPUT_VERTEX[m_nMaxVerts];

	XMFLOAT4 color = XMFLOAT4(1, 1, 0, 0);

	for (unsigned int i = 0; i < m_nMaxVerts; i++)
	{
		m_vsInput[i].pos = Verts[i];
		m_vsInput[i].col = color;
		m_vsInput[i].uv = UV[i];
	}

	//add normals here later thank you

}

void Model::loadVerts(unsigned int numVerts, INPUT_VERTEX* p_verts)
{
	m_nMaxVerts = numVerts;
	m_vsInput = p_verts;
}

void Model::CreateBuffers(ID3D11Device* device, unsigned int numIndices, const unsigned int* Indices, Scene* scnMatrix)
{
	if (device == nullptr)
	{
		return;
	}

	//Vertex buffer
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = NULL;
	BufferDesc.ByteWidth = sizeof(INPUT_VERTEX) * m_nMaxVerts;
	BufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_vsInput;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	device->CreateBuffer(&BufferDesc, &InitData, &m_pBuffer);

	if (Indices != nullptr)
	{
		//Index Buffer

		//D3D11_BUFFER_DESC
		m_nMaxIndices = numIndices;

		D3D11_BUFFER_DESC IndexBufferDesc;
		IndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		IndexBufferDesc.CPUAccessFlags = NULL;
		IndexBufferDesc.ByteWidth = sizeof(unsigned int)*m_nMaxIndices;
		IndexBufferDesc.MiscFlags = 0;

		//D3D11_SUBRESOURCE_DATA

		D3D11_SUBRESOURCE_DATA InitIndexData;
		InitIndexData.pSysMem = Indices;
		InitIndexData.SysMemPitch = 0;
		InitIndexData.SysMemSlicePitch = 0;

		device->CreateBuffer(&IndexBufferDesc, &InitIndexData, &m_pIndexBuffer);
	}

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

	device->CreateBuffer(&cBufferDesc, &ConstantBufferData, &m_pConstBuffer[OBJECT]);

	//Setting the scene buffer
	cBufferDesc.ByteWidth = sizeof(Scene);
	ConstantBufferData.pSysMem = scnMatrix;

	device->CreateBuffer(&cBufferDesc, &ConstantBufferData, &m_pConstBuffer[SCENE]);

	cBufferDesc.ByteWidth = sizeof(ANIMATION);
	ConstantBufferData.pSysMem = &m_aniAnimaiton;

	device->CreateBuffer(&cBufferDesc, &ConstantBufferData, &m_pConstBuffer_PS);
}

void Model::CreateTexture(ID3D11Device* device, D3D11_SAMPLER_DESC* SamplerDesc, D3D11_TEXTURE2D_DESC* Texture, D3D11_SUBRESOURCE_DATA* SubResource)
{
	device->CreateSamplerState(SamplerDesc, &m_pSamplerState);

	device->CreateTexture2D(Texture, SubResource, &m_pTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceView_Desc;
	ZeroMemory(&ShaderResourceView_Desc, sizeof(ShaderResourceView_Desc));
	ShaderResourceView_Desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	ShaderResourceView_Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceView_Desc.Texture2D.MipLevels = Texture->MipLevels;
	ShaderResourceView_Desc.Texture2D.MostDetailedMip = 0;

	device->CreateShaderResourceView(m_pTexture, &ShaderResourceView_Desc, &m_pShaderResource);

}

void Model::Draw(ID3D11DeviceContext* p_dcContext, ID3D11InputLayout*p_pVertexInput, D3D11_PRIMITIVE_TOPOLOGY p_Topology, ID3D11RasterizerState** p_rasterArray, unsigned int numRaster, float delta)
{
	if (m_aniAnimaiton.maxFrame > 0)
	{
		UpdateAnimation(p_dcContext, delta);
	}

	if (m_pShaderResource)
	{
		p_dcContext->PSSetShaderResources(0, 1, &m_pShaderResource);
	}

	if (m_pSamplerState)
	{
		p_dcContext->PSSetSamplers(0, 1, &m_pSamplerState);
	}


	p_dcContext->VSSetConstantBuffers(0, 2, m_pConstBuffer);


	p_dcContext->PSSetConstantBuffers(0, 1, &m_pConstBuffer_PS);


	unsigned int stride = sizeof(INPUT_VERTEX);
	unsigned int zero = 0;

	p_dcContext->IASetVertexBuffers(0, 1, &m_pBuffer, &stride, &zero);

	p_dcContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	p_dcContext->IASetInputLayout(p_pVertexInput);

	p_dcContext->IASetPrimitiveTopology(p_Topology);

	//setting the renderstate
	for (unsigned int i = 0; i < numRaster; i++)
	{
		p_dcContext->RSSetState(p_rasterArray[i]);

		if (m_nMaxIndices > 0)
		{
			p_dcContext->DrawIndexed(m_nMaxIndices, 0, 0);
		}
		else
		{
			p_dcContext->Draw(m_nMaxVerts, 0);
		}
	}

	if (numRaster < 1)
	{

		if (m_nMaxIndices > 0)
		{
			p_dcContext->DrawIndexed(m_nMaxIndices, 0, 0);
		}
		else
		{
			p_dcContext->Draw(m_nMaxVerts, 0);
		}
	}
}

void Model::UpdateAnimation(ID3D11DeviceContext* p_dcContext, float p_delta)
{

#pragma region Animation

	D3D11_MAPPED_SUBRESOURCE TimerResource;
	ZeroMemory(&TimerResource, sizeof(TimerResource));

	p_dcContext->Map(m_pConstBuffer_PS, 0, D3D11_MAP_WRITE_DISCARD, NULL, &TimerResource);

#pragma region FrameUpdate

	delta += (float)p_delta;

	if (delta >= 1)
	{
		m_aniAnimaiton.frame++;

		if (m_aniAnimaiton.maxFrame <= m_aniAnimaiton.frame)
		{
			m_aniAnimaiton.frame = 0;
		}

		delta = 0;
	}

#pragma endregion

	memcpy(TimerResource.pData, &m_aniAnimaiton, sizeof(m_aniAnimaiton));

	p_dcContext->Unmap(m_pConstBuffer_PS, 0);

#pragma endregion

}

Model::~Model()
{
	if (m_pBuffer)
	{
		m_pBuffer->Release();
		m_pBuffer = nullptr;
	}

	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = nullptr;
	}

	if (m_pShaderResource)
	{
		m_pShaderResource->Release();
		m_pShaderResource = nullptr;
	}

	if (m_pSamplerState)
	{
		m_pSamplerState->Release();
		m_pSamplerState = nullptr;
	}

	if (m_vsInput)
	{
		delete[] m_vsInput;
		m_vsInput = nullptr;
	}



	for (unsigned int i = 0; i < 2; i++)
	{
		if (m_pConstBuffer[i])
		{
			m_pConstBuffer[i]->Release();
			m_pConstBuffer[i] = nullptr;
		}
	}

	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
	}


	if (m_pNormals)
	{
		delete[] m_pNormals;
		m_pNormals = nullptr;
	}



}
