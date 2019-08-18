#include "stdafx.h"
#include "Model.h"
#include "Defines.h"

Model::Model()
{
}


Model::~Model()
{
}

bool Model::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* szTexturePath)
{
	if (!InitializeBuffers(pDevice))
		RETURN_AND_LOG(false);

	if (!LoadTexture(pDevice, pDeviceContext, szTexturePath))
		RETURN_AND_LOG(false);

	return true;
}

void Model::Render(ID3D11DeviceContext* pDeviceContext)
{
	RenderBuffers(pDeviceContext);
}

void Model::Shutdown()
{
	SHUTDOWND_DELETE(m_pTexture);

	RELEASE_AND_NULL(m_pVertexBuffer);
	RELEASE_AND_NULL(m_pIndexBuffer);
}

int Model::GetIndexCount()
{
	return m_nIndexCount;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return m_pTexture->GetTexture();
}

bool Model::InitializeBuffers(ID3D11Device* pDevice)
{
	m_nVertexCount = 3;
	m_nIndexCount = 3;

	Vertex* arrVertices = new Vertex[m_nVertexCount];
	unsigned int* arrIndices = new unsigned int[m_nIndexCount];

	arrVertices[0].position = XMFLOAT3(-1.f, -1.f, 0.f);
	arrVertices[0].texture = XMFLOAT2(0.f, 1.f);

	arrVertices[1].position = XMFLOAT3(0.f, 1.f, 0.f);
	arrVertices[1].texture = XMFLOAT2(0.5f, 0.f);

	arrVertices[2].position = XMFLOAT3(1.f, -1.f, 0.f);
	arrVertices[2].texture = XMFLOAT2(1.f, 1.f);

	arrIndices[0] = 0;
	arrIndices[1] = 1;
	arrIndices[2] = 2;

	D3D11_BUFFER_DESC vertexBufferDesc;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_nVertexCount;
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

bool Model::LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* szTexturePath)
{
	m_pTexture = new CTexture;

	return m_pTexture->Initialize(pDevice, pDeviceContext, szTexturePath);
}

void Model::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
{
	unsigned int nStride = sizeof(Vertex);
	unsigned int nOffset{0};

	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &nStride, &nOffset);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}