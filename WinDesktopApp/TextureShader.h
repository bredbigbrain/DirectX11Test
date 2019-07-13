#pragma once

#include<d3d11.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<fstream>
#include"ShaderBase.h"

using namespace DirectX;

class TextureShader : public ShaderBase
{
public:
	TextureShader();
	~TextureShader();

	bool Initialize(ID3D11Device* pDevice, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* pDeviceContext, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView* pTexture);

private:
	bool InitializeShader(ID3D11Device* pDevice, HWND, const WCHAR*, const WCHAR*);
	virtual void ShutdownShader();
	void RenderShader(ID3D11DeviceContext* pDiviceContext, int);

	bool SetShaderParameters(ID3D11DeviceContext* pDeviceContext, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView* pTexture);

private:
	ID3D11SamplerState* m_pSampleState{nullptr};
};

