#pragma once
#include "TextureShader.h"

class CFontShader : private CTextureShader
{
	struct SPixelBuffer
	{
		XMFLOAT4 color;
	};

public:
	virtual bool Initialize(ID3D11Device* pDevice, HWND hwnd) override;
	virtual void Shutdown() override;
	bool Render(ID3D11DeviceContext* pDeviceContext, int nIndexCount
		, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrProjection
		, ID3D11ShaderResourceView* pTextureSRV, XMFLOAT4 color);

protected:
	bool InitializeShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* szFileNameVS, const WCHAR* szFileNamePS);
	bool SetShaderParameters(ID3D11DeviceContext* pDeviceContext, XMMATRIX matrWorld
		, XMMATRIX matrView, XMMATRIX matrProjection, ID3D11ShaderResourceView* pTexture, XMFLOAT4 color);
	virtual void RenderShader(ID3D11DeviceContext* pDeviceContext, int nIndexCount) override;
	virtual void ShutdownShader() override;

protected:
	ID3D11Buffer* m_pPixelBuffer{nullptr};
};

