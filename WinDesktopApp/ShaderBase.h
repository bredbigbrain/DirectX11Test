#pragma once

#include<d3d11.h>
#include<d3dcompiler.h>
#include<directxmath.h>
#include<fstream>

using namespace DirectX;

class ShaderBase
{
public:
	ShaderBase();
	virtual ~ShaderBase();

protected:
	void OutputShaderErrMsg(ID3D10Blob*, HWND, const WCHAR*);
	bool CompileShaders(ID3D11Device* pDevice, HWND hwnd, const WCHAR* szPathVS, const WCHAR* szPathPS, ID3D10Blob** ppVertexShaderBuffer);
	bool SetMatrixBuffer(ID3D11DeviceContext* pDeviceContext, XMMATRIX matrWorld
		, XMMATRIX matrView, XMMATRIX matrProjection);
	bool InitializeMatrixBuffer(ID3D11Device* pDevice);

	virtual void ShutdownShader();

	ID3D11VertexShader* m_pVertexShader{nullptr};
	ID3D11PixelShader* m_pPixelShader{nullptr};
	ID3D11InputLayout* m_pInputLayout{nullptr};
	ID3D11Buffer* m_pMatrixBuffer{nullptr};
};

