#pragma once

#include<math.h>
#include<DirectXMath.h>

using namespace DirectX;

class CPosition
{
public:
	CPosition();
	~CPosition();

	void SetPosition(float fX, float fY, float fZ);
	void SetPosition(CPosition* pPosition);

	void SetRotation(float fX, float fY, float fZ);
	void SetRotation(CPosition* pPosition);

	void GetPosition(float& fX, float& fY, float& fZ);
	void GetRotation(float& fX, float& fY, float& fZ);

	void SetFrameTime(float fTime);

	XMFLOAT3 GetPosition();
	XMVECTOR GetPositionVector();

	XMFLOAT3 GetRotation();

	XMMATRIX GetRotationMatrix();
	
	XMVECTOR GetForwardVector();
	XMVECTOR GetForwardVector(XMMATRIX matrRotation);

	XMVECTOR GetUpVector();
	XMVECTOR GetUpVector(XMMATRIX matrRotation);

	void MoveForward(bool bKeyDown);
	void MoveBackward(bool bKeyDown);
	void MoveUpward(bool bKeyDown);
	void MoveDownward(bool bKeyDown);
	void TurnLeft(bool bKeyDown);
	void TurnRight(bool bKeyDown);
	void LookUpward(bool bKeyDown);
	void LookDownward(bool bKeyDown);

private:
	float m_fPositionX = 0.f, m_fPositionY = 0.f, m_fPositionZ = 0.f;
	float m_fRotationX = 0.f, m_fRotationY = 0.f, m_fRotationZ = 0.f;

	float m_fFrameTime = 0.f;

	float m_fForwardSpeed = 0.f, m_fBackwardSpeed = 0.f;
	float m_fUpwardSpeed = 0.f, m_fDownwardSpeed = 0.f;
	float m_fLeftTurnSpeed = 0.f, m_fRightTurnSpeed = 0.f;
	float m_fLookUpSpeed = 0.f, m_fLookDownSpeed = 0.f;
};

