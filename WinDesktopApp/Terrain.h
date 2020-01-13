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
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
		XMFLOAT3 color;
	};

	struct SVector
	{
		float x{0}, y{0}, z{0};
	};

	struct SModel
	{
		float x{0}, y{0}, z{0};
		float tu{0}, tv{0};
		float nx{0}, ny{0}, nz{0};
		float tx{0}, ty{0}, tz{0};
		float bx{0}, by{0}, bz{0};
		float r{0}, g{0}, b{0};

		void SetTangent(const SVector& vec)
		{
			bx = vec.x;
			by = vec.y;
			bz = vec.z;
		}

		void SetBinormal(const SVector& vec)
		{
			bx = vec.x;
			by = vec.y;
			bz = vec.z;
		}
	};

	struct SHeightMap
	{
		float x{0}, y{0}, z{0};
		float nx{0}, ny{0}, nz{0};
		float r{0}, g{0}, b{0};
	};

	struct STempVertex
	{
		float x{0}, y{0}, z{0};
		float tu{0}, tv{0};
		float nx{0}, ny{0}, nz{0};

		STempVertex() = default;
		STempVertex(const SModel& model) 
			: x{model.x}, y{model.y}, z{model.z}
			, nx{model.nx}, ny{model.ny}, nz{model.nz}
			, tu{model.tu}, tv{model.tv}
		{}

		void Copy(const SModel& model)
		{
			x = model.x;
			y = model.y;
			z = model.z;
			nx = model.nx;
			ny = model.ny;
			nz = model.nz;
			tu = model.tu;
			tv = model.tv;
		}
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
	bool LoadColorMap();
	void SetTerrainCoordinates();
	bool BuildTerrainModel();
	bool CalculateNormals();
	void CalculateTerrainVectors();
	void CalculateTangetnBinormal(STempVertex vertex1, STempVertex vertex2, STempVertex vertex3, SVector& tangent, SVector& binormal);

private:
	ID3D11Buffer* m_pVertexBuffer{nullptr};
	ID3D11Buffer* m_pIndexBuffer{nullptr};
	size_t m_nVertexCount{0};
	size_t m_nIndexCount{0};

	int m_nTerrainHeight{0};
	int m_nTerrainWidth{0};
	float m_fHeightScale{1};
	char* m_lpszTerrainFileName{nullptr};
	char* m_lpszColorMapFileName{nullptr};
	SHeightMap* m_arrHeightMap{nullptr};
	SModel* m_arrTerrainModel{nullptr};
};

