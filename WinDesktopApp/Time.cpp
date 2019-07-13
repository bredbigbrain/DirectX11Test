#include"Time.h"
#include"Debug.h"
#include"Globals.h"

namespace Time
{
	static std::chrono::steady_clock::time_point startFrameTime;
	static double nDtMS{ Settings::g_graphics.GetFrameDurationMS() };
	static double nDtSec{ Settings::g_graphics.GetFrameDurationMS() / 1000. };

	void Initialize()
	{
	}

	void OnFrameStart()
	{
		startFrameTime = std::chrono::high_resolution_clock::now();
	}

	static long i{0};
	void OnFrameEnd()
	{
		auto timeNow = std::chrono::high_resolution_clock::now();

		if (std::chrono::duration_cast<std::chrono::seconds>(timeNow - startFrameTime).count() > 1)
			nDtMS = 1000.;
		else
			nDtMS = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - startFrameTime).count();

		if(nDtMS < Settings::g_graphics.GetFrameDurationMS())
		{
			Sleep(Settings::g_graphics.GetFrameDurationMS() - nDtMS);
			nDtMS = Settings::g_graphics.GetFrameDurationMS();
		}
		nDtSec = nDtMS / 1000.;

		startFrameTime = timeNow;
	}

	double GetDtS()
	{
		return nDtSec;
	}

	long GetDtMS()
	{
		return nDtMS;
	}
}