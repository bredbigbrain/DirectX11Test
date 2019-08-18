#include "UserInterface.h"
#include "Globals.h"
#include "Debug.h"
#include "Defines.h"
#include <sstream>



bool CUserInterface::Initialize(D3D* pDirect3D, int nScreenHeight, int nScreenWidht)
{
	m_pFont = new CFont();
	
	bool bResult = m_pFont->Initialize(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext(), "Res/Font/font01.txt", "Res/Font/font01.tga", 32.f, 3);
	if(!bResult)
		RETURN_AND_LOG(false);

	m_pFpsString = new CText();
	bResult = m_pFpsString->Initialize(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext(), nScreenWidht, nScreenHeight, 16, false, m_pFont
		, "Fps: 0", 0, 0, DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f));

	if(!bResult)
		RETURN_AND_LOG(false);

	m_nPrevFps = -1;

	int nVideoMemMB{0};
	char* szVideCard = new char[pDirect3D->GetVideoCadrDescLenght()];
	pDirect3D->GetVideoCardInfo(szVideCard, nVideoMemMB);

	std::string str = "Video card: ";
	str += szVideCard;
	delete[] szVideCard;
	str += " (";
	str += std::to_string(nVideoMemMB);
	str += " MB)";

	m_pVideoString = new CText();

	bResult = m_pVideoString->Initialize(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext(), nScreenWidht, nScreenHeight, str.size(), false, m_pFont
		, str.c_str(), 0, 0, DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f));

	if(!bResult)
		RETURN_AND_LOG(false);

	m_pPositionString = new CText();
	bResult = m_pPositionString->Initialize(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext(), nScreenWidht, nScreenHeight, Debug::MAX_LOG_LENGTH, false, m_pFont
		, "Position: NULL", 0, -0, DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f));

	if(!bResult)
		RETURN_AND_LOG(false);

	return true;
}

void CUserInterface::Shutdown()
{
	SHUTDOWND_DELETE(m_pPositionString);
	SHUTDOWND_DELETE(m_pVideoString);
	SHUTDOWND_DELETE(m_pFpsString);
	SHUTDOWND_DELETE(m_pFont);
}

bool CUserInterface::Frame(ID3D11DeviceContext * pDeviceContext, int nFPS, XMFLOAT3 position, XMFLOAT3 rotation)
{
	if(!UpdateFpsString(pDeviceContext, nFPS))
		RETURN_AND_LOG(false);

	if(!UpdatePositionString(pDeviceContext, position, rotation))
		RETURN_AND_LOG(false);

	return true;
}

bool CUserInterface::Render(D3D * pDirect3D, CShaderManager * pShManager, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrOrtho)
{
	pDirect3D->TurnZBufferOff();
	pDirect3D->EnableAlphaBlending();

	m_pFpsString->Render(pDirect3D->GetDeviceContext(), pShManager, matrWorld, matrView, matrOrtho, m_pFont->GetTexture());
	m_pVideoString->Render(pDirect3D->GetDeviceContext(), pShManager, matrWorld, matrView, matrOrtho, m_pFont->GetTexture());
	m_pPositionString->Render(pDirect3D->GetDeviceContext(), pShManager, matrWorld, matrView, matrOrtho, m_pFont->GetTexture());

	pDirect3D->DisableAlphaBlending();
	pDirect3D->TurnZBufferOn();

	return true;
}

bool CUserInterface::UpdateFpsString(ID3D11DeviceContext * pDeviceContext, int nFPS)
{
	if(m_nPrevFps == nFPS)
		return true;

	m_nPrevFps = nFPS;

	if(nFPS > 99999)
		nFPS = 99999;

	XMFLOAT4 color(0.f, 1.f, 0.f, 1.f);			//green
	if(nFPS < 30)
		color = XMFLOAT4(1.f, 0.f, 0.f, 1.f);	//red
	else if(nFPS < 60)
		color = XMFLOAT4(1.f, 1.f, 0.f, 1.f);	//yellow

	std::string str = "Fps: ";
	str += std::to_string(nFPS);

	return m_pFpsString->UpdateSentence(pDeviceContext, m_pFont, str.c_str(), 0, 0, color);
}

bool CUserInterface::UpdatePositionString(ID3D11DeviceContext * pDeviceContext, XMFLOAT3& position, XMFLOAT3& rotation)
{
	if(m_prevPosition.x == position.x && m_prevPosition.y == position.y && m_prevPosition.z == position.z
		&& m_prevRotation.x == rotation.x && m_prevRotation.y == rotation.y && m_prevRotation.z == rotation.z)
	{
		return true;
	}

	std::ostringstream os;
	os << "Position: (";
	os << position.x;
	os << ", ";
	os << position.y;
	os << ", ";
	os << position.z;
	os << ") Rotation: (";
	os << rotation.x;
	os << ", ";
	os << rotation.y;
	os << ", ";
	os << rotation.z;
	os << ")";

	return m_pPositionString->UpdateSentence(pDeviceContext, m_pFont, os.str().c_str(), 0, 0, XMFLOAT4(1.f, 1.f, 1.f, 1.f));
}
