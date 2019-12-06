#include "Position.h"
#include "Globals.h"
#include <math.h>

float m_fMaxMoveSpeed = 2.5f;
float m_fMaxTurnSpeed = 0.15f;

void CPosition::SetPosition(float fX, float fY, float fZ)
{
	m_fPositionX = fX; m_fPositionY = fY; m_fPositionZ = fZ;
}
void CPosition::SetPosition(CPosition* pPosition)
{
	pPosition->GetPosition(m_fPositionX, m_fPositionY, m_fPositionZ);
}

void CPosition::SetRotation(float fX, float fY, float fZ)
{
	m_fRotationX = fX; m_fRotationY = fY; m_fRotationZ = fZ;
}
void CPosition::SetRotation(CPosition* pPosition)
{
	pPosition->GetRotation(m_fRotationX, m_fRotationY, m_fRotationZ);
}

void CPosition::GetPosition(float& fX, float& fY, float& fZ)
{
	fX = m_fPositionX; fY = m_fPositionY; fZ = m_fPositionZ;
}
XMFLOAT3 CPosition::GetPosition()
{
	return XMFLOAT3(m_fPositionX, m_fPositionY, m_fPositionZ);
}
XMVECTOR CPosition::GetPositionVector()
{
	return XMLoadFloat3(&GetPosition());
}

void CPosition::GetRotation(float& fX, float& fY, float& fZ)
{
	fX = m_fRotationX; fY = m_fRotationY; fZ = m_fRotationZ;
}
XMFLOAT3 CPosition::GetRotation()
{
	return XMFLOAT3(m_fRotationX, m_fRotationY, m_fRotationZ);
}

XMMATRIX CPosition::GetRotationMatrix()
{
	float pitch = Math3DNS::DegreesToRadians(m_fRotationX);
	float yaw = Math3DNS::DegreesToRadians(m_fRotationY);
	float roll = Math3DNS::DegreesToRadians(m_fRotationZ);

	return XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

XMVECTOR CPosition::GetForwardVector()
{
	XMVECTOR lookAtVector = XMLoadFloat3(&Math3DNS::F3_FORWARD);
	lookAtVector = XMVector3TransformCoord(lookAtVector, GetRotationMatrix());

	XMFLOAT3 position(m_fPositionX, m_fPositionY, m_fPositionZ);
	XMVECTOR positionVector = XMLoadFloat3(&position);
	return XMVectorAdd(positionVector, lookAtVector);
}

XMVECTOR CPosition::GetForwardVector(XMMATRIX matrRotation)
{
	XMVECTOR lookAtVector = XMLoadFloat3(&Math3DNS::F3_FORWARD);
	lookAtVector = XMVector3TransformCoord(lookAtVector, matrRotation);

	XMFLOAT3 position(m_fPositionX, m_fPositionY, m_fPositionZ);
	XMVECTOR positionVector = XMLoadFloat3(&position);
	return XMVectorAdd(positionVector, lookAtVector);
}

XMVECTOR CPosition::GetUpVector()
{
	XMVECTOR upVector = XMLoadFloat3(&Math3DNS::F3_UP);
	return XMVector3TransformCoord(upVector, GetRotationMatrix());
}

XMVECTOR CPosition::GetUpVector(XMMATRIX matrRotation)
{
	XMVECTOR upVector = XMLoadFloat3(&Math3DNS::F3_UP);
	return XMVector3TransformCoord(upVector, matrRotation);
}

void CPosition::SetFrameTime(float fTime)
{
	m_fFrameTime = fTime;
}

void CPosition::MoveForward(bool bKeyDown)
{
	if(bKeyDown)
	{
		m_fForwardSpeed += m_fFrameTime * 1.f;
		if(m_fForwardSpeed > m_fMaxMoveSpeed)
			m_fForwardSpeed = m_fMaxMoveSpeed;
	}
	else
	{
		m_fForwardSpeed -= m_fFrameTime * .5f;
		if(m_fForwardSpeed < 0)
			m_fForwardSpeed = 0;
	}

	if(m_fForwardSpeed != 0)
	{
		float fRadians = Math3DNS::DegreesToRadians(m_fRotationY);
		m_fPositionX += sinf(fRadians) * m_fForwardSpeed;
		m_fPositionZ += cosf(fRadians) * m_fForwardSpeed;
	}
}

void CPosition::MoveBackward(bool bKeyDown)
{
	if(bKeyDown)
	{
		m_fBackwardSpeed += m_fFrameTime * 1.f;
		if(m_fBackwardSpeed > m_fMaxMoveSpeed)
			m_fBackwardSpeed = m_fMaxMoveSpeed;
	}
	else
	{
		m_fBackwardSpeed -= m_fFrameTime * .5f;
		if(m_fBackwardSpeed < 0)
			m_fBackwardSpeed = 0;
	}

	if(m_fBackwardSpeed != 0)
	{
		float fRadians = Math3DNS::DegreesToRadians(m_fRotationY);
		m_fPositionX -= sinf(fRadians) * m_fBackwardSpeed;
		m_fPositionZ -= cosf(fRadians) * m_fBackwardSpeed;
	}
}

void CPosition::MoveUpward(bool bKeyDown)
{
	if(bKeyDown)
	{
		m_fUpwardSpeed += m_fFrameTime * 1.f;
		if(m_fUpwardSpeed > m_fMaxMoveSpeed / 30.f)
			m_fUpwardSpeed = m_fMaxMoveSpeed / 30.f;
	}
	else
	{
		m_fUpwardSpeed -= m_fFrameTime * .5f;
		if(m_fUpwardSpeed < 0)
			m_fUpwardSpeed = 0;
	}

	m_fPositionY += m_fFrameTime * m_fUpwardSpeed;
}

void CPosition::MoveDownward(bool bKeyDown)
{
	if(bKeyDown)
	{
		m_fDownwardSpeed += m_fFrameTime * 1.f;
		if(m_fDownwardSpeed > m_fMaxMoveSpeed / 30.f)
			m_fDownwardSpeed = m_fMaxMoveSpeed / 30.f;
	}
	else
	{
		m_fDownwardSpeed -= m_fFrameTime * .5f;
		if(m_fDownwardSpeed < 0)
			m_fDownwardSpeed = 0;
	}

	m_fPositionY -= m_fFrameTime * m_fDownwardSpeed;
}

void CPosition::TurnLeft(bool bKeyDown)
{
	if(bKeyDown)
	{
		m_fLeftTurnSpeed += m_fFrameTime * 5.f;
		if(m_fLeftTurnSpeed > m_fMaxTurnSpeed)
			m_fLeftTurnSpeed = m_fMaxTurnSpeed;
	}
	else
	{
		m_fLeftTurnSpeed -= m_fFrameTime * 3.5f;
		if(m_fLeftTurnSpeed < 0)
			m_fLeftTurnSpeed = 0;
	}

	m_fRotationY -= m_fFrameTime * m_fLeftTurnSpeed;

	if(m_fRotationY < 0)
		m_fRotationY += 360.f;
}

void CPosition::TurnRight(bool bKeyDown)
{
	if(bKeyDown)
	{
		m_fRightTurnSpeed += m_fFrameTime * 5.f;
		if(m_fRightTurnSpeed > m_fMaxTurnSpeed)
			m_fRightTurnSpeed = m_fMaxTurnSpeed;
	}
	else
	{
		m_fRightTurnSpeed -= m_fFrameTime * 3.5f;
		if(m_fRightTurnSpeed < 0)
			m_fRightTurnSpeed = 0;
	}

	m_fRotationY += m_fFrameTime * m_fRightTurnSpeed;

	if(m_fRotationY < 0)
		m_fRotationY += 360.f;
}

void CPosition::LookUpward(bool bKeyDown)
{
	if(bKeyDown)
	{
		m_fLookUpSpeed += m_fFrameTime * 5.f;
		if(m_fLookUpSpeed > m_fMaxTurnSpeed / 5.f)
			m_fLookUpSpeed = m_fMaxTurnSpeed;
	}
	else
	{
		m_fLookUpSpeed -= m_fFrameTime * 3.5f;
		if(m_fLookUpSpeed < 0)
			m_fLookUpSpeed = 0;
	}

	m_fRotationX -= m_fFrameTime * m_fLookUpSpeed;

	if(m_fRotationX < 0)
		m_fRotationX += 360.f;
}

void CPosition::LookDownward(bool bKeyDown)
{
	if(bKeyDown)
	{
		m_fLookDownSpeed += m_fFrameTime * 5.f;
		if(m_fLookDownSpeed > m_fMaxTurnSpeed / 5.f)
			m_fLookDownSpeed = m_fMaxTurnSpeed;
	}
	else
	{
		m_fLookDownSpeed -= m_fFrameTime * 3.5f;
		if(m_fLookDownSpeed < 0)
			m_fLookDownSpeed = 0;
	}

	m_fRotationX += m_fFrameTime * m_fLookDownSpeed;

	if(m_fRotationX >= 360.f)
		m_fRotationX -= 360.f;
}