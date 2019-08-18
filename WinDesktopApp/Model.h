#pragma once

#include<d3d11.h>
#include<DirectXMath.h>
#include"Texture.h"

using namespace DirectX;

class Model
{
private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	Model();
	~Model();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* szTexturePath);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device* pDevice);
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);

	bool LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* szTexturePath);

private:
	ID3D11Buffer* m_pVertexBuffer{nullptr};
	ID3D11Buffer* m_pIndexBuffer{nullptr};
	CTexture* m_pTexture{nullptr};
	int m_nVertexCount{0}, m_nIndexCount{0};
};