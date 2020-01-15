#pragma once

#include "stdafx.h"
#include "D3D.h"
#include "Input.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "UserInterface.h"
#include "Camera.h"
#include "Position.h"
#include "Terrain.h"
#include "LightSource.h"
#include "Model.h"

class CZone
{
public:

	bool Initialize(D3D* pDirect3D, HWND hWnd, int nScreenWidht, int nScreenHeight, float fScreenDepth);
	void Shutdown();
	bool Frame(D3D* pDirect3D, CInput* pInput, CShaderManager* pShManager, CTextureManager* pTexManager, float fDT, int nFPS);

private:
	void HandeMovementInput(CInput* pInput, float fDT);
	bool Render(D3D* pDirect3D, CShaderManager* pShManager, CTextureManager* pTexManager);

private:
	class CRenderableModel
	{
	public:
		CRenderableModel() = default;
		CRenderableModel(Model* pModel, CTextureManager::TextureIndex eDiffIndex
			, CTextureManager::TextureIndex eNormIndex = CTextureManager::TextureIndex::UNDEFINED)
			: m_eDiffuseTextureIndex(eDiffIndex), m_eNormTextureIndex(eNormIndex), m_pModel(pModel)
		{}

		CTextureManager::TextureIndex m_eDiffuseTextureIndex = CTextureManager::TextureIndex::UNDEFINED;
		CTextureManager::TextureIndex m_eNormTextureIndex = CTextureManager::TextureIndex::UNDEFINED;
		Model* m_pModel = nullptr;
	};
	std::map<CShaderManager::EShader, std::vector<CRenderableModel>> m_mapModels;  //<Shader ,<Model, textureIndex>>

	CUserInterface* m_pUserInterface{nullptr};
	CCamera* m_pCamera{nullptr};
	CPosition* m_pPosition{nullptr};
	CTerrain* m_pTerrain{nullptr};
	CLightSource* m_pLightSource{nullptr};
	bool m_bDisplayUI{false};
	bool m_bWireFrame{false};
};

