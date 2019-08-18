#pragma once

#include"D3D.h"
#include"Texture.h"


class CFont
{
private:
	struct SFont
	{
		float m_fLeft{0};
		float m_fRight{0};
		int m_nSize{0};
	};

	struct SVertex
	{
		XMFLOAT3 m_position; 
		XMFLOAT2 m_textureCoord;
	};

public:
	CFont();
	~CFont();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* lpszFontFileName, const char* lpszTextureFileName
		, float fFontSize, int nSpaseSize);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void* arrVerticies, const char* lpszSentence, float fDrawX, float fDrawY) const;
	int GetSentencePixelLenght(char* lpszSentence);
	int GetFontHeight();

private:
	bool LoadFontData(const char* lpszFileName);
	bool LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* lpszFileName);

private:
	SFont* m_pFont{nullptr};
	CTexture* m_pTexture{nullptr};
	float m_fFontHeight{0};
	int m_nSpaceSize{0};
};

