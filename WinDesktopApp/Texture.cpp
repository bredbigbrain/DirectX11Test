#include "Texture.h"
#include <string>
#include "Debug.h"
#include "Defines.h"

CTexture::CTexture() {}


CTexture::~CTexture() {}


bool CTexture::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* szFileName)
{
	int nHeight{0};
	int nWidth{0};

	bool bSuccess{false};
	switch(DetermineTextureType(szFileName))
	{
	case TextureType::BMP: bSuccess = LoadBMP(szFileName, nHeight, nWidth); break;
	case TextureType::TGA: bSuccess = LoadTagra(szFileName, nHeight, nWidth); break;
	default: 
	{
		bSuccess = false;
		Debug::LogNow(1, "CTexture::Initialize: Unsupported texture type! (*%s)", szFileName);
		break;
	}
	}
	
	if(!bSuccess)
		RETURN_AND_LOG(false);

	D3D11_TEXTURE2D_DESC textureDesc;

	textureDesc.Height = nHeight;
	textureDesc.Width = nWidth;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	if(FAILED(pDevice->CreateTexture2D(&textureDesc, NULL, &m_pTexture)))
		RETURN_AND_LOG(false);

	unsigned int nRowPitch = nWidth * 4 * sizeof(unsigned char);

	pDeviceContext->UpdateSubresource(m_pTexture, 0, NULL, m_pRawData, nRowPitch, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	if (FAILED(pDevice->CreateShaderResourceView(m_pTexture, &srvDesc, &m_pTextureView)))
		RETURN_AND_LOG(false);

	pDeviceContext->GenerateMips(m_pTextureView);

	delete[] m_pRawData;
	m_pRawData = nullptr;

	return true;
}

void CTexture::Shutdown()
{
	RELEASE_AND_NULL(m_pTextureView);
	RELEASE_AND_NULL(m_pTexture);
	DELETE_ARR(m_pRawData);
}

CTexture::TextureType CTexture::DetermineTextureType(const char * szFileName)
{
	const char szBMP[5] = ".bmp";
	const char szTGA[5] = ".tga";

	size_t nStrLen = strnlen_s(szFileName, MAX_PATH);
	if(nStrLen > sizeof(szBMP) && lstrcmpA(szFileName + (nStrLen - sizeof(szBMP) + 1), szBMP) == 0)
		return TextureType::BMP;
	else if(nStrLen > sizeof(szTGA) && lstrcmpA(szFileName + (nStrLen - sizeof(szTGA) + 1), szTGA) == 0)
		return TextureType::TGA;
	else
		return TextureType::UNDEF;
}

ID3D11ShaderResourceView* CTexture::GetTexture()
{
	return m_pTextureView;
}

bool CTexture::LoadTagra(const char* pszFilePath, int& nHeight, int& nWidth)
{
	FILE* pFile;
	if (fopen_s(&pFile, pszFilePath, "rb") != 0)
		RETURN_AND_LOG(false);

	TagraHeader_t tagraFileHeader;
	unsigned int nCount = static_cast<unsigned int>(fread(&tagraFileHeader, sizeof(TagraHeader_t), 1, pFile));
	if (nCount != 1)
		RETURN_AND_LOG(false);

	nHeight = static_cast<int>(tagraFileHeader.height);
	nWidth = static_cast<int>(tagraFileHeader.width);
	int bpp = static_cast<int>(tagraFileHeader.bpp);

	if (bpp != 32)
		RETURN_AND_LOG(false);

	int nImageSize = nWidth * nHeight * 4;
	unsigned char* imageData = new unsigned char[nImageSize];

	nCount = fread(imageData, 1, nImageSize, pFile);

	if (fclose(pFile) != 0 || nCount != nImageSize)
		RETURN_AND_LOG(false);

	if(m_pRawData)
	{
		Debug::LogNow(2, "CTexture::LoadTagra WARNIG: Overriding texture (%s)", pszFilePath);
		delete[] m_pRawData;
	}
	m_pRawData = new unsigned char[nImageSize];

	int index{0};
	int k{0};

	for(int i = 0; i < nWidth * nHeight; ++i)
	{
		m_pRawData[index + 0] = imageData[k + 2];	//r
		m_pRawData[index + 1] = imageData[k + 1];	//g
		m_pRawData[index + 2] = imageData[k + 0];	//b
		m_pRawData[index + 3] = imageData[k + 3];	//a

		k += 4;
		index += 4;
	}
	
	delete[] imageData;
	return true;
}

bool CTexture::LoadBMP(const char* pszFilePath, int& nHeight, int& nWidth)
{
	FILE* pFile;
	if(fopen_s(&pFile, pszFilePath, "rb") != 0)
		RETURN_AND_LOG(false);

	BITMAPFILEHEADER bmpHeader;
	if(fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, pFile) != 1)
		RETURN_AND_LOG(false);

	BITMAPINFOHEADER bmpInfo;
	if(fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, pFile) != 1)
		RETURN_AND_LOG(false);

	nHeight = bmpInfo.biHeight;
	nWidth = bmpInfo.biWidth;

	size_t nImageSize = nHeight * (nWidth * 3 + ((nWidth % 2) == 0 ? 0 : 1));
	unsigned char* arrBitMap = new unsigned char[nImageSize];

	fseek(pFile, bmpHeader.bfOffBits, SEEK_SET);

	if(fread(arrBitMap, nImageSize, 1, pFile) != 1)
		RETURN_AND_LOG(false);

	if(fclose(pFile) != 0)
		RETURN_AND_LOG(false);

	if(m_pRawData)
	{
		Debug::LogNow(2, "CTexture::LoadBMP WARNIG: Overriding texture (%s)", pszFilePath);
		delete[] m_pRawData;
	}
	m_pRawData = new unsigned char[nWidth * nHeight * 4];

	size_t k = 0, nIndex = 0;
	for(size_t j = 0; j < nHeight; ++j)
	{
		for(size_t i = 0; i < nWidth; ++i)
		{
			m_pRawData[nIndex + 0] = arrBitMap[k + 2];	//r
			m_pRawData[nIndex + 1] = arrBitMap[k + 1];	//g
			m_pRawData[nIndex + 2] = arrBitMap[k];		//b
			m_pRawData[nIndex + 3] = 255;				//a
			k += 3;
			nIndex += 4;
		}
		++k;
	}
	
	delete[] arrBitMap;
	return true;
}
