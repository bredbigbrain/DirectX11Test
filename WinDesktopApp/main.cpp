#include "stdafx.h"
#include <windows.h>
#include "System.h"

#ifndef UNICODE
#define UNICODE
#endif 

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	auto pSystem = std::make_unique<System>();

	if (!pSystem->Initialize())
		return -2;
	
	pSystem->Run();

	pSystem->Shutdown();

	return 0;
}