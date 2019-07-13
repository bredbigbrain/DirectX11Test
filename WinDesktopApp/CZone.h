#pragma once

#include"D3D.h"
#include"Input.h"
#include"ShaderManager.h"
#include"UserInterface.h"
#include"Camera.h"
#include"Position.h"
#include"Terrain.h"


class CZone
{
public:
	CZone();
	~CZone();

	bool Initialize(D3D* pDirect3D, HWND hWnd, int nScreenWidht, int nScreenHeight, float fScreenDepth);
	void Shutdown();
	bool Frame(D3D* pDirect3D, CInput* pInput, CShaderManager* pShManager, float fDT, int nFPS);

private:
	void HandeMovementInput(CInput* pInput, float fDT);
	bool Render(D3D* pDirect3D, CShaderManager* pShManager);

private:
	CUserInterface* m_pUserInterface{ nullptr };
	CCamera* m_pCamera{ nullptr };
	CPosition* m_pPosition{ nullptr };
	CTerrain* m_pTerrain{ nullptr };
	bool m_bDisplayUI{ false };
};

