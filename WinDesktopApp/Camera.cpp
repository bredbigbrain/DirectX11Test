#include "stdafx.h"
#include "Camera.h"
#include "Globals.h"

CCamera::CCamera()
{
}


CCamera::~CCamera()
{
}


void CCamera::SetPosition(float x, float y, float z)
{
	m_position.SetPosition(x, y, z);
}

void CCamera::SetPosition(CPosition* pPosition)
{
	m_position.SetPosition(pPosition);
}

void CCamera::SetRotation(float x, float y, float z)
{
	m_position.SetRotation(x, y, z);
}

void CCamera::SetRotation(CPosition* pPosition)
{
	m_position.SetRotation(pPosition);
}

XMFLOAT3 CCamera::GetPosition()
{
	return m_position.GetPosition();
}


XMFLOAT3 CCamera::GetRotation()
{
	return m_position.GetRotation();
}

XMMATRIX CCamera::GetViewMatrix()
{
	return m_matrView;
}

XMMATRIX CCamera::GetBaseViewMatrix()
{
	return m_matrBaseView;
}

void CCamera::Render()
{
	XMMATRIX rotationMatrix = m_position.GetRotationMatrix();
	
	m_matrView = XMMatrixLookAtLH(m_position.GetPositionVector()
		, m_position.GetForwardVector(rotationMatrix)
		, m_position.GetUpVector(rotationMatrix));
}

void CCamera::RenderBaseViewMatrix()
{
	XMMATRIX rotationMatrix = m_position.GetRotationMatrix();

	m_matrBaseView = XMMatrixLookAtLH(m_position.GetPositionVector()
		, m_position.GetForwardVector(rotationMatrix)
		, m_position.GetUpVector(rotationMatrix));
}