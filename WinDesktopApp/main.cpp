#include "stdafx.h"
#include <windows.h>
#include "System.h"

#ifndef UNICODE
#define UNICODE
#endif 

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	System* pSystem = new System();

	if (!pSystem->Initialize())
		return -2;
	
	pSystem->Run();

	pSystem->Shutdown();
	delete pSystem;

	return 0;
}