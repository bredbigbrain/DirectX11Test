#include"Texture.h"
#include<string>
#include"Debug.h"

CTexture::CTexture() {}


CTexture::~CTexture() {}


bool CTexture::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* szFileName)
{
	int nHeight{0};
	int nWidth{0};

	if (!LoadTagra(szFileName, nHeight, nWidth))
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

	pDeviceContext->UpdateSubresource(m_pTexture, 0, NULL, m_pRawTagraData, nRowPitch, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	if (FAILED(pDevice->CreateShaderResourceView(m_pTexture, &srvDesc, &m_pTextureView)))
		RETURN_AND_LOG(false);

	pDeviceContext->GenerateMips(m_pTextureView);

	delete[] m_pRawTagraData;
	m_pRawTagraData = nullptr;

	return true;
}

void CTexture::Shutdown()
{
	if (m_pTextureView)
	{
		m_pTextureView->Release();
		m_pTextureView = nullptr;
	}
	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = nullptr;
	}
	if (m_pRawTagraData)
	{
		delete[] m_pRawTagraData;
		m_pRawTagraData = nullptr;
	}
}

ID3D11ShaderResourceView* CTexture::GetTexture()
{
	return m_pTextureView;
}

bool CTexture::LoadTagra(const char* szFilePath, int& nHeight, int& nWidth)
{
	FILE* pFile;
	if (fopen_s(&pFile, szFilePath, "rb") != 0)
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

	m_pRawTagraData = new unsigned char[nImageSize];

	int index{0};
	int k = nImageSize - (nWidth * 4);

	for (int j = 0; j < nHeight; j++)
	{
		for (int i = 0; i < nWidth; i++)
		{
			m_pRawTagraData[index + 0] = imageData[k + 2];	//r
			m_pRawTagraData[index + 1] = imageData[k + 1];	//g
			m_pRawTagraData[index + 2] = imageData[k + 0];	//b
			m_pRawTagraData[index + 3] = imageData[k + 3];	//a

			k += 4;
			index += 4;
		}
		k -= nWidth * 8;
	}
	
	delete[] imageData;
	return true;
}