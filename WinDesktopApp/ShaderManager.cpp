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

	return true;
}

void CShaderManager::Shutdown()
{
	SHUTDOWND_DELETE(m_pColorShader);
	SHUTDOWND_DELETE(m_pFontShader);
}

bool CShaderManager::RenderColorShader(ID3D11DeviceContext * pDeviceContext, int nIndexCount, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrProjection)
{
	return m_pColorShader->Render(pDeviceContext, nIndexCount, matrWorld, matrView, matrProjection);
}

bool CShaderManager::RenderFontShader(ID3D11DeviceContext * pDeviceContext, int nIndexCount
	, XMMATRIX& matrWorld, XMMATRIX& matrView, XMMATRIX& matrProjection, ID3D11ShaderResourceView * pTextureSRV, XMFLOAT4 color)
{
	return m_pFontShader->Render(pDeviceContext, nIndexCount, matrWorld, matrView, matrProjection, pTextureSRV, color);
}
