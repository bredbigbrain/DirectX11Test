#include "stdafx.h"
#include "D3D.h"
#include "Defines.h"

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
	ON_FAIL_LOG_AND_RETURN(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory));

	IDXGIAdapter* pAdapter;
	ON_FAIL_LOG_AND_RETURN(pFactory->EnumAdapters(0, &pAdapter));

	IDXGIOutput* pAdapterOutput;
	ON_FAIL_LOG_AND_RETURN(pAdapter->EnumOutputs(0, &pAdapterOutput));

	unsigned int nNumModels;
	ON_FAIL_LOG_AND_RETURN(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nNumModels, NULL));

	DXGI_MODE_DESC* arrDisplayModelList = new DXGI_MODE_DESC[nNumModels];
	ON_FAIL_LOG_AND_RETURN(pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nNumModels, arrDisplayModelList));

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
	ON_FAIL_LOG_AND_RETURN(pAdapter->GetDesc(&adapterDesc));

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

	HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1
		, D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);	

	ON_FAIL_LOG_AND_RETURN(result);

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

	ON_FAIL_LOG_AND_RETURN(m_pDevice->CreateTexture2D(&depthBufferDesc, 0, &m_pDepthStencilBuffer));

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = TRUE;
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

	ON_FAIL_LOG_AND_RETURN(m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState));

	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	ON_FAIL_LOG_AND_RETURN(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView));

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	ON_FAIL_LOG_AND_RETURN(m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_pRasterState));

	m_pDeviceContext->RSSetState(m_pRasterState);
	m_eRasterState = ERasterState::SOLID;

	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	ON_FAIL_LOG_AND_RETURN(m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_pRasterStateNoCulling));

	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

	ON_FAIL_LOG_AND_RETURN(m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_pRasterStateWireframe));

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

	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	depthDisabledStencilDesc.DepthEnable = FALSE;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = TRUE;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ON_FAIL_LOG_AND_RETURN(m_pDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &m_pDepthDisabledStencilState));

	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	blendStateDescription.AlphaToCoverageEnable = FALSE;
	blendStateDescription.IndependentBlendEnable = FALSE;
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	ON_FAIL_LOG_AND_RETURN(m_pDevice->CreateBlendState(&blendStateDescription, &m_pAlphaEnableBlendingState));

	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	blendStateDescription.AlphaToCoverageEnable = FALSE;

	ON_FAIL_LOG_AND_RETURN(m_pDevice->CreateBlendState(&blendStateDescription, &m_pAlphaDisableBlendingState));

	blendStateDescription.AlphaToCoverageEnable = true;
	blendStateDescription.IndependentBlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendEnable = true;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	ON_FAIL_LOG_AND_RETURN(m_pDevice->CreateBlendState(&blendStateDescription, &m_pAlphaEnableBlendingState2));

	return true;
}

void D3D::Shutdown()
{
	if (m_pSwapChain)
		m_pSwapChain->SetFullscreenState(false, NULL);

	RELEASE_AND_NULL(m_pAlphaEnableBlendingState2);
	RELEASE_AND_NULL(m_pAlphaDisableBlendingState);
	RELEASE_AND_NULL(m_pAlphaEnableBlendingState);
	RELEASE_AND_NULL(m_pRasterState);
	RELEASE_AND_NULL(m_pRasterStateNoCulling);
	RELEASE_AND_NULL(m_pRasterStateWireframe);
	RELEASE_AND_NULL(m_pDepthStencilView);
	RELEASE_AND_NULL(m_pDepthStencilState);
	RELEASE_AND_NULL(m_pDepthDisabledStencilState);
	RELEASE_AND_NULL(m_pDepthStencilBuffer);
	RELEASE_AND_NULL(m_pRenderTargetView);
	RELEASE_AND_NULL(m_pDeviceContext);
	RELEASE_AND_NULL(m_pDevice);
	RELEASE_AND_NULL(m_pSwapChain);
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

XMMATRIX D3D::GetOrthoMatrix()
{
	return m_matrOrtho;
}

void D3D::GetWorldMatrix(XMMATRIX& matrix)
{
	matrix = m_matrWorld;
}

size_t D3D::GetVideoCadrDescLenght() const
{
	return sizeof(m_szVideoCardDescription);
}

void D3D::GetVideoCardInfo(char* szCardName, int& nMemoryMb)
{
	strcpy_s(szCardName, sizeof(m_szVideoCardDescription), m_szVideoCardDescription);
	nMemoryMb = m_nVideoCardMemory;
}

D3D::ERasterState D3D::GetRasterState()
{
	return m_eRasterState;
}

void D3D::TurnZBufferOn()
{
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
}

void D3D::TurnZBufferOff()
{
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthDisabledStencilState, 1);
}

void D3D::EnableAlphaBlending()
{
	float arrBlendFactor[4]{};
	m_pDeviceContext->OMSetBlendState(m_pAlphaEnableBlendingState, arrBlendFactor, UINT_MAX);
}

void D3D::DisableAlphaBlending()
{
	float arrBlendFactor[4]{};
	m_pDeviceContext->OMSetBlendState(m_pAlphaDisableBlendingState, arrBlendFactor, UINT_MAX);
}

void D3D::EnableAlphaToCoverageBlending()
{
	float arrBlendFactor[4]{};
	m_pDeviceContext->OMSetBlendState(m_pAlphaEnableBlendingState2, arrBlendFactor, UINT_MAX);
}

void D3D::TurnCullingOn()
{
	m_pDeviceContext->RSSetState(m_pRasterState);
	m_eRasterState = ERasterState::SOLID;
}

void D3D::TurnCullingOff()
{
	m_pDeviceContext->RSSetState(m_pRasterStateNoCulling);
	m_eRasterState = ERasterState::SOLID_NOCULLING;
}

void D3D::EnableWireframe()
{
	m_pDeviceContext->RSSetState(m_pRasterStateWireframe);
	m_eRasterState = ERasterState::WIREFRAME;
}

void D3D::DisableWireframe()
{
	m_pDeviceContext->RSSetState(m_pRasterState);
	m_eRasterState = ERasterState::SOLID;
}
