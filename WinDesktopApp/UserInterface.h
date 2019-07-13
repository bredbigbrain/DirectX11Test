#pragma once

#include "Texture.h"
#include "D3D.h"
#include "ShaderManager.h"
#include "Font.h"

class CUserInterface
{
public:
	CUserInterface();
	~CUserInterface();

	bool Initialize(D3D* pDirect3D, int nScreenHeight, int nScreenWidht);
	void Shutdown();

	bool Frame(ID3D11DeviceContext* pDeviceContext, int nFPS, XMFLOAT3 position, XMFLOAT3 rotation);
	bool Render(D3D* pDirect3D, CShaderManager* pShManager, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrProjection);

private:
	bool UpdateFpsString(ID3D11DeviceContext* pDeviceContext, int nFPS);
	bool UpdatePositionString(ID3D11DeviceContext* pDeviceContext, XMFLOAT3 position, XMFLOAT3 rotation);

private:
	CFont* m_pFont{nullptr};
	CText* m_pFpsString{nullptr};
	CText* m_pPositionSting{nullptr};
	CText* m_pVideoSting{nullptr};
	XMFLOAT3 m_prevPosition, m_prevRotation;
	int m_pPrevFps{0};
};

