#pragma once

#include<d3d11.h>
#include<directxmath.h>
#include<fstream>
#include"ShaderBase.h"

using namespace DirectX;

class ColorShader : public ShaderBase
{
public:
	ColorShader();
	~ColorShader();

	bool Initialize(ID3D11Device* pDeviceContext, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* pDeviceContext, int nIndexCount
		, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrProjection);

private:
	bool InitializeShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* szFileNameVS, const WCHAR* szFileNamePS);
	void RenderShader(ID3D11DeviceContext*, int);
};