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
		XMFLOAT2 texCoord;
		XMFLOAT3 normal;
	};

	struct SModel
	{
		float x = 0, y = 0, z = 0;
		float tu = 0, tv = 0;
		float nx = 0, ny = 0, nz = 0;
	};

	struct SHeightMap
	{
		float x = 0, y = 0, z = 0;
		float nx = 0, ny = 0, nz = 0;
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
	bool CalculateNormals();

private:
	ID3D11Buffer* m_pVertexBuffer{nullptr};
	ID3D11Buffer* m_pIndexBuffer{nullptr};
	size_t m_nVertexCount{0};
	size_t m_nIndexCount{0};

	int m_nTerrainHeight{0};
	int m_nTerrainWidth{0};
	float m_fHeightScale{1};
	char* m_lpszTerrainFileName{nullptr};
	SHeightMap* m_arrHeightMap{nullptr};
	SModel* m_arrTerrainModel{nullptr};
};

