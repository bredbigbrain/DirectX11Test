#pragma once

#include<d3d11.h>
#include<directxmath.h>
#include<fstream>
#include"ShaderBase.h"

using namespace DirectX;

class CColorShader : public ShaderBase
{
public:
	CColorShader();
	~CColorShader();

	bool Initialize(ID3D11Device* pDeviceContext, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* pDeviceContext, size_t nIndexCount
		, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrProjection);

private:
	bool InitializeShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* szFileNameVS, const WCHAR* szFileNamePS);
	void RenderShader(ID3D11DeviceContext* pDeviceContext, size_t nIndexCount);
};