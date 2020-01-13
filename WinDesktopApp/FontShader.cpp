#include "FontShader.h"
#include "Debug.h"
#include "Defines.h"


bool CFontShader::Initialize(ID3D11Device * pDevice, HWND hwnd)
{
	return InitializeShader(pDevice, hwnd, L"Shaders/Texture_VS.hlsl", L"Shaders/Font_PS.hlsl");
}

void CFontShader::Shutdown()
{
	ShutdownShader();
}

bool CFontShader::Render(ID3D11DeviceContext * pDeviceContext, size_t nIndexCount, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrProjection, ID3D11ShaderResourceView * pTextureSRV, XMFLOAT4 color)
{
	if(!SetShaderParameters(pDeviceContext, matrWorld, matrView, matrProjection, pTextureSRV, color))
		return false;

	RenderShader(pDeviceContext, nIndexCount);
	return true;
}

bool CFontShader::InitializeShader(ID3D11Device * pDevice, HWND hwnd, const WCHAR * szFileNameVS, const WCHAR * szFileNamePS)
{
	if(!CTextureShader::InitializeShader(pDevice, hwnd, szFileNameVS, szFileNamePS))
		RETURN_AND_LOG(false);

	D3D11_BUFFER_DESC pixelBufDesc;
	pixelBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufDesc.ByteWidth = sizeof(SPixelBuffer);
	pixelBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufDesc.MiscFlags = 0;
	pixelBufDesc.StructureByteStride = 0;

	ON_FAIL_LOG_AND_RETURN(pDevice->CreateBuffer(&pixelBufDesc, nullptr, &m_pPixelBuffer));

	return true;
}

bool CFontShader::SetShaderParameters(ID3D11DeviceContext* pDeviceContext, XMMATRIX matrWorld
	, XMMATRIX matrView, XMMATRIX matrProjection, ID3D11ShaderResourceView* pTexture, XMFLOAT4 color)
{
	if(!CTextureShader::SetShaderParameters(pDeviceContext, matrWorld, matrView, matrProjection, pTexture))
		RETURN_AND_LOG(false);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ON_FAIL_LOG_AND_RETURN(pDeviceContext->Map(m_pPixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	SPixelBuffer* pPixelBufferData = (SPixelBuffer*)mappedResource.pData;
	pPixelBufferData->color = color;

	pDeviceContext->Unmap(m_pPixelBuffer, 0);
	pDeviceContext->PSSetConstantBuffers(0, 1, &m_pPixelBuffer);

	return true;
}

void CFontShader::RenderShader(ID3D11DeviceContext* pDeviceContext, size_t nIndexCount)
{
	CTextureShader::RenderShader(pDeviceContext, nIndexCount);
}

void CFontShader::ShutdownShader()
{
	RELEASE_AND_NULL(m_pPixelBuffer);

	CTextureShader::ShutdownShader();
}
