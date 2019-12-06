#pragma once

#include "D3D.h"
#include "ColorShader.h"
#include "FontShader.h"
#include "TextureShader.h"
#include "LightShader.h"
#include "TerrainShader.h"

class CShaderManager
{
public:
	bool Initialize(ID3D11Device* pDevice, HWND hwnd);
	void Shutdown();

	bool RenderColorShader(ID3D11DeviceContext* pDeviceContext, int nIndexCount, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrProjection);
	bool RenderTextureShader(ID3D11DeviceContext* pDeviceContext, int nIndexCount, XMMATRIX& matrWorld, XMMATRIX& matrView, XMMATRIX& matrProjection
		, ID3D11ShaderResourceView* pTextureSRV);
	bool RenderFontShader(ID3D11DeviceContext* pDeviceContext, int nIndexCount, XMMATRIX& matrWorld, XMMATRIX& matrView, XMMATRIX& matrProjection
		, ID3D11ShaderResourceView* pTextureSRV, XMFLOAT4 color);
	bool RenderLightShader(ID3D11DeviceContext* pDeviceContext, int nIndexCount, XMMATRIX& matrWorld, XMMATRIX& matrView, XMMATRIX& matrProjection
		, ID3D11ShaderResourceView* pTextureSRV, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);
	bool RenderTerrainShader(ID3D11DeviceContext* pDeviceContext, int nIndexCount, XMMATRIX& matrWorld, XMMATRIX& matrView, XMMATRIX& matrProjection
		, ID3D11ShaderResourceView* pTextureSRV, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

private:
	CColorShader* m_pColorShader{nullptr};
	CFontShader* m_pFontShader{nullptr};
	CTextureShader* m_pTextureShader{nullptr};
	CLightShader* m_pLightShader{nullptr};
	CTerrainShader* m_pTerrainShader{nullptr};
};

