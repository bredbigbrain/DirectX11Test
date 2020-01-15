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
		XMFLOAT2 texCoord;
	};
	struct Vertex_N	//normal
	{
		XMFLOAT3 position;
		XMFLOAT2 texCoord;
		XMFLOAT3 normal;
	};
	struct Vertex_NBCm	//normal, bump, colormap
	{
		XMFLOAT3 position;
		XMFLOAT2 texCoord;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
		XMFLOAT3 color;
	};
public:

	enum class VertexDataType { TEXTURE, TEXTURE_N, TEXTURE_NBCm };

	struct SInitOptionalParams
	{
		SInitOptionalParams() = default;
		SInitOptionalParams(XMFLOAT3 pos) : m_position(pos){}
		SInitOptionalParams(XMFLOAT3 pos, XMFLOAT3 scale) : m_position(pos), m_scale(scale) {}

		XMFLOAT3 m_position;
		XMFLOAT3 m_scale{1.f, 1.f, 1.f};
		CTexture* m_pTexture = nullptr;
	};

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, XMFLOAT3 position, VertexDataType eVertexDataType, const char* szTexturePath);
	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, VertexDataType eVertexDataType, const SInitOptionalParams& optionalPrms);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	VertexDataType GetVertexDataType() const { return m_eVertexDataType; }
	int GetIndexCount() const;
	void SetTexture(CTexture* pTexture);
	ID3D11ShaderResourceView* GetTexture() const;

	XMFLOAT3 GetPosition() const { return m_position; }

private:
	bool InitializeBuffers(ID3D11Device* pDevice);
	void* CreateVertices(VertexDataType eVertexDataType);
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);
	size_t GetVertexSize() const;

	bool LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* szTexturePath);

private:
	VertexDataType m_eVertexDataType{VertexDataType::TEXTURE};
	XMFLOAT3 m_position;
	XMFLOAT3 m_scale{1.f, 1.f, 1.f};
	ID3D11Buffer* m_pVertexBuffer{nullptr};
	ID3D11Buffer* m_pIndexBuffer{nullptr};
	CTexture* m_pTexture{nullptr};
	bool m_bHandleTexture{false};
	int m_nVertexCount{0}, m_nIndexCount{0};
};