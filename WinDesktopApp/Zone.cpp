#include "Zone.h"
#include "Debug.h"
#include "Defines.h"
#include "Globals.h"

CZone::CZone()
{
}


CZone::~CZone()
{
}


bool CZone::Initialize(D3D* pDirect3D, HWND hWnd, int nScreenWidht, int nScreenHeight, float fScreenDepth)
{
	m_pUserInterface = new CUserInterface();

	if(!m_pUserInterface->Initialize(pDirect3D, nScreenHeight, nScreenWidht))
		RETURN_AND_LOG(false);

	m_pCamera = new CCamera();
	
	m_pCamera->SetPosition(128.f, 10.f, -10.f);
	m_pCamera->Render();
	m_pCamera->RenderBaseViewMatrix();

	m_pPosition = new CPosition();

	m_pPosition->SetPosition(128.f, 5.f, -10.f);
	m_pPosition->SetRotation(0.f, 0.f, 0.f);

	m_pLightSource = new CLightSource();
	m_pLightSource->SetDiffuseColor(1.f, 1.f, 1.f, 1.f);
	m_pLightSource->SetDirection(-0.5f, -1.f, -0.5f);

	m_pTerrain = new CTerrain();

	if(!m_pTerrain->Initialize(pDirect3D->GetDevice(), Settings::TERRAIN_DATA_FILE_PATH))
		RETURN_AND_LOG(false);

	m_bDisplayUI = true;
	//m_bWireFrame = true;

	return true;
}


void CZone::Shutdown()
{
	SHUTDOWN_DELETE(m_pTerrain);
	DELETE(m_pPosition);
	DELETE(m_pCamera);
	DELETE(m_pLightSource);
	SHUTDOWN_DELETE(m_pUserInterface);
}

bool CZone::Frame(D3D* pDirect3D, CInput* pInput, CShaderManager* pShManager, CTextureManager* pTexManager, float fDT, int nFPS)
{
	HandeMovementInput(pInput, fDT);

	if(!Render(pDirect3D, pShManager, pTexManager))
		RETURN_AND_LOG(false);
	if(!m_pUserInterface->Frame(pDirect3D->GetDeviceContext(), nFPS, m_pPosition->GetPosition(), m_pPosition->GetRotation()))
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

	if(pInput->IsKeyClicked(VK_F2))
		m_bWireFrame = !m_bWireFrame;

	if(pInput->IsKeyClicked(VK_HOME))
	{
		m_pPosition->SetPosition(128.f, 5.f, -10.f);
		m_pPosition->SetRotation(0.f, 0.f, 0.f);
	}
}


bool CZone::Render(D3D* pDirect3D, CShaderManager* pShManager, CTextureManager* pTexManager)
{
	m_pCamera->Render();

	XMMATRIX matrWorld, matrProjection, matrView;
	pDirect3D->GetWorldMatrix(matrWorld);
	pDirect3D->GetProjectionMatrix(matrProjection);

	pDirect3D->BeginScene(0.f, 0.f, 0.f, 1.f);

	if(m_bWireFrame)
		pDirect3D->EnableWireframe();

	m_pTerrain->Render(pDirect3D->GetDeviceContext());

	matrView = m_pCamera->GetViewMatrix();
	bool bResult = pShManager->RenderLightShader(pDirect3D->GetDeviceContext(), m_pTerrain->GetIndexCount()
		, matrWorld, matrView, matrProjection, pTexManager->GetTexture(1), m_pLightSource->GetDirection(), m_pLightSource->GetDiffuseColor());

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