#pragma once

#include "D3D.h"
#include "Texture.h"

class CTextureManager
{
public:
	
	bool Initialize(size_t nTexturesCount);
	void Shutdown();

	bool LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* lpszTexturePath, size_t nIndex);
	ID3D11ShaderResourceView* GetTexture(int nIndex);

private:
	CTexture* m_arrTextures{nullptr};
	size_t m_nTexturesCount{0};
};

