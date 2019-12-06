#pragma once

#include "D3D.h"
#include "TextureShader.h"

class CTerrainShader : public CTextureShader
{
	struct SLightBuffer
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float fPadding;
	};

public:
	virtual bool Initialize(ID3D11Device* pDevice, HWND hwnd);
	bool Render(ID3D11DeviceContext* pDeviceContext, int nIndexCount, XMMATRIX matrWorld
		, XMMATRIX matrView, XMMATRIX matrProjection, ID3D11ShaderResourceView* pTexture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

protected:
	virtual bool InitializeShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* szPathVS, const WCHAR* szPathPS) override;
	bool SetShaderParameters(ID3D11DeviceContext* pDeviceContext, XMMATRIX matrWorld
		, XMMATRIX matrView, XMMATRIX matrProjection, ID3D11ShaderResourceView* pTexture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

protected:
	ID3D11Buffer* m_pLightBuffer{nullptr};
};

