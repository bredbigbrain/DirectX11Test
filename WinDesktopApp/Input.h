#pragma once

#include<map>
#include<chrono>

class CInput
{
	enum
	{
		KEY_W = 0x57,
		KEY_A = 0x41,
		KEY_D = 0x44,
		KEY_S = 0x53,
	};

public:
	CInput();
	~CInput();

	void Initialize();
	void OnEndOfFrame();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);
	bool IsKeyClicked(unsigned int);

	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsUpPressed();
	bool IsDownPressed();

	bool IsWPressed();
	bool IsAPressed();
	bool IsSPressed();
	bool IsDPressed();

private:
	bool m_arrKeys[256]{};
	bool m_arrClicks[256]{};
	std::map<int, std::chrono::time_point<std::chrono::steady_clock>> m_mapDownTimes;
};

