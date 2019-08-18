#include "Terrain.h"
#include "Debug.h"
#include "Defines.h"
#include <fstream>
#include <algorithm>

bool CTerrain::Initialize(ID3D11Device* pDevice, const char* lpszSetupFilePath)
{
	if(!LoadSetupFile(lpszSetupFilePath))
		RETURN_AND_LOG(false);

	if(!LoadBitmapHeightMap())
		RETURN_AND_LOG(false);

	SetTerrainCoordinates();

	if(!BuildTerrainModel())
		RETURN_AND_LOG(false);

	DELETE_ARR(m_arrHeightMap);

	if(!InitializeBuffers(pDevice))
		RETURN_AND_LOG(false);

	DELETE_ARR(m_arrTerrainModel);
	return true;
}

void CTerrain::Shutdown()
{
	RELEASE_AND_NULL(m_pVertexBuffer);
	RELEASE_AND_NULL(m_pIndexBuffer);
	DELETE_ARR(m_arrTerrainModel);
	DELETE_ARR(m_arrHeightMap);
	DELETE_ARR(m_lpszTerrainFileName);
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

	m_nIndexCount = m_nVertexCount = (nTerrainWidth - 1) * (nTerrainHeight - 1) * 6;

	SVertex* arrVerticies = new SVertex[m_nVertexCount];
	unsigned long* arrIndicies = new unsigned long[m_nIndexCount];

	for(size_t i = 0; i < m_nVertexCount; i++)
	{
		arrVerticies[i].position = m_arrTerrainModel[i];
		arrVerticies[i].color = color;
		arrIndicies[i] = i;
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

	ON_FAIL_LOG_AND_RETURN(pDevice->CreateBuffer(&vertexBufferDesc, &vertexSubData, &m_pVertexBuffer));

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_nIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexSubData;
	indexSubData.SysMemPitch = 0;
	indexSubData.SysMemSlicePitch = 0;
	indexSubData.pSysMem = arrIndicies;

	ON_FAIL_LOG_AND_RETURN(pDevice->CreateBuffer(&indexBufferDesc, &indexSubData, &m_pIndexBuffer));

	delete[] arrVerticies;
	delete[] arrIndicies;

	return true;
}

void CTerrain::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
{
	unsigned int nStride = sizeof(SVertex);
	unsigned int nOffset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &nStride, &nOffset);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool CTerrain::LoadSetupFile(const char * lpszFilePath)
{
	m_lpszTerrainFileName = new char[MAX_PATH];

	std::ifstream fin;
	fin.open(lpszFilePath);
	if(fin.fail())
		RETURN_AND_LOG(false);

	int nCounter = 0;
	char szLine[1024];
	fin.getline(szLine, sizeof(szLine));
	while(!fin.fail())
	{
		if(char* szSubStr = strstr(szLine, "Terrain Filename:"))
		{
			const size_t nSubStrLength = sizeof("Terrain Filename:");
			strncpy_s(m_lpszTerrainFileName, MAX_PATH, szSubStr + nSubStrLength, strnlen_s(szSubStr + nSubStrLength, sizeof(szLine) - nSubStrLength));
			++nCounter;
		}
		else if(char* szSubStr = strstr(szLine, "Terrain Height:"))
		{
			const size_t nSubStrLength = sizeof("Terrain Height:");
			m_nTerrainHeight = atoi(szSubStr + nSubStrLength);
			++nCounter;
		}
		else if(char* szSubStr = strstr(szLine, "Terrain Width:"))
		{
			const size_t nSubStrLength = sizeof("Terrain Width:");
			m_nTerrainWidth = atoi(szSubStr + nSubStrLength);
			++nCounter;
		}
		else if(char* szSubStr = strstr(szLine, "Terrain Scaling:"))
		{
			const size_t nSubStrLength = sizeof("Terrain Scaling:");
			m_fHeightScale = atof(szSubStr + nSubStrLength);
			++nCounter;
		}
		fin.getline(szLine, sizeof(szLine));
	}

	return nCounter == 4;
}

bool CTerrain::LoadBitmapHeightMap()
{
	m_arrHeightMap = new XMFLOAT3[m_nTerrainWidth * m_nTerrainHeight];

	FILE* pFile;
	if(fopen_s(&pFile, m_lpszTerrainFileName, "rb") != 0)
		RETURN_AND_LOG(false);

	BITMAPFILEHEADER bmpHeader;
	if(fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, pFile) !=1)
		RETURN_AND_LOG(false);

	BITMAPINFOHEADER bmpInfo;
	if(fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, pFile) != 1)
		RETURN_AND_LOG(false);

	if(bmpInfo.biHeight != m_nTerrainHeight || bmpInfo.biWidth != m_nTerrainWidth)
	{
		QUIK_LOG_M("Wrong .bmp image dimentions!");
		return false;
	}

	size_t nImageSize = m_nTerrainHeight * (m_nTerrainWidth * 3 + 1);
	auto arrBitMap = new unsigned char[nImageSize];

	fseek(pFile, bmpHeader.bfOffBits, SEEK_SET);

	if(fread(arrBitMap, nImageSize, 1, pFile) != 1)
		RETURN_AND_LOG(false);

	if(fclose(pFile) != 0)
		RETURN_AND_LOG(false);

	size_t k = 0, nIndex = 0;
	for(size_t j = 0; j < m_nTerrainHeight; j++)
	{
		for(size_t i = 0; i < m_nTerrainWidth; i++)
		{
			nIndex = m_nTerrainWidth * (m_nTerrainHeight - 1 - j) + i;
			m_arrHeightMap[nIndex].y = static_cast<float>(arrBitMap[k]);
			k += 3;
		}
		++k;
	}

	delete[] arrBitMap;

	return true;
}

void CTerrain::SetTerrainCoordinates()
{
	size_t nIndex = 0;
	for(size_t j = 0; j < m_nTerrainWidth; j++)
	{
		for(size_t i = 0; i < m_nTerrainHeight; i++)
		{
			nIndex = m_nTerrainWidth * j + i;
			m_arrHeightMap[nIndex].x = i;
			m_arrHeightMap[nIndex].z = -static_cast<float>(j) + m_nTerrainHeight + 1;

			m_arrHeightMap[nIndex].y *= m_fHeightScale;
		}
	}
}

bool CTerrain::BuildTerrainModel()
{
	m_nVertexCount = (m_nTerrainHeight - 1) * (m_nTerrainWidth - 1) * 6;
	m_arrTerrainModel = new XMFLOAT3[m_nVertexCount];

	size_t nIndex = 0, nIndex1 = 0, nIndex2 = 0, nIndex3 = 0, nIndex4 = 0;
	for(size_t j = 0; j < m_nTerrainHeight - 1; ++j)
	{
		for(size_t i = 0; i < m_nTerrainWidth - 1; ++i)
		{
			nIndex1 = m_nTerrainWidth * j + i;	//Upper left
			nIndex2 = m_nTerrainWidth * j + i + 1;	//Upper right
			nIndex3 = m_nTerrainWidth * (j + 1) + i;	//Bottom left
			nIndex4 = m_nTerrainWidth * (j + 1) + i + 1;	//Bottom right

			// Triangle 1 - Upper left.
			m_arrTerrainModel[nIndex].x = m_arrHeightMap[nIndex1].x;
			m_arrTerrainModel[nIndex].y = m_arrHeightMap[nIndex1].y;
			m_arrTerrainModel[nIndex].z = m_arrHeightMap[nIndex1].z;
			++nIndex;

			// Triangle 1 - Upper right.
			m_arrTerrainModel[nIndex].x = m_arrHeightMap[nIndex2].x;
			m_arrTerrainModel[nIndex].y = m_arrHeightMap[nIndex2].y;
			m_arrTerrainModel[nIndex].z = m_arrHeightMap[nIndex2].z;
			++nIndex;

			// Triangle 1 - Bottom left.
			m_arrTerrainModel[nIndex].x = m_arrHeightMap[nIndex3].x;
			m_arrTerrainModel[nIndex].y = m_arrHeightMap[nIndex3].y;
			m_arrTerrainModel[nIndex].z = m_arrHeightMap[nIndex3].z;
			++nIndex;

			// Triangle 2 - Bottom left.
			m_arrTerrainModel[nIndex].x = m_arrHeightMap[nIndex3].x;
			m_arrTerrainModel[nIndex].y = m_arrHeightMap[nIndex3].y;
			m_arrTerrainModel[nIndex].z = m_arrHeightMap[nIndex3].z;
			++nIndex;

			// Triangle 2 - Upper right.
			m_arrTerrainModel[nIndex].x = m_arrHeightMap[nIndex2].x;
			m_arrTerrainModel[nIndex].y = m_arrHeightMap[nIndex2].y;
			m_arrTerrainModel[nIndex].z = m_arrHeightMap[nIndex2].z;
			++nIndex;

			// Triangle 2 - Bottom right.
			m_arrTerrainModel[nIndex].x = m_arrHeightMap[nIndex4].x;
			m_arrTerrainModel[nIndex].y = m_arrHeightMap[nIndex4].y;
			m_arrTerrainModel[nIndex].z = m_arrHeightMap[nIndex4].z;
			++nIndex;
		}
	}

	return true;
}

