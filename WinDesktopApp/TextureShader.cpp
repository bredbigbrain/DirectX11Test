#include "TextureShader.h"
#include "Globals.h"
#include "Debug.h"
#include "HelperFunctions.h"

CTextureShader::CTextureShader() {}


CTextureShader::~CTextureShader() {}

bool CTextureShader::Initialize(ID3D11Device* pDevice, HWND hwnd)
{
	return InitializeShader(pDevice, hwnd, L"Shaders/Texture_VS.hlsl", L"Shaders/Texture_PS.hlsl");
}

void CTextureShader::Shutdown()
{
	ShutdownShader();
}

bool CTextureShader::Render(ID3D11DeviceContext* pDeviceContext, size_t nIndexCount, XMMATRIX matrWorld
	, XMMATRIX matrView, XMMATRIX matrProjection, ID3D11ShaderResourceView* pTexture)
{
	if (!SetShaderParameters(pDeviceContext, matrWorld, matrView, matrProjection, pTexture))
		RETURN_AND_LOG(false);

	RenderShader(pDeviceContext, nIndexCount);
	return true;
}

bool CTextureShader::InitializeShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* szPathVS, const WCHAR* szPathPS)
{
	ID3D10Blob* pVertexShaderBuffer{nullptr};
	if (!CompileShaders(pDevice, hwnd, szPathVS, szPathPS, &pVertexShaderBuffer))
		RETURN_AND_LOG(false);

	D3D11_INPUT_ELEMENT_DESC arrInputLayout[2];

	arrInputLayout[0].SemanticName = "POSITION";
	arrInputLayout[0].SemanticIndex = 0;
	arrInputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	arrInputLayout[0].InputSlot = 0;
	arrInputLayout[0].AlignedByteOffset = 0;
	arrInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrInputLayout[0].InstanceDataStepRate = 0;

	arrInputLayout[1].SemanticName = "TEXCOORD";
	arrInputLayout[1].SemanticIndex = 0;
	arrInputLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	arrInputLayout[1].InputSlot = 0;
	arrInputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	arrInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrInputLayout[1].InstanceDataStepRate = 0;

	unsigned int nNumElements = sizeof(arrInputLayout) / sizeof(arrInputLayout[0]);

	ON_FAIL_LOG_AND_RETURN(pDevice->CreateInputLayout(arrInputLayout, nNumElements, pVertexShaderBuffer->GetBufferPointer()
		, pVertexShaderBuffer->GetBufferSize(), &m_pInputLayout));

	pVertexShaderBuffer->Release();

	CreateSamplerState(pDevice);

	return InitializeMatrixBuffer(pDevice);
}

void CTextureShader::ShutdownShader() 
{
	SafeReleaseAndNull(m_pSampleState);

	ShaderBase::ShutdownShader();
}


bool CTextureShader::SetShaderParameters(ID3D11DeviceContext* pDeviceContext, XMMATRIX matrWorld
	, XMMATRIX matrView, XMMATRIX matrProjection, ID3D11ShaderResourceView* pTexture)
{
	if (!SetMatrixBuffer(pDeviceContext, matrWorld, matrView, matrProjection))
		RETURN_AND_LOG(false);

	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);
	return true;
}

void CTextureShader::RenderShader(ID3D11DeviceContext* pDeviceContext, size_t nIndexCount)
{
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);

	pDeviceContext->PSGetSamplers(0, 1, &m_pSampleState);

	pDeviceContext->DrawIndexed(nIndexCount, 0, 0);
}

bool CTextureShader::CreateSamplerState(ID3D11Device* pDevice)
{
	D3D11_SAMPLER_DESC samplerDesc;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ON_FAIL_LOG_AND_RETURN(pDevice->CreateSamplerState(&samplerDesc, &m_pSampleState));

	return true;
}
