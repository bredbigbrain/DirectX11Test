#include"Globals.h"
#include<math.h>

namespace Math3DNS
{
	const DirectX::XMFLOAT3 F3_FORWARD{ 0.f, 0.f, 1.f };
	const DirectX::XMFLOAT3 F3_UP{ 0.f, 1.f, 0.f };
	const double RADIANS_PER_DEGREE_d{ 0.0174532925 };
	const float RADIANS_PER_DEGREE_f{ 0.0174532925f };
	constexpr float DEGREES_TO_RADIANS(float fDegrees) { return fDegrees * RADIANS_PER_DEGREE_f; };
	constexpr float RADIANS_TO_DEGREES(float fRadians) { return fRadians / RADIANS_PER_DEGREE_f; };
	
	inline float DegreesToRadians(float fDegrees) { return fDegrees * RADIANS_PER_DEGREE_f; }
	inline float RadiansToDegrees(float fRadians) { return fRadians / RADIANS_PER_DEGREE_f; }

	DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3 & lhs, const DirectX::XMFLOAT3 & rhs)
	{
		return DirectX::XMFLOAT3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	namespace Radians
	{
		inline float Sin(float fRadians)
		{
			return sinf(fRadians);
		}

		inline float Cos(float fRadians)
		{
			return cosf(fRadians);
		}
	}

	namespace Degrees
	{
		inline float Sin(float fDegrees)
		{
			return sinf(DegreesToRadians(fDegrees));
		}

		inline float Cos(float fDegrees)
		{
			return cosf(DegreesToRadians(fDegrees));
		}
	}
}

namespace Settings
{
	const char* SETTINGS_FILE_PATH{ "Info/Settings.bytes" };
	const char* TERRAIN_DATA_FILE_PATH{"Info/TerrainData.txt"};
	const char* SHADER_ERROR_FILE_PATH{"Info/Shader-error.txt"};
	const unsigned int VERSION{0};

	SGraphics_v1 g_graphics;
	SDebug_v1 g_debug;
}

namespace Debug
{
	const char* LOG_FILE_PATH{ "Info/Log.txt" };
}

namespace Input
{
	const int CLICK_TIME_DELTA = 100;
}