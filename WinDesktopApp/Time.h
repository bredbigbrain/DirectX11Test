#pragma once
#include"stdafx.h"
#include<chrono>

namespace Time
{
	void Initialize();
	void OnFrameStart();
	void OnFrameEnd();

	long GetDtMS();
	double GetDtS();
}

