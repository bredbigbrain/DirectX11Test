#include "ShaderManager.h"
#include "Debug.h"
#include "Defines.h"

bool CShaderManager::Initialize(ID3D11Device * pDevice, HWND hwnd)
{
	m_pColorShader = new CColorShader();
	if(!m_pColorShader->Initialize(pDevice, hwnd))
		RETURN_AND_LOG(false);

	m_pFontShader = new CFontShader();
	if(!m_pFontShader->Initialize(pDevice, hwnd))
		RETURN_AND_LOG(false);

	m_pTextureShader = new CTextureShader();
	if(!m_pTextureShader->Initialize(pDevice, hwnd))
		RETURN_AND_LOG(false);
	
	m_pLightShader = new CLightShader();
	if(!m_pLightShader->Initialize(pDevice, hwnd))
		RETURN_AND_LOG(false);

	return true;
}

void CShaderManager::Shutdown()
{
	SHUTDOWN_DELETE(m_pColorShader);
	SHUTDOWN_DELETE(m_pFontShader);
	SHUTDOWN_DELETE(m_pTextureShader);
	SHUTDOWN_DELETE(m_pLightShader);
}

bool CShaderManager::RenderColorShader(ID3D11DeviceContext * pDeviceContext, int nIndexCount, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrProjection)
{
	return m_pColorShader->Render(pDeviceContext, nIndexCount, matrWorld, matrView, matrProjection);
}

bool CShaderManager::RenderTextureShader(ID3D11DeviceContext * pDeviceContext, int nIndexCount, XMMATRIX& matrWorld, XMMATRIX& matrView, XMMATRIX& matrProjection
	, ID3D11ShaderResourceView* pTextureSRV)
{
	return m_pTextureShader->Render(pDeviceContext, nIndexCount, matrWorld, matrView, matrProjection, pTextureSRV);
}

bool CShaderManager::RenderFontShader(ID3D11DeviceContext * pDeviceContext, int nIndexCount, XMMATRIX& matrWorld, XMMATRIX& matrView, XMMATRIX& matrProjection
	, ID3D11ShaderResourceView * pTextureSRV, XMFLOAT4 color)
{
	return m_pFontShader->Render(pDeviceContext, nIndexCount, matrWorld, matrView, matrProjection, pTextureSRV, color);
}

bool CShaderManager::RenderLightShader(ID3D11DeviceContext* pDeviceContext, int nIndexCount, XMMATRIX& matrWorld, XMMATRIX& matrView, XMMATRIX& matrProjection
	, ID3D11ShaderResourceView* pTextureSRV, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	return m_pLightShader->Render(pDeviceContext, nIndexCount, matrWorld, matrView, matrProjection, pTextureSRV, lightDirection, diffuseColor);
}
