#include "Zone.h"
#include "Debug.h"
#include "Defines.h"
#include "Globals.h"

const CPosition CAMERA_START_POSITION(XMFLOAT3(128.f, 5.f, -10.f), XMFLOAT3(0.f, 0.f, 0.f));

bool CZone::Initialize(D3D* pDirect3D, HWND hWnd, int nScreenWidht, int nScreenHeight, float fScreenDepth)
{
	m_pUserInterface = new CUserInterface();

	if(!m_pUserInterface->Initialize(pDirect3D, nScreenHeight, nScreenWidht))
		RETURN_AND_LOG(false);

	m_pCamera = new CCamera();
	
	m_pCamera->CopyPosition(CAMERA_START_POSITION);
	m_pCamera->Render();
	m_pCamera->RenderBaseViewMatrix();

	m_pPosition = new CPosition(CAMERA_START_POSITION);

	m_pLightSource = new CLightSource();
	m_pLightSource->SetDiffuseColor(1.f, 1.f, 1.f, 1.f);
	m_pLightSource->SetDirection(-0.5f, -1.f, 0.5f);

	m_pTerrain = new CTerrain();

	if(!m_pTerrain->Initialize(pDirect3D->GetDevice(), Settings::TERRAIN_DATA_FILE_PATH))
		RETURN_AND_LOG(false);

	{
		using namespace Math3DNS;

		auto pModel = new Model();
		pModel->Initialize(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext(), Model::VertexDataType::TEXTURE_NBCm
			, Model::SInitOptionalParams(m_pCamera->GetPosition() + XMFLOAT3(0, -1, 7), XMFLOAT3(2, 2, 2)));
		m_mapModels[CShaderManager::EShader::TERRAIN].push_back(CRenderableModel(pModel, CTextureManager::TextureIndex::GroundForestDiff, CTextureManager::TextureIndex::GroundForestNorm));

		pModel = new Model();
		pModel->Initialize(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext(), Model::VertexDataType::TEXTURE_NBCm
			, Model::SInitOptionalParams(m_pCamera->GetPosition() + XMFLOAT3(0, 1, 7), XMFLOAT3(2, 2, 2), XMFLOAT3(180, 0, 0)));
		m_mapModels[CShaderManager::EShader::TERRAIN].push_back(CRenderableModel(pModel, CTextureManager::TextureIndex::GroundForestDiff, CTextureManager::TextureIndex::GroundForestNorm));

		pModel = new Model();
		pModel->Initialize(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext(), Model::VertexDataType::TEXTURE
			, Model::SInitOptionalParams(m_pCamera->GetPosition() + XMFLOAT3(-2, -1, 7)));
		m_mapModels[CShaderManager::EShader::TEXTURE].push_back(CRenderableModel(pModel, CTextureManager::TextureIndex::GroundForestDiffTest));
	}

	m_bDisplayUI = true;

	return true;
}


void CZone::Shutdown()
{
	SHUTDOWN_DELETE(m_pTerrain);
	DELETE(m_pPosition);
	DELETE(m_pCamera);
	DELETE(m_pLightSource);
	SHUTDOWN_DELETE(m_pUserInterface);

	for(auto& pair : m_mapModels)
	{
		for(auto& model : pair.second)
			SHUTDOWN_DELETE(model.m_pModel);
	}
	m_mapModels.clear();
}

bool CZone::Frame(D3D* pDirect3D, CInput* pInput, CShaderManager* pShManager, CTextureManager* pTexManager, float fDT, int nFPS)
{
	HandeMovementInput(pInput, fDT);

	using namespace Math3DNS;

	if(pInput->IsKeyDown(VK_LEFT) || pInput->IsKeyDown(VK_RIGHT))
	{
		int nDirection = pInput->IsKeyDown(VK_LEFT) ? -1 : 1;
		for(auto& pair : m_mapModels)
		{
			for(auto& model : pair.second)
				model.m_pModel->SetRotation(model.m_pModel->GetRotation() + XMFLOAT3(0, .1f, 0) * (fDT * nDirection));
		}
	}

	if(!Render(pDirect3D, pShManager, pTexManager))
		RETURN_AND_LOG(false);
	if(!m_pUserInterface->Frame(pDirect3D->GetDeviceContext(), nFPS, m_pPosition->GetPosition(), m_pPosition->GetRotation()))
		RETURN_AND_LOG(false);

	return true;
}

void CZone::HandeMovementInput(CInput* pInput, float fDT)
{
	m_pPosition->SetFrameTime(fDT);

	if(pInput->IsKeyClicked(VK_HOME))
	{
		*m_pPosition = CAMERA_START_POSITION;
	}
	else
	{
		m_pPosition->TurnLeft(pInput->IsAPressed());
		m_pPosition->TurnRight(pInput->IsDPressed());
		m_pPosition->LookUpward(pInput->IsUpPressed());
		m_pPosition->LookDownward(pInput->IsDownPressed());

		m_pPosition->MoveForward(pInput->IsWPressed());
		m_pPosition->MoveBackward(pInput->IsSPressed());
		m_pPosition->MoveUpward(pInput->IsKeyDown(VK_SHIFT));
		m_pPosition->MoveDownward(pInput->IsKeyDown(VK_CONTROL));
	}

	m_pCamera->SetPosition(m_pPosition);
	m_pCamera->SetRotation(m_pPosition);

	if(pInput->IsKeyClicked(VK_F1))
		m_bDisplayUI = !m_bDisplayUI;

	if(pInput->IsKeyClicked(VK_F2))
		m_bWireFrame = !m_bWireFrame;
}


bool CZone::Render(D3D* pDirect3D, CShaderManager* pShManager, CTextureManager* pTexManager)
{
	m_pCamera->Render();

	XMMATRIX matrWorld, matrProjection, matrView;
	pDirect3D->GetWorldMatrix(matrWorld);
	pDirect3D->GetProjectionMatrix(matrProjection);

	pDirect3D->BeginScene(Colors::Red);

	if(m_bWireFrame)
		pDirect3D->EnableWireframe();

	m_pTerrain->Render(pDirect3D->GetDeviceContext());

	matrView = m_pCamera->GetViewMatrix();
	bool bResult = pShManager->RenderTerrainShader(pDirect3D->GetDeviceContext()
		, m_pTerrain->GetIndexCount()
		, matrWorld, matrView, matrProjection
		, pTexManager->GetTexture(CTextureManager::TextureIndex::GroundForestDiff)
		, pTexManager->GetTexture(CTextureManager::TextureIndex::GroundForestNorm)
		, m_pLightSource->GetDirection(), m_pLightSource->GetDiffuseColor()
	);

	for(auto& pair : m_mapModels)
	{
		switch(pair.first)
		{
		case CShaderManager::EShader::TEXTURE:
			for(auto& model : pair.second)
			{
				model.m_pModel->Render(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext());
				pShManager->RenderTextureShader(pDirect3D->GetDeviceContext(), model.m_pModel->GetIndexCount(), matrWorld, matrView, matrProjection
					, pTexManager->GetTexture(model.m_eDiffuseTextureIndex));
			}
			break;
		case CShaderManager::EShader::LIGTH:
			for(auto& model : pair.second)
			{
				model.m_pModel->Render(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext());
				pShManager->RenderLightShader(pDirect3D->GetDeviceContext(), model.m_pModel->GetIndexCount(), matrWorld, matrView, matrProjection
					, pTexManager->GetTexture(model.m_eDiffuseTextureIndex), m_pLightSource->GetDirection(), m_pLightSource->GetDiffuseColor());
			}
			break;
		case CShaderManager::EShader::TERRAIN:
			for(auto& model : pair.second)
			{
				model.m_pModel->Render(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext());
				pShManager->RenderTerrainShader(pDirect3D->GetDeviceContext(), model.m_pModel->GetIndexCount(), matrWorld, matrView, matrProjection
					, pTexManager->GetTexture(model.m_eDiffuseTextureIndex), pTexManager->GetTexture(model.m_eNormTextureIndex)
					, m_pLightSource->GetDirection(), m_pLightSource->GetDiffuseColor());
			}
			break;
		case CShaderManager::EShader::COLOR:
		default:
			assert(false);
			break;
		}
	}

	if(!bResult)
		RETURN_AND_LOG(false);

	if(m_bWireFrame)
		pDirect3D->DisableWireframe();

	if(m_bDisplayUI)
	{
		if(!m_pUserInterface->Render(pDirect3D, pShManager, matrWorld, m_pCamera->GetBaseViewMatrix(), pDirect3D->GetOrthoMatrix()))
			RETURN_AND_LOG(false);
	}

	pDirect3D->EndScene();
	
	return true;
}