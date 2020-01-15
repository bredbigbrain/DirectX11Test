#include "stdafx.h"
#include "Model.h"
#include "Defines.h"
#include "Globals.h"

bool Model::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, XMFLOAT3 position, VertexDataType eVertexDataType, const char* szTexturePath)
{
	m_position = position;
	m_eVertexDataType = eVertexDataType;

	if (!InitializeBuffers(pDevice))
		RETURN_AND_LOG(false);

	if (!LoadTexture(pDevice, pDeviceContext, szTexturePath))
		RETURN_AND_LOG(false);


	return true;
}

bool Model::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, VertexDataType eVertexDataType, const SInitOptionalParams& optionalPrms)
{
	m_position = optionalPrms.m_position;
	m_scale = optionalPrms.m_scale;
	m_eVertexDataType = eVertexDataType;

	if(!InitializeBuffers(pDevice))
		RETURN_AND_LOG(false);

	SetTexture(optionalPrms.m_pTexture);

	return true;
}

void Model::Render(ID3D11DeviceContext* pDeviceContext)
{
	RenderBuffers(pDeviceContext);
}

void Model::Shutdown()
{
	if(m_bHandleTexture)
		SHUTDOWN_DELETE(m_pTexture);

	RELEASE_AND_NULL(m_pVertexBuffer);
	RELEASE_AND_NULL(m_pIndexBuffer);
}

int Model::GetIndexCount() const
{
	return m_nIndexCount;
}

void Model::SetTexture(CTexture* pTexture)
{
	m_bHandleTexture = false;
	m_pTexture = pTexture;
}

ID3D11ShaderResourceView* Model::GetTexture() const
{
	return m_pTexture->GetTexture();
}

bool Model::InitializeBuffers(ID3D11Device* pDevice)
{
	auto arrVertices = CreateVertices(m_eVertexDataType);
	
	m_nIndexCount = 6;
	auto arrIndices = new unsigned int[m_nIndexCount];
	arrIndices[0] = 0;
	arrIndices[1] = 1;
	arrIndices[2] = 2;
	arrIndices[3] = 0;
	arrIndices[4] = 2;
	arrIndices[5] = 3;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = GetVertexSize() * m_nVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	vertexData.pSysMem = arrVertices;

	if (FAILED(pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer)))
		RETURN_AND_LOG(false);

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_nIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	indexData.pSysMem = arrIndices;

	if (FAILED(pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer)))
		RETURN_AND_LOG(false);

	delete[] arrVertices;
	delete[] arrIndices;

	return true;
}

void* Model::CreateVertices(VertexDataType eVertexDataType)
{
	using namespace Math3DNS;	//XMFLOAT3 operator +

	m_nVertexCount = 4;

	switch(eVertexDataType)
	{
	case VertexDataType::TEXTURE:
	{
		auto arrVertices = new Vertex[m_nVertexCount];
		arrVertices[0].position = m_position + XMFLOAT3(0.f, 0.f, 0.f) * m_scale;
		arrVertices[0].texCoord = XMFLOAT2(0.f, 0.f);

		arrVertices[1].position = m_position + XMFLOAT3(0.f, 1.f, 0.f);
		arrVertices[1].texCoord = XMFLOAT2(0.f, 1.f);

		arrVertices[2].position = m_position + XMFLOAT3(1.f, 1.f, 0.f) * m_scale;
		arrVertices[2].texCoord = XMFLOAT2(1.f, 1.f);

		arrVertices[3].position = m_position + XMFLOAT3(1.f, 0.f, 0.f) * m_scale;
		arrVertices[3].texCoord = XMFLOAT2(1.f, 0.f);

		return arrVertices;
	}
	case VertexDataType::TEXTURE_N:
	{
		auto arrVertices = new Vertex_N[m_nVertexCount];
		arrVertices[0].position = m_position + XMFLOAT3(0.f, 0.f, 0.f) * m_scale;
		arrVertices[0].texCoord = XMFLOAT2(0.f, 0.f);
		arrVertices[0].normal = XMFLOAT3(0, 0, -1);

		arrVertices[1].position = m_position + XMFLOAT3(0.f, 1.f, 0.f) * m_scale;
		arrVertices[1].texCoord = XMFLOAT2(0.f, 1.f);
		arrVertices[1].normal = XMFLOAT3(1, 0, -1);

		arrVertices[2].position = m_position + XMFLOAT3(1.f, 1.f, 0.f) * m_scale;
		arrVertices[2].texCoord = XMFLOAT2(1.f, 1.f);
		arrVertices[2].normal = XMFLOAT3(1, 0, -1);

		arrVertices[3].position = m_position + XMFLOAT3(1.f, 0.f, 0.f) * m_scale;
		arrVertices[3].texCoord = XMFLOAT2(1.f, 0.f);
		arrVertices[3].normal = XMFLOAT3(1, 0, -1);

		return arrVertices;
	}
	case VertexDataType::TEXTURE_NBCm:
	{
		auto arrVertices = new Vertex_NBCm[m_nVertexCount];
		arrVertices[0].position = m_position + XMFLOAT3(0.f, 0.f, 0.f) * m_scale;
		arrVertices[0].texCoord = XMFLOAT2(0.f, 0.f);
		arrVertices[0].normal = XMFLOAT3(0, 0, -1);
		arrVertices[0].color = XMFLOAT3(1, 1, 1);
		arrVertices[0].binormal = XMFLOAT3(0, 1, 0);
		arrVertices[0].tangent = XMFLOAT3(1, 0, 0);

		arrVertices[1].position = m_position + XMFLOAT3(0.f, 1.f, 0.f) * m_scale;
		arrVertices[1].texCoord = XMFLOAT2(0.f, 1.f);
		arrVertices[1].normal = XMFLOAT3(1, 0, -1);
		arrVertices[1].color = XMFLOAT3(1, 1, 1);
		arrVertices[1].binormal = XMFLOAT3(0, 1, 0);
		arrVertices[1].tangent = XMFLOAT3(1, 0, 0);

		arrVertices[2].position = m_position + XMFLOAT3(1.f, 1.f, 0.f) * m_scale;
		arrVertices[2].texCoord = XMFLOAT2(1.f, 1.f);
		arrVertices[2].normal = XMFLOAT3(1, 0, -1);
		arrVertices[2].color = XMFLOAT3(1, 1, 1);
		arrVertices[2].binormal = XMFLOAT3(0, 1, 0);
		arrVertices[2].tangent = XMFLOAT3(1, 0, 0);

		arrVertices[3].position = m_position + XMFLOAT3(1.f, 0.f, 0.f) * m_scale;
		arrVertices[3].texCoord = XMFLOAT2(1.f, 0.f);
		arrVertices[3].normal = XMFLOAT3(1, 0, -1);
		arrVertices[3].color = XMFLOAT3(1, 1, 1);
		arrVertices[3].binormal = XMFLOAT3(0, 1, 0);
		arrVertices[3].tangent = XMFLOAT3(1, 0, 0);

		return arrVertices;
	}
	}
}

bool Model::LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* szTexturePath)
{
	m_bHandleTexture = true;
	m_pTexture = new CTexture;

	return m_pTexture->Initialize(pDevice, pDeviceContext, szTexturePath);
}

void Model::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
{
	unsigned int nStride = GetVertexSize();
	unsigned int nOffset{0};

	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &nStride, &nOffset);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

size_t Model::GetVertexSize() const
{
	switch(m_eVertexDataType)
	{
	case VertexDataType::TEXTURE: return sizeof(Vertex);
	case VertexDataType::TEXTURE_N: return sizeof(Vertex_N);
	case VertexDataType::TEXTURE_NBCm: return sizeof(Vertex_NBCm);
	default: assert(false); return 0;
	}
}
