#pragma once

#include "D3D.h"
#include "ColorShader.h"
#include "FontShader.h"

class CShaderManager
{
public:
	bool Initialize(ID3D11Device* pDevice, HWND hwnd);
	void Shutdown();

	bool RenderColorShader(ID3D11DeviceContext* pDeviceContext, int nIndexCount, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrProjection);
	bool RenderFontShader(ID3D11DeviceContext* pDeviceContext, int nIndexCount, XMMATRIX& matrWorld, XMMATRIX& matrView, XMMATRIX& matrProjection
		, ID3D11ShaderResourceView* pTextureSRV, XMFLOAT4 color);

private:
	CColorShader* m_pColorShader{nullptr};
	CFontShader* m_pFontShader{nullptr};
};

