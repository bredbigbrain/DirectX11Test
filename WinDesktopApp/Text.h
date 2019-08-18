#pragma once

#include"D3D.h"
#include"Font.h"
#include"ShaderManager.h"

class CText
{
private:
	struct SVertex
	{
		XMFLOAT3 m_position;
		XMFLOAT2 m_textureCoord;
	};

public:
	CText();
	~CText();

	bool Initialize(ID3D11Device* pDvice, ID3D11DeviceContext* pDeviceContext, int nScreenWidth, int nScreenHeight, int nMaxLength,
		bool bEnableShadow, CFont* pFont, const char* lpszText, int nPositionX, int nPositionY, XMFLOAT4 color);
	void Shutdown();
	void Render(ID3D11DeviceContext* pDeviceContext, CShaderManager* pShManager, XMMATRIX matrWorld, XMMATRIX matrView,
		XMMATRIX matrOrtho, ID3D11ShaderResourceView* pFontTexture);

	bool UpdateSentence(ID3D11DeviceContext* pDeviceContext, CFont* pFont, const char* lpszText, int nPositionX, int nPositionY, XMFLOAT4 color);
	bool UpdateSentence(ID3D11DeviceContext* pDeviceContext, CFont* pFont, const char* lpszText);
	const char* GetText();

protected:
	bool InitilalizeSentence(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CFont* pFont, const char* lpszText, int nPositionX,
		int nPositionY, XMFLOAT4 color);
	void RenderSentence(ID3D11DeviceContext* pDeviceContext, CShaderManager* pShManager, XMMATRIX& matrWorld, XMMATRIX& matrView,
		XMMATRIX& matrOrtho, ID3D11ShaderResourceView* pFontTexture);

protected:
	ID3D11Buffer* m_pVertexBuffer{nullptr};
	ID3D11Buffer* m_pIndexBuffer{nullptr};
	ID3D11Buffer* m_pVertexBuffer2{nullptr};
	ID3D11Buffer* m_pIndexBuffer2{nullptr};
	XMFLOAT4 m_pixelColor;
	XMFLOAT2 m_lastPosition;
	int m_nScreenWidth{0};
	int m_nScreenHeight{0};
	int m_nMaxLength{0};
	int m_nVertexCount{0};
	int m_nIndexCount{0};
	bool m_bEnableShadow{false};
	std::string m_sText;
};

