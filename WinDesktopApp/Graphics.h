#pragma once

#include<windows.h>
#include "stdafx.h"

#include"d3d.h"
#include"Camera.h"
#include"Model.h"
#include"ColorShader.h"
#include"TextureShader.h"

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3D* m_pDirect3D{nullptr};
	CCamera* m_pCamera{nullptr};
	Model* m_pModel{nullptr};
	//ColorShader* m_pColorShader{nullptr};
	TextureShader* m_pTextureShader{nullptr};
};

