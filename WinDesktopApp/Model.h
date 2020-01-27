#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Texture.h"
#include "Position.h"

using namespace DirectX;

class Model
{
private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 texCoord;
	};
	struct Vertex_N : public Vertex	//normal
	{
		XMFLOAT3 normal;
	};
	struct Vertex_NBCm : public Vertex_N	//normal, bump, colormap
	{
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
		SInitOptionalParams(XMFLOAT3 pos, XMFLOAT3 scale, XMFLOAT3 rotation) : m_position(pos), m_scale(scale), m_rotation(rotation) {}

		XMFLOAT3 m_position{0, 0, 0};
		XMFLOAT3 m_rotation{0, 0, 0};
		XMFLOAT3 m_scale{1.f, 1.f, 1.f};
		CTexture* m_pTexture = nullptr;
	};

	Model() = default;

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, XMFLOAT3 position, VertexDataType eVertexDataType, const char* szTexturePath);
	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, VertexDataType eVertexDataType, const SInitOptionalParams& optionalPrms);
	void Shutdown();
	void Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	VertexDataType GetVertexDataType() const { return m_eVertexDataType; }
	int GetIndexCount() const;
	void SetTexture(CTexture* pTexture);
	ID3D11ShaderResourceView* GetTexture() const;

	XMFLOAT3 GetPosition() const { return m_position.GetPosition(); }
	void SetPosition(XMFLOAT3 position);

	XMFLOAT3 GetRotation() const { return m_position.GetRotation(); }
	void SetRotation(XMFLOAT3 rotation);

	XMFLOAT3 GetScale() const { return m_scale; }
	void SetScale(XMFLOAT3 scale);

private:
	bool InitializeBuffers(ID3D11Device* pDevice);
	bool InitializeVertexBuffer(ID3D11Device* pDevice);
	void* CreateVertices(VertexDataType eVertexDataType);
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);
	size_t GetVertexSize() const;

	bool LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* szTexturePath);

private:
	VertexDataType m_eVertexDataType{VertexDataType::TEXTURE};
	CPosition m_position;
	XMFLOAT3 m_scale{1.f, 1.f, 1.f};
	ID3D11Buffer* m_pVertexBuffer{nullptr};
	ID3D11Buffer* m_pIndexBuffer{nullptr};
	CTexture* m_pTexture{nullptr};
	bool m_bHandleTexture{false};
	bool m_bTransformed{false};
	bool m_bInitialized{false};
	int m_nVertexCount{0}, m_nIndexCount{0};
};