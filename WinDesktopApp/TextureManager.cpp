#include "TextureManager.h"
#include "HelperFunctions.h"
#include "Debug.h"

bool CTextureManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	if(!Initialize(m_vecTextures.size()))
		RETURN_AND_LOG(false);

	for(size_t i = 0; i < m_vecTextures.size(); ++i)
	{
		if(!LoadTexture(pDevice, pDeviceContext, m_vecTextures[i].c_str(), i))
			RETURN_AND_LOG(false);
	}
	return true;
}

bool CTextureManager::Initialize(size_t nTexturesCount)
{
	m_nTexturesCount = nTexturesCount;
	m_arrTextures = new CTexture[m_nTexturesCount];

	return true;
}

void CTextureManager::Shutdown()
{
	if(m_arrTextures)
	{
		for(size_t i = 0; i < m_nTexturesCount; i++)
			m_arrTextures[i].Shutdown();
		SafeDeleteArray(m_arrTextures);
	}
}

bool CTextureManager::LoadTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const char * lpszTexturePath, size_t nIndex)
{
	if(nIndex >= m_nTexturesCount)
		RETURN_AND_LOG(false);
	
	if(!m_arrTextures[nIndex].Initialize(pDevice, pDeviceContext, lpszTexturePath))
		RETURN_AND_LOG(false);

	return true;
}

ID3D11ShaderResourceView* CTextureManager::GetTexture(size_t nIndex) const
{
	if(nIndex >= m_nTexturesCount)
		RETURN_AND_LOG(nullptr);

	return m_arrTextures[nIndex].GetTexture();
}

ID3D11ShaderResourceView* CTextureManager::GetTexture(TextureIndex eIndex) const
{
	if(eIndex == TextureIndex::UNDEFINED)
	{
		QUIK_LOG_ML(2, "CTextureManager::GetTexture: UNDEFINED texture requested.");
		return nullptr;
	}

	return GetTexture(static_cast<size_t>(eIndex));
}