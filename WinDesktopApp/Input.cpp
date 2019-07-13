#include "stdafx.h"
#include "Input.h"
#include "Globals.h"

CInput::CInput() 
{
}


CInput::~CInput() {}

void CInput::Initialize()
{
}

void CInput::OnEndOfFrame()
{
	ZeroMemory(m_arrClicks, sizeof(m_arrClicks));
}

void CInput::KeyDown(unsigned int nKey)
{
	m_arrKeys[nKey] = true;
	m_mapDownTimes[nKey] = std::chrono::high_resolution_clock::now();
}

void CInput::KeyUp(unsigned int nKey)
{
	m_arrKeys[nKey] = false;
	auto now = std::chrono::high_resolution_clock::now();
	long long lMS = std::chrono::duration_cast<std::chrono::milliseconds>(m_mapDownTimes[nKey] - now).count();
	m_arrClicks[nKey] = lMS <= Input::CLICK_TIME_DELTA;
}

bool CInput::IsKeyClicked(unsigned int nKey)
{
	return m_arrClicks[nKey];
}

bool CInput::IsKeyDown(unsigned int nKey)
{
	return m_arrKeys[nKey];
}

bool CInput::IsLeftPressed()
{
	return m_arrKeys[VK_LEFT];
}

bool CInput::IsRightPressed()
{
	return m_arrKeys[VK_RIGHT];
}

bool CInput::IsUpPressed()
{
	return m_arrKeys[VK_UP];
}

bool CInput::IsDownPressed()
{
	return m_arrKeys[VK_DOWN];
}

bool CInput::IsWPressed()
{
	return m_arrKeys[KEY_W];
}

bool CInput::IsAPressed()
{
	return m_arrKeys[KEY_A];
}

bool CInput::IsSPressed()
{
	return m_arrKeys[KEY_S];
}

bool CInput::IsDPressed()
{
	return m_arrKeys[KEY_D];
}