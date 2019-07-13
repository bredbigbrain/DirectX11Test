#pragma once
#include"stdafx.h"

namespace Settings
{
	class CSettings
	{
	public:
		char szSettingsFilePath[MAX_PATH]{0};

		void Initialize();
		void Shutdown();
		bool Load(const char* szPath);
		bool Save();
	};

	template<class T>
	bool Read(T& value, FILE* pFile, size_t nCount = 1)
	{
		if(fread(&value, sizeof(T), nCount, pFile) != nCount)
		{
			Debug::LogNow("Error reading file!");
			fclose(pFile);
			return false;
		}
		return true;
	}

	template<class T>
	bool Write(const T& value, FILE* pFile, size_t nCount = 1)
	{
		if(fwrite(&value, sizeof(T), nCount, pFile) != nCount)
		{
			Debug::LogNow("Error writing to file!");
			fclose(pFile);
			return false;
		}
		return true;
	}
}