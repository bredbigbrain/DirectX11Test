#include "CZone.h"
#include"Debug.h"


CZone::CZone()
{
}


CZone::~CZone()
{
}


bool CZone::Initialize(D3D* pDirect3D, HWND hWnd, int nScreenWidht, int nScreenHeight, float fScreenDepth)
{
	m_pUserInterface = new CUserInterface();
	if(!m_pUserInterface)
		RETURN_AND_LOG(false);

	if(m_pUserInterface->Initialize(pDirect3D, nScreenHeight, nScreenWidht))
		RETURN_AND_LOG(false);

	m_pCamera = new CCamera();
	if(!m_pCamera)
		RETURN_AND_LOG(false);
	
	m_pCamera->SetPosition(128.f, 5.f, -10.f);
	m_pCamera->Render();
	m_pCamera->RenderBaseViewMatrix();

	m_pPosition = new CPosition();
	if(!m_pPosition)
		RETURN_AND_LOG(false);

	m_pPosition->SetPosition(128.f, 5.f, -10.f);
	m_pPosition->SetRotation(0.f, 0.f, 0.f);

	m_pTerrain = new CTerrain();
	if(!m_pTerrain)
		RETURN_AND_LOG(false);

	if(!m_pTerrain->Initialize(pDirect3D->GetDevice()))
		RETURN_AND_LOG(false);

	m_bDisplayUI = true;

	return true;
}


void CZone::Shutdown()
{
	if(m_pTerrain)
	{
		m_pTerrain->Shutdown();
		delete m_pTerrain;
		m_pTerrain = nullptr;
	}

	if(m_pPosition)
	{
		delete m_pPosition;
		m_pPosition = nullptr;
	}

	if(m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	if(m_pUserInterface)
	{
		m_pUserInterface->Shutdown();
		delete m_pUserInterface;
		m_pUserInterface = nullptr;
	}
}

bool CZone::Frame(D3D* pDirect3D, CInput* pInput, CShaderManager* pShManager, float fDT, int nFPS)
{
	HandeMovementInput(pInput, fDT);

	float fPosX = 0, fPosY = 0, fPosZ = 0, fRotX = 0, fRotY = 0, fRotZ = 0;

	m_pPosition->GetPosition(fPosX, fPosY, fPosZ);
	m_pPosition->GetRotation(fRotX, fRotY, fRotZ);

	if(!m_pUserInterface()->Frame(pDirect3D->GetDevice(), nFPS, fPosX, fPosY, fPosZ, fRotX, fRotY, fRotZ))
		RETURN_AND_LOG(false);
	if(!Render(pDirect3D, pShManager))
		RETURN_AND_LOG(false);

	return true;
}

void CZone::HandeMovementInput(CInput* pInput, float fDT)
{
	m_pPosition->SetFrameTime(fDT);

	m_pPosition->TurnLeft(pInput->IsAPressed());
	m_pPosition->TurnRight(pInput->IsDPressed());
	m_pPosition->LookUpward(pInput->IsUpPressed());
	m_pPosition->LookDownward(pInput->IsDownPressed());

	m_pPosition->MoveForward(pInput->IsWPressed());
	m_pPosition->MoveBackward(pInput->IsSPressed());
	m_pPosition->MoveUpward(pInput->IsKeyDown(VK_SHIFT));
	m_pPosition->MoveDownward(pInput->IsKeyDown(VK_CONTROL));

	m_pCamera->SetPosition(m_pPosition);
	m_pCamera->SetRotation(m_pPosition);

	if(pInput->IsKeyClicked(VK_F1))
		m_bDisplayUI = !m_bDisplayUI;
}


bool CZone::Render(D3D* pDirect3D, CShaderManager* pShManager)
{
	m_pCamera->Render();

	XMMATRIX matrWorld, matrView, matrProjection, matrOrtho, matrBaseView;
	m_pCamera->GetViewMatrix(matrView);
	m_pCamera->GetBaseViewMatrix(matrBaseView);
	pDirect3D->GetWorldMatrix(matrWorld);
	pDirect3D->GetProjectionMatrix(matrProjection);
	pDirect3D->GetOrthoMatrix(matrOrtho);

	pDirect3D->BeginScene(0.f, 0.f, 0.f, 1.f);

	m_pTerrain->Render(pDirect3D->GetDeviceContext());

	bool bResult = pShManager->RenderColorShader(pDirect3D->GetDeviceContext(), m_pTerrain->GetIndexCount()
		, matrWorld, matrView, matrProjection);

	if(!bResult)
		RETURN_AND_LOG(false);

	if(m_bDisplayUI)
	{
		if(!m_pUserInterface->Render(pDirect3D, pShManager, matrWorld, matrBaseView, matrOrtho))
			RETURN_AND_LOG(false);
	}

	pDirect3D->EndScene();
	
	return true;
}