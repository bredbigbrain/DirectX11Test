#include "stdafx.h"
#include "ShaderBase.h"
#include "Globals.h"
#include "Defines.h"

ShaderBase::ShaderBase()
{
}


ShaderBase::~ShaderBase()
{
}

bool ShaderBase::CompileShaders(ID3D11Device* pDevice, HWND hwnd, const WCHAR* szPathVS, const WCHAR* szPathPS, ID3D10Blob** ppVertexShaderBuffer)
{
	HRESULT result;
	ID3D10Blob* pPixelShaderBuffer{nullptr};
	ID3D10Blob* pErrorMessage{nullptr};

	result = D3DCompileFromFile(szPathVS, NULL, NULL, "vsMain", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		ppVertexShaderBuffer, &pErrorMessage);
	if (FAILED(result))
	{
		if (pErrorMessage)
			OutputShaderErrMsg(pErrorMessage, hwnd, szPathVS);
		else
			MessageBox(hwnd, szPathVS, L"Missing Shader File", MB_OK | MB_ICONWARNING);
		RETURN_AND_LOG_ERR(false, result);
	}

	result = D3DCompileFromFile(szPathPS, NULL, NULL, "psMain", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pPixelShaderBuffer, &pErrorMessage);
	if (FAILED(result))
	{
		if (pErrorMessage)
			OutputShaderErrMsg(pErrorMessage, hwnd, szPathPS);
		else
			MessageBox(hwnd, szPathPS, L"Missing Shader File", MB_OK | MB_ICONWARNING);
		RETURN_AND_LOG_ERR(false, result);
	}

	result = pDevice->CreateVertexShader((*ppVertexShaderBuffer)->GetBufferPointer(), (*ppVertexShaderBuffer)->GetBufferSize(), NULL, &m_pVertexShader);
	ON_FAIL_LOG_AND_RETURN(result);

	result = pDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader);
	ON_FAIL_LOG_AND_RETURN(result);

	pPixelShaderBuffer->Release();
	return true;
}

void ShaderBase::OutputShaderErrMsg(ID3D10Blob* pErrorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	char* compileErrors = (char*)(pErrorMessage->GetBufferPointer());
	unsigned long long bufferSize = pErrorMessage->GetBufferSize();

	std::ofstream fout;
	fout.open("shader-error.txt");

	for (unsigned long long i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	pErrorMessage->Release();
	pErrorMessage = 0;

	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

void ShaderBase::ShutdownShader()
{
	RELEASE_AND_NULL(m_pMatrixBuffer);
	RELEASE_AND_NULL(m_pInputLayout);
	RELEASE_AND_NULL(m_pPixelShader);
	RELEASE_AND_NULL(m_pVertexShader);
}

bool ShaderBase::SetMatrixBuffer(ID3D11DeviceContext* pDeviceContext, XMMATRIX matrWorld
	, XMMATRIX matrView, XMMATRIX matrProjection)
{
	matrWorld = XMMatrixTranspose(matrWorld);
	matrView = XMMatrixTranspose(matrView);
	matrProjection = XMMatrixTranspose(matrProjection);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ON_FAIL_LOG_AND_RETURN(pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	Math3DNS::MatrixBuffer_t* dataPtr = (Math3DNS::MatrixBuffer_t*)mappedResource.pData;

	dataPtr->world = matrWorld;
	dataPtr->view = matrView;
	dataPtr->projection = matrProjection;

	pDeviceContext->Unmap(m_pMatrixBuffer, 0);

	unsigned int bufferNumber = 0;
	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_pMatrixBuffer);

	return true;
}

bool ShaderBase::InitializeMatrixBuffer(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC matrixBufferDesc;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(Math3DNS::MatrixBuffer_t);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	ON_FAIL_LOG_AND_RETURN(pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer));

	return true;
}
