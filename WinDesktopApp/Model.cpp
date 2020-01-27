#include "stdafx.h"
#include "Model.h"
#include "Defines.h"
#include "Globals.h"
#include "Terrain.h"

bool Model::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, XMFLOAT3 position, VertexDataType eVertexDataType, const char* szTexturePath)
{
	assert(!m_bInitialized);
	if(m_bInitialized)
		RETURN_AND_LOG(false);

	m_position.SetPosition(position);
	m_eVertexDataType = eVertexDataType;

	if (!InitializeBuffers(pDevice))
		RETURN_AND_LOG(false);

	if (!LoadTexture(pDevice, pDeviceContext, szTexturePath))
		RETURN_AND_LOG(false);
	
	return true;
}

bool Model::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, VertexDataType eVertexDataType, const SInitOptionalParams& optionalPrms)
{
	assert(!m_bInitialized);
	if(m_bInitialized)
		RETURN_AND_LOG(false);

	m_position.SetPosition(optionalPrms.m_position);
	m_position.SetRotation(optionalPrms.m_rotation);
	m_scale = optionalPrms.m_scale;
	m_eVertexDataType = eVertexDataType;

	if(!InitializeBuffers(pDevice))
		RETURN_AND_LOG(false);

	SetTexture(optionalPrms.m_pTexture);

	return true;
}

void Model::Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	if(m_bTransformed)
	{
		RELEASE_AND_NULL(m_pVertexBuffer);
		InitializeVertexBuffer(pDevice);
	}

	RenderBuffers(pDeviceContext);
}

void Model::Shutdown()
{
	if(m_bHandleTexture)
		SHUTDOWN_DELETE(m_pTexture);

	RELEASE_AND_NULL(m_pVertexBuffer);
	RELEASE_AND_NULL(m_pIndexBuffer);
}

int Model::GetIndexCount() const
{
	return m_nIndexCount;
}

void Model::SetTexture(CTexture* pTexture)
{
	m_bHandleTexture = false;
	m_pTexture = pTexture;
}

ID3D11ShaderResourceView* Model::GetTexture() const
{
	return m_pTexture->GetTexture();
}

void Model::SetPosition(XMFLOAT3 position)
{
	using namespace Math3DNS;

	m_bTransformed |= position != GetPosition();
	m_position.SetPosition(position);
}

void Model::SetRotation(XMFLOAT3 rotation)
{
	using namespace Math3DNS;

	m_bTransformed |= rotation != GetRotation();
	m_position.SetRotation(rotation);
}

void Model::SetScale(XMFLOAT3 scale)
{
	using namespace Math3DNS;

	m_bTransformed |= scale != m_scale;
	m_scale = scale;
}

bool Model::InitializeBuffers(ID3D11Device* pDevice)
{
	if(!InitializeVertexBuffer(pDevice))
		RETURN_AND_LOG(false);

	m_nIndexCount = 6;
	auto arrIndices = new unsigned int[m_nIndexCount];
	arrIndices[0] = 0;
	arrIndices[1] = 1;
	arrIndices[2] = 2;
	arrIndices[3] = 0;
	arrIndices[4] = 2;
	arrIndices[5] = 3;

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_nIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	indexData.pSysMem = arrIndices;

	if (FAILED(pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer)))
		RETURN_AND_LOG(false);

	delete[] arrIndices;

	return true;
}

bool Model::InitializeVertexBuffer(ID3D11Device * pDevice)
{
	auto arrVertices = CreateVertices(m_eVertexDataType);

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = GetVertexSize() * m_nVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	vertexData.pSysMem = arrVertices;

	if(FAILED(pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer)))
		RETURN_AND_LOG(false);

	delete[] arrVertices;
	return true;
}

void* Model::CreateVertices(VertexDataType eVertexDataType)
{
	m_nVertexCount = 4;

	auto funcCalculatePosition = [this](float x, float y, float z) -> XMFLOAT3
	{
		using namespace Math3DNS;
		
		XMMATRIX transform = m_position.GetRotationMatrix() * XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

		XMFLOAT3 tmpPos(x, y, z);
		DirectX::XMStoreFloat3(&tmpPos, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&tmpPos), transform));
		return tmpPos + m_position.GetPosition();
	};

	auto funcCalculateNormal = [this]() -> XMFLOAT3
	{
		using namespace Math3DNS;
		return ToXMFLOAT3(m_position.GetForwardVectorLocal()) * -1;
	};

	switch(eVertexDataType)
	{
	case VertexDataType::TEXTURE:
	{
		auto arrVertices = new Vertex[m_nVertexCount];
		arrVertices[0].position = funcCalculatePosition(0.f, 0.f, 0.f);
		arrVertices[0].texCoord = XMFLOAT2(0.f, 0.f);

		arrVertices[1].position = funcCalculatePosition(0.f, 1.f, 0.f);
		arrVertices[1].texCoord = XMFLOAT2(0.f, 1.f);

		arrVertices[2].position = funcCalculatePosition(1.f, 1.f, 0.f);
		arrVertices[2].texCoord = XMFLOAT2(1.f, 1.f);

		arrVertices[3].position = funcCalculatePosition(1.f, 0.f, 0.f);
		arrVertices[3].texCoord = XMFLOAT2(1.f, 0.f);

		return arrVertices;
	}
	case VertexDataType::TEXTURE_N:
	{
		auto arrVertices = new Vertex_N[m_nVertexCount];
		arrVertices[0].position = funcCalculatePosition(0.f, 0.f, 0.f);
		arrVertices[0].texCoord = XMFLOAT2(0.f, 0.f);
		arrVertices[0].normal = funcCalculateNormal();

		arrVertices[1].position = funcCalculatePosition(0.f, 1.f, 0.f);
		arrVertices[1].texCoord = XMFLOAT2(0.f, 1.f);
		arrVertices[1].normal = funcCalculateNormal();

		arrVertices[2].position = funcCalculatePosition(1.f, 1.f, 0.f);
		arrVertices[2].texCoord = XMFLOAT2(1.f, 1.f);
		arrVertices[2].normal = funcCalculateNormal();

		arrVertices[3].position = funcCalculatePosition(1.f, 0.f, 0.f);
		arrVertices[3].texCoord = XMFLOAT2(1.f, 0.f);
		arrVertices[3].normal = funcCalculateNormal();

		return arrVertices;
	}
	case VertexDataType::TEXTURE_NBCm:
	{
		auto arrVertices = new Vertex_NBCm[m_nVertexCount];
		arrVertices[0].position = funcCalculatePosition(0.f, 0.f, 0.f);
		arrVertices[0].texCoord = XMFLOAT2(0.f, 0.f);
		arrVertices[0].normal = funcCalculateNormal();

		arrVertices[1].position = funcCalculatePosition(0.f, 1.f, 0.f);
		arrVertices[1].texCoord = XMFLOAT2(0.f, 1.f);
		arrVertices[1].normal = funcCalculateNormal();

		arrVertices[2].position = funcCalculatePosition(1.f, 1.f, 0.f);
		arrVertices[2].texCoord = XMFLOAT2(1.f, 1.f);
		arrVertices[2].normal = funcCalculateNormal();

		arrVertices[3].position = funcCalculatePosition(1.f, 0.f, 0.f);
		arrVertices[3].texCoord = XMFLOAT2(1.f, 0.f);
		arrVertices[3].normal = funcCalculateNormal();

		arrVertices[0].color = arrVertices[1].color = arrVertices[2].color = arrVertices[3].color = XMFLOAT3(1, 1, 1);

		CTerrain::SVector binormal, tangent;
		CTerrain::CalculateTangetnBinormal(CTerrain::STempVertex(arrVertices[0].position, arrVertices[0].normal, arrVertices[0].texCoord)
			, CTerrain::STempVertex(arrVertices[1].position, arrVertices[1].normal, arrVertices[1].texCoord)
			, CTerrain::STempVertex(arrVertices[2].position, arrVertices[2].normal, arrVertices[2].texCoord)
			, tangent, binormal);

		arrVertices[0].binormal = arrVertices[1].binormal = arrVertices[2].binormal = arrVertices[3].binormal = binormal;
		arrVertices[0].tangent = arrVertices[1].tangent = arrVertices[2].tangent = arrVertices[3].tangent = tangent;

		return arrVertices;
	}
	}
}

bool Model::LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* szTexturePath)
{
	m_bHandleTexture = true;
	m_pTexture = new CTexture;

	return m_pTexture->Initialize(pDevice, pDeviceContext, szTexturePath);
}

void Model::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
{
	unsigned int nStride = GetVertexSize();
	unsigned int nOffset{0};

	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &nStride, &nOffset);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

size_t Model::GetVertexSize() const
{
	switch(m_eVertexDataType)
	{
	case VertexDataType::TEXTURE: return sizeof(Vertex);
	case VertexDataType::TEXTURE_N: return sizeof(Vertex_N);
	case VertexDataType::TEXTURE_NBCm: return sizeof(Vertex_NBCm);
	default: assert(false); return 0;
	}
}
