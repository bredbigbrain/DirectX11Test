#include "Graphics.h"
#include "Globals.h"
#include "HelperFunctions.h"

Graphics::Graphics()
{
}


Graphics::~Graphics()
{
}

bool Graphics::Initialize(int nScreenWidth, int nScreenHeight, HWND hwnd)
{
	m_pDirect3D = new D3D();

	if(!m_pDirect3D->Initialize(nScreenWidth, nScreenHeight, Settings::g_graphics.bVSync, hwnd
		, Settings::g_graphics.bFullScreen, Settings::g_graphics.fScreenDepth, Settings::g_graphics.fScreenNear))
	{
		Debug::LogNow("Graphics::Initialize: Could not initialize Direct3D!");
		return false;
	}

	m_pCamera = new CCamera();

	m_pCamera->SetPosition(0.f, 0.f, -5.f);

	m_pModel = new Model();

	if(!m_pModel->Initialize(m_pDirect3D->GetDevice(), m_pDirect3D->GetDeviceContext(), {0, 0, 0}, Model::VertexDataType::TEXTURE, "Res/Wall.tga"))
	{
		Debug::LogNow("Graphics::Initialize: Could not initialize model!");
		return false;
	}

	m_pTextureShader = new CTextureShader();

	if(!m_pTextureShader->Initialize(m_pDirect3D->GetDevice(), hwnd))
	{
		Debug::LogNow("Graphics::Initialize: Could not initialize shader!");
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	SafeShutdounAndDelete(m_pTextureShader);
	SafeShutdounAndDelete(m_pModel);
	SafeDelete(m_pCamera);
	SafeShutdounAndDelete(m_pDirect3D);
}

bool Graphics::Frame()
{
	return Render();
}

bool Graphics::Render()
{
	m_pDirect3D->BeginScene(Colors::AliceBlue);
	
	m_pCamera->Render();

	XMMATRIX matrWorld, matrProjection;
	m_pDirect3D->GetWorldMatrix(matrWorld);
	m_pDirect3D->GetProjectionMatrix(matrProjection);
	XMMATRIX matrView = m_pCamera->GetViewMatrix();

	m_pModel->Render(m_pDirect3D->GetDevice(), m_pDirect3D->GetDeviceContext());
	if (!m_pTextureShader->Render(m_pDirect3D->GetDeviceContext(), m_pModel->GetIndexCount()
		, matrWorld, matrView, matrProjection, m_pModel->GetTexture()))
		RETURN_AND_LOG(false);

	m_pDirect3D->EndScene();
	return true;
}