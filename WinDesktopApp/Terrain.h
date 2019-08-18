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
	bool Initialize(ID3D11Device* pDevice, const char* lpszSetupFilePath);
	void Shutdown();
	bool Render(ID3D11DeviceContext* pDeviceContext);

	size_t GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* pDevice);
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);

	bool LoadSetupFile(const char* lpszFilePath);
	bool LoadBitmapHeightMap();
	void SetTerrainCoordinates();
	bool BuildTerrainModel();

private:
	ID3D11Buffer* m_pVertexBuffer{nullptr};
	ID3D11Buffer* m_pIndexBuffer{nullptr};
	size_t m_nVertexCount{0};
	size_t m_nIndexCount{0};

	int m_nTerrainHeight{0};
	int m_nTerrainWidth{0};
	float m_fHeightScale{1};
	char* m_lpszTerrainFileName{nullptr};
	XMFLOAT3* m_arrHeightMap{nullptr};
	XMFLOAT3* m_arrTerrainModel{nullptr};
};

