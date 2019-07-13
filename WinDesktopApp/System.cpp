#include "stdafx.h"
#include "System.h"
#include "Globals.h"

System::System()
{
}


System::~System() {}

 
bool System::Initialize()
{
	int nScreenHeight{0}, nScreenWidth{0};
	
	Debug::Initialize();

	InitializeWindows(nScreenWidth, nScreenHeight);

	settings.Initialize();

	m_pInput = new CInput();

	m_pInput->Initialize();

	m_pGraphics = new Graphics();

	if (!m_pGraphics->Initialize(nScreenWidth, nScreenHeight, m_hwnd))
	{
		ShowCursor(true);
		MessageBox(m_hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		RETURN_AND_LOG(false);
	}

	Time::Initialize();

	return true;
}

void System::Shutdown()
{
	if (m_pGraphics)
	{
		m_pGraphics->Shutdown();
		delete m_pGraphics;
		m_pGraphics = nullptr;
	}

	if (m_pInput)
	{
		delete m_pInput;
		m_pInput = nullptr;
	}

	ShutdownWindows();	
	settings.Shutdown();
	Debug::Shutdown();
}

void System::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	bool bDone{ false };

	while (!bDone)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			bDone = true;
		else
			bDone = !Frame();
	}
}

bool System::Frame()
{
	Time::OnFrameStart();

	if (m_pInput->IsKeyDown(VK_ESCAPE))
		return false;

	if (!m_pGraphics->Frame())
		RETURN_AND_LOG(false);

	m_pInput->OnEndOfFrame();
	Debug::OnEndOfFrame();
	Time::OnFrameEnd();

	return true;
}

LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_KEYDOWN:
		m_pInput->KeyDown(static_cast<unsigned int>(wparam));
		return 0;
	case WM_KEYUP:
		m_pInput->KeyUp(static_cast<unsigned int>(wparam));
		return 0;
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

void System::InitializeWindows(int& nScreenWidth, int& nScreenHeight)
{
	pApplicationHandle = this;
	m_hinstance = GetModuleHandle(NULL);
	m_szApplicationName = L"Desctop App";

	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_szApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	DEVMODE dmScreenSettings;
	int posX{0};
	int posY{0};
	
	if(Settings::g_graphics.bFullScreen)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = static_cast<unsigned int>(nScreenWidth);
		dmScreenSettings.dmPelsHeight = static_cast<unsigned int>(nScreenHeight);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		nScreenWidth = 800;
		nScreenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - nScreenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - nScreenHeight) / 2;
	}

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_szApplicationName, m_szApplicationName
		, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP
		, posX, posY, nScreenWidth, nScreenHeight, NULL, NULL, m_hinstance, NULL);

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	ShowCursor(FALSE);
}

void System::ShutdownWindows()
{
	ShowCursor(TRUE);

	if(Settings::g_graphics.bFullScreen)
		ChangeDisplaySettings(NULL, 0);
	
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	UnregisterClass(m_szApplicationName, m_hinstance);
	m_hinstance = NULL;

	pApplicationHandle = nullptr;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return pApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
	}
	return 0;
}