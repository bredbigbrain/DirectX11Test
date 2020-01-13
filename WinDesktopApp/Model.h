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
	struct VertexWithNormal
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

public:

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, XMFLOAT3 position, bool bUseNormal, const char* szTexturePath);
	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, XMFLOAT3 position, bool bUseNormal, CTexture* pTexture = nullptr);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	bool IsUsingNormals() const { return m_bUseNormals; }
	int GetIndexCount() const;
	void SetTexture(CTexture* pTexture);
	ID3D11ShaderResourceView* GetTexture() const;

	XMFLOAT3 GetPosition() const { return m_position; }

private:
	bool InitializeBuffers(ID3D11Device* pDevice);
	void* CreateVertices(bool bUseNormal);
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);
	size_t GetVertexSize() const;

	bool LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* szTexturePath);

private:
	bool m_bUseNormals{false};
	XMFLOAT3 m_position;
	ID3D11Buffer* m_pVertexBuffer{nullptr};
	ID3D11Buffer* m_pIndexBuffer{nullptr};
	CTexture* m_pTexture{nullptr};
	bool m_bHandleTexture{false};
	int m_nVertexCount{0}, m_nIndexCount{0};
};