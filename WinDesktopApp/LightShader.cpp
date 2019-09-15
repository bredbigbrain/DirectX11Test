#include "LightShader.h"
#include "Debug.h"


bool CLightShader::Initialize(ID3D11Device* pDevice, HWND hwnd)
{
	return InitializeShader(pDevice, hwnd, L"Shaders/Light_VS.hlsl", L"Shaders/Light_PS.hlsl");
}

bool CLightShader::Render(ID3D11DeviceContext * pDeviceContext, int nIndexCount, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrProjection, ID3D11ShaderResourceView * pTexture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	if(!SetShaderParameters(pDeviceContext, matrWorld, matrView, matrProjection, pTexture, lightDirection, diffuseColor))
		RETURN_AND_LOG(false);

	RenderShader(pDeviceContext, nIndexCount);
	return true;
}

bool CLightShader::InitializeShader(ID3D11Device * pDevice, HWND hwnd, const WCHAR * szPathVS, const WCHAR * szPathPS)
{
	ID3D10Blob* pVertexShaderBuffer{nullptr};
	if(!CompileShaders(pDevice, hwnd, szPathVS, szPathPS, &pVertexShaderBuffer))
		RETURN_AND_LOG(false);

	D3D11_INPUT_ELEMENT_DESC arrInputLayout[3];

	arrInputLayout[0].SemanticName = "POSITION";
	arrInputLayout[0].SemanticIndex = 0;
	arrInputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	arrInputLayout[0].InputSlot = 0;
	arrInputLayout[0].AlignedByteOffset = 0;
	arrInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrInputLayout[0].InstanceDataStepRate = 0;

	arrInputLayout[1].SemanticName = "TEXCOORD";
	arrInputLayout[1].SemanticIndex = 0;
	arrInputLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	arrInputLayout[1].InputSlot = 0;
	arrInputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	arrInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrInputLayout[1].InstanceDataStepRate = 0;

	arrInputLayout[2].SemanticName = "NORMAL";
	arrInputLayout[2].SemanticIndex = 0;
	arrInputLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	arrInputLayout[2].InputSlot = 0;
	arrInputLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	arrInputLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	arrInputLayout[2].InstanceDataStepRate = 0;

	unsigned int nNumElements = sizeof(arrInputLayout) / sizeof(arrInputLayout[0]);

	ON_FAIL_LOG_AND_RETURN(pDevice->CreateInputLayout(arrInputLayout, nNumElements, pVertexShaderBuffer->GetBufferPointer()
		, pVertexShaderBuffer->GetBufferSize(), &m_pInputLayout));

	pVertexShaderBuffer->Release();

	CreateSamplerState(pDevice);

	if(!InitializeMatrixBuffer(pDevice))
		RETURN_AND_LOG(false);

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(SLightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	ON_FAIL_LOG_AND_RETURN(pDevice->CreateBuffer(&lightBufferDesc, nullptr, &m_pLightBuffer));

	return true;
}

bool CLightShader::SetShaderParameters(ID3D11DeviceContext * pDeviceContext, XMMATRIX matrWorld, XMMATRIX matrView, XMMATRIX matrProjection, ID3D11ShaderResourceView * pTexture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	if(!CTextureShader::SetShaderParameters(pDeviceContext, matrWorld, matrView, matrProjection, pTexture))
		RETURN_AND_LOG(false);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ON_FAIL_LOG_AND_RETURN(pDeviceContext->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	auto dataPtr = static_cast<SLightBuffer*>(mappedResource.pData);
	dataPtr->diffuseColor = diffuseColor;
	dataPtr->lightDirection = lightDirection;
	dataPtr->fPadding = 0.f;

	pDeviceContext->Unmap(m_pLightBuffer, 0);
	pDeviceContext->PSSetConstantBuffers(0, 1, &m_pLightBuffer);

	return true;
}
