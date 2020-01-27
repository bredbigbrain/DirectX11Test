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

	if(!CalculateNormals())
		RETURN_AND_LOG(false);

	if(!LoadColorMap())
		RETURN_AND_LOG(false);

	if(!BuildTerrainModel())
		RETURN_AND_LOG(false);

	DELETE_ARR(m_arrHeightMap);

	CalculateTerrainVectors();

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
	DELETE_ARR(m_lpszColorMapFileName);
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
		arrVerticies[i].position = XMFLOAT3(m_arrTerrainModel[i].x, m_arrTerrainModel[i].y, m_arrTerrainModel[i].z);
		arrVerticies[i].texCoord = XMFLOAT2(m_arrTerrainModel[i].tu, m_arrTerrainModel[i].tv);
		arrVerticies[i].normal = XMFLOAT3(m_arrTerrainModel[i].nx, m_arrTerrainModel[i].ny, m_arrTerrainModel[i].nz);
		arrVerticies[i].tangent = XMFLOAT3(m_arrTerrainModel[i].tx, m_arrTerrainModel[i].ty, m_arrTerrainModel[i].tz);
		arrVerticies[i].binormal = XMFLOAT3(m_arrTerrainModel[i].bx, m_arrTerrainModel[i].by, m_arrTerrainModel[i].bz);
		arrVerticies[i].color = XMFLOAT3(m_arrTerrainModel[i].r, m_arrTerrainModel[i].g, m_arrTerrainModel[i].b);
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
	m_lpszColorMapFileName = new char[MAX_PATH];

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
		if(char* szSubStr = strstr(szLine, "Color Map Filename:"))
		{
			const size_t nSubStrLength = sizeof("Color Map Filename:");
			strncpy_s(m_lpszColorMapFileName, MAX_PATH, szSubStr + nSubStrLength, strnlen_s(szSubStr + nSubStrLength, sizeof(szLine) - nSubStrLength));
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

	return nCounter == 5;
}

bool CTerrain::LoadBitmapHeightMap()
{
	m_arrHeightMap = new SHeightMap[m_nTerrainWidth * m_nTerrainHeight];

	FILE* pFile = nullptr;
	if(fopen_s(&pFile, m_lpszTerrainFileName, "rb") != 0 || !pFile)
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

	size_t nImageSize = m_nTerrainHeight * ((size_t)m_nTerrainWidth * 3 + 1);
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
			nIndex = m_nTerrainWidth * ((size_t)m_nTerrainHeight - 1 - j) + i;
			m_arrHeightMap[nIndex].y = static_cast<float>(arrBitMap[k]);
			k += 3;
		}
		++k;
	}

	delete[] arrBitMap;

	return true;
}

bool CTerrain::LoadColorMap()
{
	FILE* pFile = nullptr;
	if(fopen_s(&pFile, m_lpszColorMapFileName, "rb") != 0 || !pFile)
		RETURN_AND_LOG(false);

	BITMAPFILEHEADER bmpHeader;
	if(fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, pFile) != 1)
		RETURN_AND_LOG(false);

	BITMAPINFOHEADER bmpInfo;
	if(fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, pFile) != 1)
		RETURN_AND_LOG(false);

	if(bmpInfo.biHeight != m_nTerrainHeight || bmpInfo.biWidth != m_nTerrainWidth)
	{
		QUIK_LOG_M("Wrong .bmp image dimentions!");
		return false;
	}

	size_t nImageSize = m_nTerrainHeight * ((size_t)m_nTerrainWidth * 3 + 1);
	auto arrBitMap = new unsigned char[nImageSize];

	fseek(pFile, bmpHeader.bfOffBits, SEEK_SET);

	if(fread(arrBitMap, nImageSize, 1, pFile) != 1)
		RETURN_AND_LOG(false);

	if(fclose(pFile) != 0)
		RETURN_AND_LOG(false);

	size_t k = 0, nIndex = 0;
	for(size_t j = 0; j < m_nTerrainHeight; ++j)
	{
		for(size_t i = 0; i < m_nTerrainWidth; ++i)
		{
			nIndex = m_nTerrainWidth * ((size_t)m_nTerrainHeight - 1 - j) + i;
			m_arrHeightMap[nIndex].b = static_cast<float>(arrBitMap[k]) / 255.f;
			m_arrHeightMap[nIndex].g = static_cast<float>(arrBitMap[k + 1]) / 255.f;
			m_arrHeightMap[nIndex].r = static_cast<float>(arrBitMap[k + 2]) / 255.f;
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
	m_arrTerrainModel = new SModel[m_nVertexCount];

	size_t nIndex = 0, nIndex1 = 0, nIndex2 = 0, nIndex3 = 0, nIndex4 = 0;
	const auto funcProcessVertices = [this, &nIndex](size_t nIndex1, float tu, float tv)
	{
		m_arrTerrainModel[nIndex].x = m_arrHeightMap[nIndex1].x;
		m_arrTerrainModel[nIndex].y = m_arrHeightMap[nIndex1].y;
		m_arrTerrainModel[nIndex].z = m_arrHeightMap[nIndex1].z;
		m_arrTerrainModel[nIndex].tu = tu;
		m_arrTerrainModel[nIndex].tv = tv;
		m_arrTerrainModel[nIndex].nx = m_arrHeightMap[nIndex1].nx;
		m_arrTerrainModel[nIndex].ny = m_arrHeightMap[nIndex1].ny;
		m_arrTerrainModel[nIndex].nz = m_arrHeightMap[nIndex1].nz;
		m_arrTerrainModel[nIndex].r = m_arrHeightMap[nIndex1].r;
		m_arrTerrainModel[nIndex].g = m_arrHeightMap[nIndex1].g;
		m_arrTerrainModel[nIndex].b = m_arrHeightMap[nIndex1].b;
		++nIndex;
	};

	for(size_t j = 0; j < m_nTerrainHeight - 1; ++j)
	{
		for(size_t i = 0; i < m_nTerrainWidth - 1; ++i)
		{
			nIndex1 = m_nTerrainWidth * j + i;	//Upper left
			nIndex2 = m_nTerrainWidth * j + i + 1;	//Upper right
			nIndex3 = m_nTerrainWidth * (j + 1) + i;	//Bottom left
			nIndex4 = m_nTerrainWidth * (j + 1) + i + 1;	//Bottom right

			funcProcessVertices(nIndex1, 0.f, 0.f); // Triangle 1 - Upper left.
			funcProcessVertices(nIndex2, 1.f, 0.f); // Triangle 1 - Upper right.
			funcProcessVertices(nIndex3, 0.f, 1.f); // Triangle 1 - Bottom left.
			funcProcessVertices(nIndex3, 0.f, 1.f); // Triangle 2 - Bottom left.			
			funcProcessVertices(nIndex2, 1.f, 0.f); // Triangle 2 - Upper right.
			funcProcessVertices(nIndex4, 1.f, 1.f); // Triangle 2 - Bottom right.
		}
	}

	return true;
}

bool CTerrain::CalculateNormals()
{
	int i, j, index1, index2, index3, index;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	// Create a temporary array to hold the face normal vectors.
	XMFLOAT3* normals = new XMFLOAT3[((size_t)m_nTerrainHeight - 1) * (m_nTerrainWidth - 1)];

	// Go through all the faces in the mesh and calculate their normals.
	for(j = 0; j < (m_nTerrainHeight - 1); j++)
	{
		for(i = 0; i < (m_nTerrainWidth - 1); i++)
		{
			index1 = ((j + 1) * m_nTerrainWidth) + i;      // Bottom left vertex.
			index2 = ((j + 1) * m_nTerrainWidth) + (i + 1);  // Bottom right vertex.
			index3 = (j * m_nTerrainWidth) + i;          // Upper left vertex.

			// Get three vertices from the face.
			vertex1[0] = m_arrHeightMap[index1].x;
			vertex1[1] = m_arrHeightMap[index1].y;
			vertex1[2] = m_arrHeightMap[index1].z;

			vertex2[0] = m_arrHeightMap[index2].x;
			vertex2[1] = m_arrHeightMap[index2].y;
			vertex2[2] = m_arrHeightMap[index2].z;

			vertex3[0] = m_arrHeightMap[index3].x;
			vertex3[1] = m_arrHeightMap[index3].y;
			vertex3[2] = m_arrHeightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_nTerrainWidth - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// Calculate the length.
			length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) +
				(normals[index].z * normals[index].z));

			// Normalize the final value for this face using the length.
			normals[index].x = (normals[index].x / length);
			normals[index].y = (normals[index].y / length);
			normals[index].z = (normals[index].z / length);
		}
	}

	// Now go through all the vertices and take a sum of the face normals that touch this vertex.
	for(j = 0; j < m_nTerrainHeight; j++)
	{
		for(i = 0; i < m_nTerrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if(((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_nTerrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Bottom right face.
			if((i < (m_nTerrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_nTerrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper left face.
			if(((i - 1) >= 0) && (j < (m_nTerrainHeight - 1)))
			{
				index = (j * (m_nTerrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper right face.
			if((i < (m_nTerrainWidth - 1)) && (j < (m_nTerrainHeight - 1)))
			{
				index = (j * (m_nTerrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Calculate the length of this normal.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_nTerrainWidth) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_arrHeightMap[index].nx = (sum[0] / length);
			m_arrHeightMap[index].ny = (sum[1] / length);
			m_arrHeightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

void CTerrain::CalculateTerrainVectors()
{
	STempVertex vertex1, vertex2, vertex3;
	SVector tangent, binormal;

	size_t nIndex{0};
	size_t nFacesCount = m_nVertexCount / 3;
	for(size_t i = 0; i < nFacesCount; ++i)
	{
		vertex1.Copy(m_arrTerrainModel[nIndex++]);
		vertex2.Copy(m_arrTerrainModel[nIndex++]);
		vertex3.Copy(m_arrTerrainModel[nIndex++]);

		CalculateTangetnBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		m_arrTerrainModel[nIndex - 1].SetTangent(tangent);
		m_arrTerrainModel[nIndex - 2].SetTangent(tangent);
		m_arrTerrainModel[nIndex - 3].SetTangent(tangent);
		
		m_arrTerrainModel[nIndex - 1].SetBinormal(binormal);
		m_arrTerrainModel[nIndex - 2].SetBinormal(binormal);
		m_arrTerrainModel[nIndex - 3].SetBinormal(binormal);
	}
}

void CTerrain::CalculateTangetnBinormal(const STempVertex& vertex1, const STempVertex& vertex2, const STempVertex& vertex3
	, SVector& tangent, SVector& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of the tangent.
	length = (float)sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the tangent and then store it.
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of the binormal.
	length = (float)sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the binormal and then store it.
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;
}

