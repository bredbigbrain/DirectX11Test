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
	enum class ERasterState
	{
		SOLID, SOLID_NOCULLING, WIREFRAME 
	};

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
	XMMATRIX GetOrthoMatrix();

	size_t GetVideoCadrDescLenght() const;
	void GetVideoCardInfo(char* szCardName, int& nMemoryMb);

	ERasterState GetRasterState();

	void TurnZBufferOn();
	void TurnZBufferOff();

	void EnableAlphaBlending();
	void DisableAlphaBlending();
	void EnableAlphaToCoverageBlending();

	void TurnCullingOn();
	void TurnCullingOff();

	void EnableWireframe();
	void DisableWireframe();

private:
	ERasterState m_eRasterState;
	bool m_bVsyncEnabled{false};
	int m_nVideoCardMemory{0};
	char m_szVideoCardDescription[128]{};
	IDXGISwapChain* m_pSwapChain{nullptr};
	ID3D11Device* m_pDevice{nullptr};
	ID3D11DeviceContext* m_pDeviceContext{nullptr};
	ID3D11RenderTargetView* m_pRenderTargetView{nullptr};
	ID3D11Texture2D* m_pDepthStencilBuffer{nullptr};
	ID3D11DepthStencilState* m_pDepthStencilState{nullptr};
	ID3D11DepthStencilState* m_pDepthDisabledStencilState{nullptr};
	ID3D11DepthStencilView* m_pDepthStencilView{nullptr};
	ID3D11RasterizerState* m_pRasterState{nullptr};
	ID3D11RasterizerState* m_pRasterStateNoCulling{nullptr};
	ID3D11RasterizerState* m_pRasterStateWireframe{nullptr};
	XMMATRIX m_matrProjection{};
	XMMATRIX m_matrWorld{};
	XMMATRIX m_matrOrtho{};
	ID3D11BlendState* m_pAlphaEnableBlendingState{nullptr};
	ID3D11BlendState* m_pAlphaDisableBlendingState{nullptr};
	ID3D11BlendState* m_pAlphaEnableBlendingState2{nullptr};
};

