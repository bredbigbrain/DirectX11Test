#include "UserInterface.h"
#include "Debug.h"


CUserInterface::CUserInterface()
{
}


CUserInterface::~CUserInterface()
{
}

bool CUserInterface::Initialize(D3D* pDirect3D, int nScreenHeight, int nScreenWidht)
{
	m_pFont = new CFont();
	
	bool bResult = m_pFont->Initialize(pDirect3D->GetDevice(), pDirect3D->GetDeviceContext(), "", "", 32.f, 3);
	if(!bResult)
		RETURN_AND_LOG(false);


}