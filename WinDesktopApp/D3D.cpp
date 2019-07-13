#include "stdafx.h"
#include "D3D.h"


D3D::D3D()
{
}


D3D::~D3D()
{
}


bool D3D::Initialize(int nScreenWidth, int nScreenHeight, bool bVsync, HWND hwnd
	, bool bFullScreen, float fScreenDepth, float fScreenNear)
{
	m_bVsyncEnabled = bVsync;

	IDXGIFactory* pFactory;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory)))
		RETURN_AND_LOG(false);

	IDXGIAdapter* pAdapter;
	if (FAILED(pFactory->EnumAdapters(0, &pAdapter)))
		RETURN_AND_LOG(false);

	IDXGIOutput* pAdapterOutput;
	if (FAILED(pAdapter->EnumOutputs(0, &pAdapterOutput)))
		RETURN_AND_LOG(false);

	unsigned int nNumModels;
	if (FAILED(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nNumModels, NULL)))
		RETURN_AND_LOG(false);

	DXGI_MODE_DESC* arrDisplayModelList = new DXGI_MODE_DESC[nNumModels];

	if (FAILED(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nNumModels, arrDisplayModelList)))
		RETURN_AND_LOG(false);

	bool bResult{ false };
	unsigned int nNumerator{0}, nDenomerator{0};
	for (size_t i = 0; i < nNumModels; ++i)
	{
		if (arrDisplayModelList[i].Height == static_cast<unsigned int>(nScreenHeight)
			&& arrDisplayModelList[i].Width == static_cast<unsigned int>(nScreenWidth))
		{
			nNumerator = arrDisplayModelList[i].RefreshRate.Numerator;
			nDenomerator = arrDisplayModelList[i].RefreshRate.Denominator;
			bResult = true;
			break;
		}
	}

	if (!bResult)
		RETURN_AND_LOG(false);

	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(pAdapter->GetDesc(&adapterDesc)))
		RETURN_AND_LOG(false);

	m_nVideoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	unsigned long long lnStringLenght;
	auto nSize = sizeof(m_szVideoCardDescription);
	auto nError = wcstombs_s(&lnStringLenght, m_szVideoCardDescription, nSize, adapterDesc.Description, nSize);
	if (nError != 0)
		RETURN_AND_LOG(false);

	delete[] arrDisplayModelList;
	pAdapterOutput->Release();
	pAdapter->Release();
	pFactory->Release();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = nScreenWidth;
	swapChainDesc.BufferDesc.Height = nScreenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	
	if (m_bVsyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = nNumerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = nDenomerator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.Windowed = !bFullScreen;
	
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	auto result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1
		, D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);	

	if (FAILED(result))
		RETURN_AND_LOG(false);

	ID3D11Texture2D* pBackBuffer;
	if(FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
		RETURN_AND_LOG(false);

	if(FAILED(m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView)))
		RETURN_AND_LOG(false);

	pBackBuffer->Release();

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = nScreenWidth;
	depthBufferDesc.Height = nScreenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	if(FAILED(m_pDevice->CreateTexture2D(&depthBufferDesc, 0, &m_pDepthStencilBuffer)))
		RETURN_AND_LOG(false);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState)))
		RETURN_AND_LOG(false);

	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if(FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView)))
		RETURN_AND_LOG(false);

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	if(FAILED(m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_pRasterState)))
		RETURN_AND_LOG(false);

	m_pDeviceContext->RSSetState(m_pRasterState);

	D3D11_VIEWPORT viewport;

	viewport.Width = static_cast<float>(nScreenWidth);
	viewport.Height = static_cast<float>(nScreenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_pDeviceContext->RSSetViewports(1, &viewport);

	float fFieldOfView = 3.141592654f / 4.0f;
	float fScreenAspect = static_cast<float>(nScreenWidth) / static_cast<float>(nScreenHeight);

	m_matrProjection = XMMatrixPerspectiveFovLH(fFieldOfView, fScreenAspect, fScreenNear, fScreenDepth);
	m_matrWorld = XMMatrixIdentity();
	m_matrOrtho = XMMatrixOrthographicLH(static_cast<float>(nScreenWidth), static_cast<float>(nScreenHeight)
		, fScreenNear, fScreenDepth);

	return true;
}

void D3D::Shutdown()
{
	if (m_pSwapChain)
		m_pSwapChain->SetFullscreenState(false, NULL);
	if (m_pRasterState)
		m_pRasterState->Release();
	if (m_pDepthStencilView)
	{
		m_pDepthStencilView->Release();
		m_pDepthStencilView = nullptr;
	}
	if (m_pDepthStencilState)
	{
		m_pDepthStencilState->Release();
		m_pDepthStencilState = nullptr;
	}
	if (m_pDepthStencilBuffer)
	{
		m_pDepthStencilBuffer->Release();
		m_pDepthStencilBuffer = nullptr;
	}
	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = nullptr;
	}
	if (m_pDeviceContext)
	{
		m_pDeviceContext->Release();
		m_pDeviceContext = nullptr;
	}
	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = nullptr;
	}
	if (m_pSwapChain)
	{
		m_pSwapChain->Release();
		m_pSwapChain = nullptr;
	}
}

void D3D::BeginScene(float r, float g, float b, float a)
{
	float color[4]{r, g, b, a};
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D::EndScene()
{
	m_pSwapChain->Present(m_bVsyncEnabled ? 1 : 0, 0);
}

ID3D11Device* D3D::GetDevice()
{
	return m_pDevice;
}

ID3D11DeviceContext* D3D::GetDeviceContext()
{
	return m_pDeviceContext;
}

void D3D::GetProjectionMatrix(XMMATRIX& matrix)
{
	matrix = m_matrProjection;
}

void D3D::GetOrthoMatrix(XMMATRIX& matrix)
{
	matrix = m_matrOrtho;
}

void D3D::GetWorldMatrix(XMMATRIX& matrix)
{
	matrix = m_matrWorld;
}

void D3D::GetVideoCardInfo(char* szCardName, int& nMemoryMb)
{
	strcpy_s(szCardName, sizeof(m_szVideoCardDescription), m_szVideoCardDescription);
	nMemoryMb = m_nVideoCardMemory;
}