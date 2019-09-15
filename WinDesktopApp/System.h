#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "Input.h"
#include "D3D.h"
#include "Time.h"
#include "Settings.h"
#include "Zone.h"
#include "ShaderManager.h"
#include "TextureManager.h"

class System
{
public:
	System();
	~System();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	Settings::CSettings settings;
	LPCWSTR m_szApplicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	CShaderManager* m_pShManager{nullptr};
	CTextureManager* m_pTexManager{nullptr};
	D3D* m_pDirect3D{nullptr};
	CZone* m_pZone{nullptr};
	CInput* m_pInput{nullptr};
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static System* pApplicationHandle = nullptr;