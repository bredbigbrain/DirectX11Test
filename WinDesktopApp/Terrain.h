#pragma once

#include<d3d11.h>
#include<directxmath.h>

using namespace DirectX;

class CTerrain
{
private:
	struct SVertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	CTerrain();
	~CTerrain();

	bool Initialize(ID3D11Device* pDevice);
	void Shutdown();
	bool Render(ID3D11DeviceContext* pDeviceContext);

	size_t GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* pDevice);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);

private:
	ID3D11Buffer* m_pVertexBuffer{nullptr};
	ID3D11Buffer* m_pIndexBuffer{nullptr};
	size_t m_nVertexCount{0};
	size_t m_nIndexCount{0};
};

