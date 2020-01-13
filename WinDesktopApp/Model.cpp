#include "stdafx.h"
#include "Model.h"
#include "Defines.h"
#include "Globals.h"

bool Model::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, XMFLOAT3 position, bool bUseNormal, const char* szTexturePath)
{
	m_position = position;
	m_bUseNormals = bUseNormal;

	if (!InitializeBuffers(pDevice))
		RETURN_AND_LOG(false);

	if (!LoadTexture(pDevice, pDeviceContext, szTexturePath))
		RETURN_AND_LOG(false);


	return true;
}

bool Model::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, XMFLOAT3 position, bool bUseNormal, CTexture* pTexture /*= nullptr*/)
{
	m_position = position;
	m_bUseNormals = bUseNormal;

	if(!InitializeBuffers(pDevice))
		RETURN_AND_LOG(false);

	SetTexture(pTexture);

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
	auto arrVertices = CreateVertices(m_bUseNormals);
	
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

void* Model::CreateVertices(bool bUseNormals)
{
	using namespace Math3DNS;	//XMFLOAT3 operator +

	m_nVertexCount = 4;

	if(bUseNormals)
	{
		auto arrVertices = new VertexWithNormal[m_nVertexCount];
		arrVertices[0].position = m_position + XMFLOAT3(0.f, 0.f, 0.f);
		arrVertices[0].texture = XMFLOAT2(0.f, 0.f);
		arrVertices[0].normal = XMFLOAT3(0, 0, -1);

		arrVertices[1].position = m_position + XMFLOAT3(0.f, 1.f, 0.f);
		arrVertices[1].texture = XMFLOAT2(0.f, 1.f);
		arrVertices[1].normal = XMFLOAT3(1, 0, -1);

		arrVertices[2].position = m_position + XMFLOAT3(1.f, 1.f, 0.f);
		arrVertices[2].texture = XMFLOAT2(1.f, 1.f);
		arrVertices[2].normal = XMFLOAT3(1, 0, -1);

		arrVertices[3].position = m_position + XMFLOAT3(1.f, 0.f, 0.f);
		arrVertices[3].texture = XMFLOAT2(1.f, 0.f);
		arrVertices[3].normal = XMFLOAT3(1, 0, -1);

		return arrVertices;
	}
	else
	{
		auto arrVertices = new Vertex[m_nVertexCount];
		arrVertices[0].position = m_position + XMFLOAT3(0.f, 0.f, 0.f);
		arrVertices[0].texture = XMFLOAT2(0.f, 0.f);

		arrVertices[1].position = m_position + XMFLOAT3(0.f, 1.f, 0.f);
		arrVertices[1].texture = XMFLOAT2(0.f, 1.f);

		arrVertices[2].position = m_position + XMFLOAT3(1.f, 1.f, 0.f);
		arrVertices[2].texture = XMFLOAT2(1.f, 1.f);

		arrVertices[3].position = m_position + XMFLOAT3(1.f, 0.f, 0.f);
		arrVertices[3].texture = XMFLOAT2(1.f, 0.f);

		return arrVertices;
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
	return m_bUseNormals ? sizeof(VertexWithNormal) : sizeof(Vertex);
}
