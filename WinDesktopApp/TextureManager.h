#pragma once

#include "stdafx.h"
#include "D3D.h"
#include "Texture.h"

class CTextureManager
{
public:
	const std::vector<std::string> m_vecTextures
	{
		"Res/test.tga",
		"Res/dirt01d.tga",
		"Res/whiteDiff.bmp",
		"Res/dirtDiff.bmp",
		"Res/dirtNorm.bmp",
		"Res/Arrow.bmp",
		"Res/Arrow.tga",
		"Res/GroundForestDiff.bmp",
		"Res/GroundForestNorm.bmp",
		"Res/GroundForestDiffTest.bmp",
	};

	enum class TextureIndex
	{
		UNDEFINED = -1, testTga, dirt01d, whiteDiffBmp, dirtDiffBmp, dirtNormBmp, ArrowBmp, ArrowTga, GroundForestDiff, GroundForestNorm
		, GroundForestDiffTest
	};

	bool Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	bool Initialize(size_t nTexturesCount);
	void Shutdown();

	bool LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* lpszTexturePath, size_t nIndex);
	ID3D11ShaderResourceView* GetTexture(size_t nIndex) const;
	ID3D11ShaderResourceView* GetTexture(TextureIndex eIndex) const;

private:
	CTexture* m_arrTextures{nullptr};
	size_t m_nTexturesCount{0};
};

