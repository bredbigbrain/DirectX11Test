#include "Text.h"
#include "Debug.h"
#include "Globals.h"
#include "HelperFunctions.h"

CText::CText()
{
}


CText::~CText()
{
}

bool CText::Initialize(ID3D11Device* pDvice, ID3D11DeviceContext* pDeviceContext, int nScreenWidth, int nScreenHeight, int nMaxLength,
	bool bEnableShadow, CFont* pFont, const char* lpszText, int nPositionX, int nPositionY, XMFLOAT4 color)
{
	m_nScreenWidth = nScreenWidth;
	m_nScreenHeight = nScreenHeight;
	m_nMaxLength = nMaxLength;
	m_bEnableShadow = bEnableShadow;

	if(!InitilalizeSentence(pDvice, pDeviceContext, pFont, lpszText, nPositionX, nPositionY, color))
		RETURN_AND_LOG(false);

	return true;
}

void CText::Shutdown()
{
	SafeReleaseAndNull(m_pVertexBuffer);
	SafeReleaseAndNull(m_pIndexBuffer);
	SafeReleaseAndNull(m_pVertexBuffer2);
	SafeReleaseAndNull(m_pIndexBuffer2);
}

void CText::Render(ID3D11DeviceContext * pDeviceContext, CShaderManager * pShManager, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrOrtho, ID3D11ShaderResourceView * pFontTexture)
{
	RenderSentence(pDeviceContext, pShManager, matrWorld, matrView, matrOrtho, pFontTexture);
}

bool CText::UpdateSentence(ID3D11DeviceContext * pDeviceContext, CFont * pFont, const char * lpszText)
{
	return UpdateSentence(pDeviceContext, pFont, lpszText, m_lastPosition.x, m_lastPosition.y, m_pixelColor);
}

bool CText::UpdateSentence(ID3D11DeviceContext * pDeviceContext, CFont * pFont, const char * lpszText, int nPositionX, int nPositionY, XMFLOAT4 color)
{
	m_pixelColor = color;

	m_lastPosition.x = nPositionX;
	m_lastPosition.y = nPositionY;

	size_t nNumLetters = strnlen_s(lpszText, Debug::MAX_LOG_LENGTH);
	if(nNumLetters > m_nMaxLength)
		RETURN_AND_LOG(false);

	m_sText = lpszText;

	SVertex* arrVerticies = new SVertex[m_nVertexCount];

	const size_t arrVertSize = sizeof(SVertex) * m_nVertexCount;

	memset(arrVerticies, 0, arrVertSize);

	float fDrawX = (m_nScreenWidth / 2. * -1.) + nPositionX;
	float fDrawY = m_nScreenHeight / 2. + nPositionY;

	pFont->BuildVertexArray(arrVerticies, lpszText, fDrawX, fDrawY);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if(FAILED(pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		RETURN_AND_LOG(false);
	
	memcpy(mappedResource.pData, arrVerticies, arrVertSize);

	pDeviceContext->Unmap(m_pVertexBuffer, 0);

	if(m_bEnableShadow)
	{
		memset(arrVerticies, 0, arrVertSize);

		fDrawX += 2;
		fDrawY -= 2;

		pFont->BuildVertexArray(arrVerticies, lpszText, fDrawX, fDrawY);

		if(FAILED(pDeviceContext->Map(m_pVertexBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
			RETURN_AND_LOG(false);

		memcpy(mappedResource.pData, arrVerticies, arrVertSize);

		pDeviceContext->Unmap(m_pVertexBuffer2, 0);
	}

	delete[] arrVerticies;

	return true;											   
}

const char* CText::GetText()
{
	return m_sText.c_str();
}

bool CText::InitilalizeSentence(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, CFont * pFont, const char * lpszText, int nPositionX, int nPositionY, XMFLOAT4 color)
{
	m_nVertexCount = m_nIndexCount = 6 * m_nMaxLength;

	SVertex* arrVerticies = new SVertex[m_nVertexCount];
	const size_t nArrVertSize = sizeof(SVertex) * m_nVertexCount;
	memset(arrVerticies, 0, nArrVertSize);

	unsigned long* arrIndicies = new unsigned long[m_nIndexCount];
	for(size_t i = 0; i < m_nIndexCount; i++)
		arrIndicies[i] = i;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = nArrVertSize;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = arrVerticies;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ON_FAIL_LOG_AND_RETURN(pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer));

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
	indexData.pSysMem = arrIndicies;

	ON_FAIL_LOG_AND_RETURN(pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer));

	if(m_bEnableShadow)
	{
		ON_FAIL_LOG_AND_RETURN(pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer2));
		ON_FAIL_LOG_AND_RETURN(pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer2));
	}

	delete[] arrVerticies;
	delete[] arrIndicies;

	if(!UpdateSentence(pDeviceContext, pFont, lpszText, nPositionX, nPositionY, color))
		RETURN_AND_LOG(false);

	return true;
}

void CText::RenderSentence(ID3D11DeviceContext * pDeviceContext, CShaderManager * pShManager
	, XMMATRIX& matrWorld, XMMATRIX& matrView, XMMATRIX& matrOrtho, ID3D11ShaderResourceView * pFontTexture)
{
	unsigned int nStride = sizeof(SVertex);
	unsigned int nOffset = 0;

	if(m_bEnableShadow)
	{
		XMFLOAT4 shadowColor(0.f, 0.f, 0.f, 1.f);
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer2, &nStride, &nOffset);
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer2, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pShManager->RenderFontShader(pDeviceContext, m_nIndexCount, matrWorld, matrView, matrOrtho, pFontTexture, shadowColor);
	}

	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &nStride, &nOffset);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pShManager->RenderFontShader(pDeviceContext, m_nIndexCount, matrWorld, matrView, matrOrtho, pFontTexture, m_pixelColor);
}
															   