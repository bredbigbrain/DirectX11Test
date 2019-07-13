#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include"d3d11.h"
#include"directxmath.h"

using namespace DirectX;

class D3D
{
public:
	D3D();
	~D3D();

	bool Initialize(int nScreenWidth, int nScreenHeight, bool bVsync, HWND hwnd
		, bool bFullScreen, float fScreenDepth, float fScreenNear);
	void Shutdown();

	void BeginScene(float r, float g, float b, float a);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char* szCardName, int& nMemoryMb);

private:
	bool m_bVsyncEnabled{ false };
	int m_nVideoCardMemory{0};
	char m_szVideoCardDescription[128]{};
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
	ID3D11Texture2D* m_pDepthStencilBuffer = nullptr;
	ID3D11DepthStencilState* m_pDepthStencilState = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
	ID3D11RasterizerState* m_pRasterState = nullptr;
	XMMATRIX m_matrProjection{};
	XMMATRIX m_matrWorld{};
	XMMATRIX m_matrOrtho{};
};

