#pragma once
#define WIN32_LEAN_AND_MEAN

#include<windows.h>
#include"Input.h"
#include"Graphics.h"
#include"Time.h"
#include"Settings.h"
#include"Debug.h"

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

	Input* m_pInput = nullptr;
	Graphics* m_pGraphics = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static System* pApplicationHandle = nullptr;