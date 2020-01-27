#pragma once

#include<DirectXMath.h>

using namespace DirectX;

class CPosition
{
public:
	CPosition() = default;
	CPosition(const XMFLOAT3& position, const XMFLOAT3& rotation);

	void SetPosition(float fX, float fY, float fZ);
	void SetPosition(const CPosition* pPosition);
	void SetPosition(const XMFLOAT3& position);

	void SetRotation(float fX, float fY, float fZ);
	void SetRotation(const CPosition* pPosition);
	void SetRotation(const XMFLOAT3& rotation);

	void GetPosition(float& fX, float& fY, float& fZ) const;
	void GetRotation(float& fX, float& fY, float& fZ) const;

	void SetFrameTime(float fTime);

	XMFLOAT3 GetPosition() const;
	XMVECTOR GetPositionVector() const;

	XMFLOAT3 GetRotation() const;

	XMMATRIX GetRotationMatrix() const;
	
	XMVECTOR GetForwardVector() const;
	XMVECTOR GetForwardVectorLocal() const;
	XMVECTOR GetForwardVector(XMMATRIX matrRotation) const;

	XMVECTOR GetUpVector() const;
	XMVECTOR GetUpVector(XMMATRIX matrRotation) const;

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
	float m_fRotationX = 0.f, m_fRotationY = 0.f, m_fRotationZ = 0.f; //Degrees

	float m_fFrameTime = 0.f;

	float m_fForwardSpeed = 0.f, m_fBackwardSpeed = 0.f;
	float m_fUpwardSpeed = 0.f, m_fDownwardSpeed = 0.f;
	float m_fLeftTurnSpeed = 0.f, m_fRightTurnSpeed = 0.f;
	float m_fLookUpSpeed = 0.f, m_fLookDownSpeed = 0.f;
};

