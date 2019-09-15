#include "LightSource.h"

void CLightSource::SetAmbientColor(float r, float g, float b, float a)
{
	m_ambientColor = XMFLOAT4(r, g, b, a);
}

void CLightSource::SetDiffuseColor(float r, float g, float b, float a)
{
	m_diffuseColor = XMFLOAT4(r, g, b, a);
}

void CLightSource::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
}

void CLightSource::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}

XMFLOAT4 CLightSource::GetAmbientColor() const
{
	return m_ambientColor;
}

XMFLOAT4 CLightSource::GetDiffuseColor() const
{
	return m_diffuseColor;
}

XMFLOAT3 CLightSource::GetDirection() const
{
	return m_direction;
}

XMFLOAT3 CLightSource::GetPosition() const
{
	return m_position;
}