#pragma once

#include<d3d11.h>
#include<stdio.h>

class CTexture
{
private:
	struct TagraHeader_t
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	CTexture();
	~CTexture();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* szFileName);
	void Shutdown();

	enum class TextureType { UNDEF, BMP, TGA, };
	static TextureType DetermineTextureType(const char* szFileName);

	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadTagra(const char* pszFileName, int& nHeight, int& nWidth);
	bool LoadBMP(const char* pszFileName, int& nHeight, int& nWidth);

private:
	unsigned char* m_pRawData{nullptr};
	ID3D11Texture2D* m_pTexture{nullptr};
	ID3D11ShaderResourceView* m_pTextureView{nullptr};
};

