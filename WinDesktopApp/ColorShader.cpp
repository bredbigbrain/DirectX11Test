#include "stdafx.h"
#include "ColorShader.h"
#include <xstring>
#include "Globals.h"

CColorShader::CColorShader() {}

CColorShader::~CColorShader() {}


bool CColorShader::Initialize(ID3D11Device* pDevice, HWND hwnd)
{	
	return InitializeShader(pDevice, hwnd, L"Shaders/Color_VS.hlsl", L"Shaders/Color_PS.hlsl");
}


void CColorShader::Shutdown()
{
	ShutdownShader();
}


bool CColorShader::Render(ID3D11DeviceContext* pDeviceContext, int nIndexCount, XMMATRIX matrWorld, XMMATRIX matrView,
	XMMATRIX matrProjection)
{
	if (!SetMatrixBuffer(pDeviceContext, matrWorld, matrView, matrProjection))
		RETURN_AND_LOG(false);

	RenderShader(pDeviceContext, nIndexCount);
	return true;
}


bool CColorShader::InitializeShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* vsFileName, const WCHAR* psFileName)
{
	ID3D10Blob* pVertexShaderBuffer{nullptr};
	if (!CompileShaders(pDevice, hwnd, vsFileName, psFileName, &pVertexShaderBuffer))
		RETURN_AND_LOG(false);

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	unsigned int nNumElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	auto result = pDevice->CreateInputLayout(polygonLayout, nNumElements, pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &m_pInputLayout);
	
	ON_FAIL_LOG_AND_RETURN(result);

	pVertexShaderBuffer->Release();

	return InitializeMatrixBuffer(pDevice);
}



void CColorShader::RenderShader(ID3D11DeviceContext* pDeviceContext, int nIndexCount)
{
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);

	pDeviceContext->DrawIndexed(nIndexCount, 0, 0);
}