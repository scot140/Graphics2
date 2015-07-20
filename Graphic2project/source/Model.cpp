#include "Model.h"
#include "DDSTextureLoader.h"

#define OBJECT 0
#define SCENE  1

Model::Model()
{
	XMStoreFloat4x4(&m_objMatrix.m_mxConstMatrix, XMMatrixIdentity());


	ZERO_OUT(m_pBuffer);
	ZERO_OUT(m_pTexture);
	ZERO_OUT(m_pIndexBuffer);
	ZERO_OUT(m_pShaderResource);
	ZERO_OUT(m_nMaxVerts);
	ZERO_OUT(m_nMaxIndices);
	ZERO_OUT(m_pSamplerState);
	ZERO_OUT(m_vsInput);
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

XMFLOAT3 Model::GetModelPosition()
{
	XMFLOAT3 pos;
	pos.x = m_objMatrix.m_mxConstMatrix(3, 0);
	pos.y = m_objMatrix.m_mxConstMatrix(3, 1);
	pos.z = m_objMatrix.m_mxConstMatrix(3, 2);

	return pos;
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

void Model::CreateBuffers(ID3D11Device* device, unsigned int numIndices, const unsigned int* Indices, unsigned int p_num, InstanceType* p_data)
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

	if (p_data != nullptr)
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		// create instance buffer 

		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Index Buffer
		instCount = p_num;
		//D3D11_BUFFER_DESC
		D3D11_BUFFER_DESC InstanceBufferDesc;
		ZERO_OUT(InstanceBufferDesc);
		InstanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		InstanceBufferDesc.CPUAccessFlags = NULL;
		InstanceBufferDesc.ByteWidth = sizeof(InstanceType)*instCount;
		InstanceBufferDesc.MiscFlags = 0;

		//D3D11_SUBRESOURCE_DATA
		D3D11_SUBRESOURCE_DATA InstanceData;
		InstanceData.pSysMem = p_data;
		InstanceData.SysMemPitch = 0;
		InstanceData.SysMemSlicePitch = 0;

		device->CreateBuffer(&InstanceBufferDesc, &InstanceData, &m_pInstanceBuffer);
	}
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

void Model::CreateTexture(ID3D11Device* device, const wchar_t* filename, const wchar_t* Secondfilename, D3D11_SAMPLER_DESC* p_sampler)
{
	device->CreateSamplerState(p_sampler, &m_pSamplerState);

	CreateDDSTextureFromFile(device, filename, nullptr, &m_pShaderResource);

	if (Secondfilename != nullptr)
	{
		CreateDDSTextureFromFile(device, Secondfilename, nullptr, &m_pSecondShaderResource);
	}
}

void Model::Draw(ID3D11DeviceContext* p_dcContext, ID3D11InputLayout*p_pVertexInput, D3D11_PRIMITIVE_TOPOLOGY p_Topology, ID3D11RasterizerState** p_rasterArray, unsigned int numRaster)
{

	if (m_pSecondShaderResource)
	{
		ID3D11ShaderResourceView* ShaderResourceArray[2];
		ShaderResourceArray[0] = m_pShaderResource;
		ShaderResourceArray[1] = m_pSecondShaderResource;

		p_dcContext->PSSetShaderResources(0, 2, ShaderResourceArray);
	}
	else if (m_pShaderResource)
	{
		p_dcContext->PSSetShaderResources(0, 1, &m_pShaderResource);
	}

	if (m_pSamplerState)
	{
		p_dcContext->PSSetSamplers(0, 1, &m_pSamplerState);
	}


	if (m_pInstanceBuffer)
	{
		unsigned int zero[2];
		zero[0] = 0;
		zero[1] = 0;

		unsigned int stride[2];
		stride[0] = sizeof(INPUT_VERTEX);
		stride[1] = sizeof(InstanceType);

		ID3D11Buffer* buffers[2];
		buffers[0] = m_pBuffer;
		buffers[1] = m_pInstanceBuffer;

		p_dcContext->IASetVertexBuffers(0, 2, buffers, stride, zero);
	}
	else if (m_pBuffer)
	{
		unsigned int zero = 0;
		unsigned int stride = sizeof(INPUT_VERTEX);
		p_dcContext->IASetVertexBuffers(0, 1, &m_pBuffer, &stride, &zero);
	}

	p_dcContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	p_dcContext->IASetInputLayout(p_pVertexInput);

	p_dcContext->IASetPrimitiveTopology(p_Topology);

	//setting the renderstate
	for (unsigned int i = 0; i < numRaster; i++)
	{
		p_dcContext->RSSetState(p_rasterArray[i]);

		if (m_pInstanceBuffer && m_nMaxIndices > 0)
		{
			p_dcContext->DrawIndexedInstanced(m_nMaxIndices, instCount, 0, 0, 0);
		}
		else if (m_nMaxIndices > 0)
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
		p_dcContext->RSSetState(nullptr);

		if (m_pInstanceBuffer && m_nMaxIndices > 0)
		{
			p_dcContext->DrawIndexedInstanced(m_nMaxIndices, instCount, 0, 0, 0);
		}
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

void Model::SetShaderResourceView(ID3D11ShaderResourceView* main, ID3D11ShaderResourceView* second)
{
	if (main)
	{
		SAFE_RELEASE(m_pShaderResource);
		m_pShaderResource = main;
		m_pShaderResource->AddRef();
	}

	if (second)
	{
		SAFE_RELEASE(m_pSecondShaderResource);
		m_pSecondShaderResource = second;
		m_pSecondShaderResource->AddRef();
	}
}

void Model::Release()
{
	SAFE_RELEASE(m_pInstanceBuffer);

	SAFE_RELEASE(m_pSecondShaderResource);

	SAFE_RELEASE(m_pBuffer);


	SAFE_RELEASE(m_pTexture);


	SAFE_RELEASE(m_pShaderResource);


	SAFE_RELEASE(m_pSamplerState);


	if (m_vsInput)
	{
		delete[] m_vsInput;
		m_vsInput = nullptr;
	}


	SAFE_RELEASE(m_pIndexBuffer);

}

Model::~Model()
{

}
