#include "Font.h"
#include "Debug.h"
#include "Globals.h"
#include "Defines.h"
#include <fstream>

CFont::CFont()
{
}


CFont::~CFont()
{
}

bool CFont::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const char * lpszFontFileName, const char * lpszTextureFileName, float fFontSize, int nSpaseSize)
{
	m_fFontHeight = fFontSize;
	m_nSpaceSize = nSpaseSize;

	if(!LoadFontData(lpszFontFileName))
		RETURN_AND_LOG(false);

	if(!LoadTexture(pDevice, pDeviceContext, lpszTextureFileName))
		RETURN_AND_LOG(false);

	return true;
}

void CFont::Shutdown()
{
	SHUTDOWN_DELETE(m_pTexture);

	DELETE_ARR(m_pFont);
}

ID3D11ShaderResourceView * CFont::GetTexture()
{
	return m_pTexture->GetTexture();
}

void CFont::BuildVertexArray(void * arrVerticies, const char* lpszSentence, float fDrawX, float fDrawY) const
{
	SVertex* arrVerticies_ = reinterpret_cast<SVertex*>(arrVerticies);
	size_t nNumLetters = strnlen_s(lpszSentence, Debug::MAX_LOG_LENGTH);

	int nIndex{0};
	int nLetter{0};
	for(size_t i = 0; i < nNumLetters; ++i)
	{
		nLetter = static_cast<int>(lpszSentence[i]) - 32;

		if(nLetter == 0)
			fDrawX += static_cast<float>(m_nSpaceSize);
		else
		{
			// First triangle in quad.
			arrVerticies_[nIndex].m_position = XMFLOAT3(fDrawX, fDrawY, Settings::g_graphics.fScreenNear);  // Top left.
			arrVerticies_[nIndex].m_textureCoord = XMFLOAT2(m_pFont[nLetter].m_fLeft, 1.f);
			++nIndex;

			arrVerticies_[nIndex].m_position = XMFLOAT3((fDrawX + m_pFont[nLetter].m_nSize), (fDrawY - m_fFontHeight), Settings::g_graphics.fScreenNear);  // Bottom m_fRight.
			arrVerticies_[nIndex].m_textureCoord = XMFLOAT2(m_pFont[nLetter].m_fRight, 0.0f);
			++nIndex;

			arrVerticies_[nIndex].m_position = XMFLOAT3(fDrawX, (fDrawY - m_fFontHeight), Settings::g_graphics.fScreenNear);  // Bottom m_fLeft.
			arrVerticies_[nIndex].m_textureCoord = XMFLOAT2(m_pFont[nLetter].m_fLeft, 0.0f);
			++nIndex;

			// Second triangle in quad.
			arrVerticies_[nIndex].m_position = XMFLOAT3(fDrawX, fDrawY, Settings::g_graphics.fScreenNear);  // Top m_fLeft.
			arrVerticies_[nIndex].m_textureCoord = XMFLOAT2(m_pFont[nLetter].m_fLeft, 1.0f);
			++nIndex;

			arrVerticies_[nIndex].m_position = XMFLOAT3(fDrawX + m_pFont[nLetter].m_nSize, fDrawY, Settings::g_graphics.fScreenNear);  // Top m_fRight.
			arrVerticies_[nIndex].m_textureCoord = XMFLOAT2(m_pFont[nLetter].m_fRight, 1.0f);
			++nIndex;

			arrVerticies_[nIndex].m_position = XMFLOAT3((fDrawX + m_pFont[nLetter].m_nSize), (fDrawY - m_fFontHeight), Settings::g_graphics.fScreenNear);  // Bottom m_fRight.
			arrVerticies_[nIndex].m_textureCoord = XMFLOAT2(m_pFont[nLetter].m_fRight, 0.0f);
			++nIndex;

			fDrawX += m_pFont[nLetter].m_nSize + 1.f;
		}
	}
}

int CFont::GetSentencePixelLenght(char * lpszSentence)
{
	int nPixelLength{0};
	int nLetter{0};
	size_t nNumLetters = strnlen_s(lpszSentence, Debug::MAX_LOG_LENGTH);

	for(size_t i = 0; i < nNumLetters; ++i)
	{
		nLetter = static_cast<int>(lpszSentence[i]) - 32;

		nPixelLength += nLetter == 0 ? m_nSpaceSize : (m_pFont[nLetter].m_nSize + 1);
	}

	return nPixelLength;
}

int CFont::GetFontHeight()
{
	return static_cast<int>(m_fFontHeight);
}

bool CFont::LoadFontData(const char * lpszFileName)
{
	const int nFontsSize = 95;

	m_pFont = new SFont[nFontsSize];

	std::ifstream fin;
	fin.open(lpszFileName);
	if(fin.fail())
		RETURN_AND_LOG(false);

	char cTemp;
	for(int i = 0; i < nFontsSize; ++i)
	{
		fin.get(cTemp);
		while(cTemp != ' ')
			fin.get(cTemp);
		fin.get(cTemp);
		while(cTemp != ' ')
			fin.get(cTemp);

		fin >> m_pFont[i].m_fLeft;
		fin >> m_pFont[i].m_fRight;
		fin >> m_pFont[i].m_nSize;
	}

	fin.close();

	return true;
}

bool CFont::LoadTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const char * lpszFileName)
{
	m_pTexture = new CTexture();

	if(!m_pTexture->Initialize(pDevice, pDeviceContext, lpszFileName))
		RETURN_AND_LOG(false);

	return true;
}
