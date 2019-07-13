#pragma once

#include<DirectXMath.h>
#include"Position.h"

using namespace DirectX;

class CCamera
{
public:
	CCamera();
	~CCamera();

	void SetPosition(float, float, float);
	void SetPosition(CPosition* pPosition);

	void SetRotation(float, float, float);
	void SetRotation(CPosition* pPosition);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void RenderBaseViewMatrix();

	XMMATRIX GetViewMatrix();
	XMMATRIX GetBaseViewMatrix();

protected:
	CPosition m_position;
	XMMATRIX m_matrView, m_matrBaseView;
};

