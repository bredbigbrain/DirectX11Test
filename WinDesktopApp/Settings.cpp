#include"Settings.h"
#include"Debug.h"
#include"Globals.h"

namespace Settings
{
	void CSettings::Initialize()
	{
		if(!Load(Settings::SETTINGS_FILE_PATH))
			Debug::LogNow("CSettings::Initialize: failed to load settings!");
	}

	bool CSettings::Load(const char* szPath)
	{
		FILE* pFile;

		if(strcpy_s(szSettingsFilePath, szPath) != 0)
			RETURN_AND_LOG(false);

		if(fopen_s(&pFile, szPath, "rb") != 0)
		{
			Debug::LogNow("CSettings::Load: No settings file at: %s", szPath);
			return false;
		}

		unsigned int nVersion{0};
		if(!Read(nVersion, pFile))
			RETURN_AND_LOG(false);

		if(!Read(g_graphics, pFile))
			RETURN_AND_LOG(false);

		fclose(pFile);
		return true;
	}

	void CSettings::Shutdown()
	{
		if(!Save())
			Debug::LogNow("CSettings::Shutdown: saving failed!");
	}

	bool CSettings::Save()
	{
		if(szSettingsFilePath[0] == '\0')
		{
			Debug::LogNow("CSettings::Save: Settings file path is empty!");
			return false;
		}

		FILE* pFile;

		if(fopen_s(&pFile, szSettingsFilePath, "wb") != 0)
		{
			Debug::LogNow("CSettings::Save: Error creating settings file at: %s", szSettingsFilePath);
			return false;
		}

		if(!Write(VERSION, pFile))
			RETURN_AND_LOG(false);

		if(!Write(g_graphics, pFile))
			RETURN_AND_LOG(false);

		fclose(pFile);
		return true;
	}
}