#pragma once

#include "D3D.h"

class CLightSource
{
public:
	void SetAmbientColor(float r, float g, float b, float a);
	void SetDiffuseColor(float r, float g, float b, float a);
	void SetDirection(float x, float y, float z);
	void SetPosition(float x, float y, float z);

	XMFLOAT4 GetAmbientColor() const;
	XMFLOAT4 GetDiffuseColor() const;
	XMFLOAT3 GetDirection() const;
	XMFLOAT3 GetPosition() const;

protected:
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
	XMFLOAT3 m_position;
};

