#pragma once
#include<DirectXMath.h>

namespace Math3DNS
{
	extern const DirectX::XMFLOAT3 F3_FORWARD;
	extern const DirectX::XMFLOAT3 F3_UP;
	extern const double RADIANS_PER_DEGREE_d;
	extern const float RADIANS_PER_DEGREE_f;

	constexpr float DEGREES_TO_RADIANS(float fDegrees);
	constexpr float RADIANS_TO_DEGREES(float fRadians);

	extern inline float DegreesToRadians(float fDegrees);
	extern inline float RadiansToDegrees(float fRadians);

	DirectX::XMFLOAT3 operator+ (const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
	DirectX::XMFLOAT3 operator- (const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
	DirectX::XMFLOAT3 operator* (const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
	DirectX::XMFLOAT3 operator* (const DirectX::XMFLOAT3& lhs, float fVal);
	bool operator== (const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
	bool operator!= (const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);

	DirectX::XMFLOAT3 ToXMFLOAT3(const DirectX::XMVECTOR& vector);

	namespace Radians
	{
		inline float Sin(float fRadians);
		inline float Cos(float fRadians);
	}

	namespace Degrees
	{
		inline float Sin(float fDegrees);
		inline float Cos(float fDegrees);
	}

	struct MatrixBuffer_t
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};
}

namespace Debug
{
	extern const char* LOG_FILE_PATH;
	extern const short MAX_LOG_LENGTH;
}

namespace Settings
{
	extern const char* SETTINGS_FILE_PATH;
	extern const char* TERRAIN_DATA_FILE_PATH;
	extern const char* SHADER_ERROR_FILE_PATH;
	extern const unsigned int VERSION;

	struct SGraphics_v1
	{
		bool bFullScreen{ false };
		bool bVSync{ false };
		float fScreenDepth{ 1000.f };
		float fScreenNear{ 0.1f };
		double dFramesPerSecond{ 30 };

		double GetFrameDurationMS()
		{
			return 1000. / dFramesPerSecond;
		}
	};

	struct SDebug_v1
	{
		int nConsoleLogLevel{ 10 };
		int nFileLogLevel{ 10 };
	};

	extern SGraphics_v1 g_graphics;
	extern SDebug_v1 g_debug;
}

namespace Input
{
	extern const int CLICK_TIME_DELTA;
}