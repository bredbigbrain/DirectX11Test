#pragma once

#include<d3d11.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<fstream>
#include"ShaderBase.h"

using namespace DirectX;

class CTextureShader : public ShaderBase
{
public:
	CTextureShader();
	~CTextureShader();

	virtual bool Initialize(ID3D11Device* pDevice, HWND hwnd);
	virtual void Shutdown();
	bool Render(ID3D11DeviceContext* pDeviceContext, size_t nIndexCount, XMMATRIX matrWorld
		, XMMATRIX matrView, XMMATRIX matrProjection, ID3D11ShaderResourceView* pTexture);

protected:
	virtual bool InitializeShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* szPathVS, const WCHAR* szPathPS);
	virtual void ShutdownShader() override;
	virtual void RenderShader(ID3D11DeviceContext* pDiviceContext, size_t nIndexCount);

	virtual bool CreateSamplerState(ID3D11Device* pDevice);

	bool SetShaderParameters(ID3D11DeviceContext* pDeviceContext, XMMATRIX matrWorld
		, XMMATRIX matrView, XMMATRIX matrProjection, ID3D11ShaderResourceView* pTexture);

protected:
	ID3D11SamplerState* m_pSampleState{nullptr};
};

