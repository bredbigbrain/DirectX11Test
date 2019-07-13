#include "Terrain.h"
#include"Debug.h"


CTerrain::CTerrain()
{
}


CTerrain::~CTerrain()
{
}


bool CTerrain::Initialize(ID3D11Device* pDevice)
{
	if(!InitializeBuffers(pDevice))
		RETURN_AND_LOG(false);
	return true;
}

void CTerrain::Shutdown()
{
	ShutdownBuffers();
}


bool CTerrain::Render(ID3D11DeviceContext* pDeviceContext)
{
	RenderBuffers(pDeviceContext);

	return true;
}

size_t CTerrain::GetIndexCount()
{
	return m_nIndexCount;
}


bool CTerrain::InitializeBuffers(ID3D11Device* pDevice)
{
	int nTerrainHeight = 256;
	int nTerrainWidth = 256;

	XMFLOAT4 color(1.f, 1.f, 1.f, 1.f);

	m_nIndexCount = m_nVertexCount = (nTerrainWidth - 1) * (nTerrainHeight - 1) * 8;

	SVertex* arrVerticies = new SVertex[m_nVertexCount];
	unsigned long* arrIndicies = new unsigned long[m_nIndexCount];

	size_t nIndex = 0;
	float fPosX = 0, fPosZ = 0;
	for(size_t j = 0; j < (nTerrainHeight - 1); ++j)
	{
		for(size_t i = 0; i < (nTerrainWidth- 1); i++)
		{
			// Line 1 - Upper left.
			fPosX = (float)i;
			fPosZ = (float)(j + 1);

			arrVerticies[nIndex].position = XMFLOAT3(fPosX, 0.0f, fPosZ);
			arrVerticies[nIndex].color = color;
			arrIndicies[nIndex] = nIndex;
			nIndex++;

			// Line 1 - Upper right.
			fPosX = (float)(i + 1);
			fPosZ = (float)(j + 1);

			arrVerticies[nIndex].position = XMFLOAT3(fPosX, 0.0f, fPosZ);
			arrVerticies[nIndex].color = color;
			arrIndicies[nIndex] = nIndex;
			nIndex++;

			// Line 2 - Upper right
			fPosX = (float)(i + 1);
			fPosZ = (float)(j + 1);

			arrVerticies[nIndex].position = XMFLOAT3(fPosX, 0.0f, fPosZ);
			arrVerticies[nIndex].color = color;
			arrIndicies[nIndex] = nIndex;
			nIndex++;

			// Line 2 - Bottom right.
			fPosX = (float)(i + 1);
			fPosZ = (float)j;

			arrVerticies[nIndex].position = XMFLOAT3(fPosX, 0.0f, fPosZ);
			arrVerticies[nIndex].color = color;
			arrIndicies[nIndex] = nIndex;
			nIndex++;

			// Line 3 - Bottom right.
			fPosX = (float)(i + 1);
			fPosZ = (float)j;

			arrVerticies[nIndex].position = XMFLOAT3(fPosX, 0.0f, fPosZ);
			arrVerticies[nIndex].color = color;
			arrIndicies[nIndex] = nIndex;
			nIndex++;

			// Line 3 - Bottom left.
			fPosX = (float)i;
			fPosZ = (float)j;

			arrVerticies[nIndex].position = XMFLOAT3(fPosX, 0.0f, fPosZ);
			arrVerticies[nIndex].color = color;
			arrIndicies[nIndex] = nIndex;
			nIndex++;

			// Line 4 - Bottom left.
			fPosX = (float)i;
			fPosZ = (float)j;

			arrVerticies[nIndex].position = XMFLOAT3(fPosX, 0.0f, fPosZ);
			arrVerticies[nIndex].color = color;
			arrIndicies[nIndex] = nIndex;
			nIndex++;

			// Line 4 - Upper left.
			fPosX = (float)i;
			fPosZ = (float)(j + 1);

			arrVerticies[nIndex].position = XMFLOAT3(fPosX, 0.0f, fPosZ);
			arrVerticies[nIndex].color = color;
			arrIndicies[nIndex] = nIndex;
			nIndex++;
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SVertex) * m_nVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA vertexSubData;
	vertexSubData.pSysMem = arrVerticies;
	vertexSubData.SysMemPitch = 0;
	vertexSubData.SysMemSlicePitch = 0;

	if(FAILED(pDevice->CreateBuffer(&vertexBufferDesc, &vertexSubData, &m_pVertexBuffer)))
		RETURN_AND_LOG(false);

	D3D11_BUFFER_DESC indexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(unsigned int) * m_nIndexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexSubData;
	vertexSubData.pSysMem = arrIndicies;
	vertexSubData.SysMemPitch = 0;
	vertexSubData.SysMemSlicePitch = 0;

	if(FAILED(pDevice->CreateBuffer(&indexBufferDesc, &indexSubData, &m_pIndexBuffer)))
		RETURN_AND_LOG(false);

	delete[] arrVerticies;
	delete[] arrIndicies;

	return true;
}

void CTerrain::ShutdownBuffers()
{
	if(m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}
	if(m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = nullptr;
	}
}

void CTerrain::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
{
	unsigned int nStride = sizeof(SVertex);
	unsigned int nOffset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &nStride, &nOffset);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}