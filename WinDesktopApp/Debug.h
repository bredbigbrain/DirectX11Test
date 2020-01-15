#pragma once
#include <Windows.h>
#include <cstdarg>
#include <string>

constexpr const char* str_end(const char *str) {
	return *str ? str_end(str + 1) : str;
}

constexpr bool str_slant(const char *str) {
	return *str == '\\' ? true : (*str ? str_slant(str + 1) : false);
}

constexpr const char* r_slant(const char* str) {
	return *str == '\\' ? (str + 1) : r_slant(str - 1);
}
constexpr const char* file_name(const char* str) {
	return str_slant(str) ? r_slant(str_end(str)) : str;
}

#define __FILENAME__ (file_name(__FILE__))

#define QUIK_LOG() {Debug::LogNow("[QUIK LOG]: file: %s, line: %d", __FILENAME__, __LINE__);}
#define QUIK_LOG_T(TAG) {Debug::LogNow("[%s]: file: %s, line: %d", TAG, __FILENAME__, __LINE__);}
#define QUIK_LOG_M(MESSAGE) {Debug::LogNow("[QUIK LOG]: %s file: %s, line: %d", MESSAGE, __FILENAME__, __LINE__);}
#define QUIK_LOG_ML(MESSAGE, nLevel) {Debug::LogNow(nLevel,"[QUIK LOG]: %s file: %s, line: %d", MESSAGE, __FILENAME__, __LINE__);}
#define QUIK_LOG_TM(TAG, MESSAGE) {Debug::LogNow("[%s]: %s file: %s, line: %d", TAG, MESSAGE, __FILENAME__, __LINE__);}

#define RETURN_AND_LOG(result) { QUIK_LOG(); return result;}
#define RETURN_AND_LOG_ERR(result, hResult) { if(!result){ QUIK_LOG_M(Debug::GetErrorDesc(hResult, true).c_str()); } else QUIK_LOG(); return result;}

#define ON_FAIL_LOG_AND_RETURN(expr) { HRESULT hResult = expr; if(FAILED(hResult)) RETURN_AND_LOG_ERR(false, hResult); }

namespace Debug
{
	void Initialize();
	void Shutdown();
	void Log(short nLogLevel, const char* lpszFormat, va_list& list);
	void LogNow(const char* lpszFormat, ...);
	void LogNow(short nLogLevel, const char* lpszFormat, ...);

	void AddToFrameLog(short nLogLevel, const char* lpszFormat, va_list& list);
	void AddToFrameLog(const char* lpszFormat, ...);
	void AddToFrameLog(short nLogLevel, const char* lpszFormat, ...);

	void OnEndOfFrame();
	std::string GetErrorDesc(HRESULT hResult, bool bFromReturn);
	std::string GetErrorDesc(HRESULT hResult);

	class CLogMessage
	{
	public:
		CLogMessage() = delete;
		CLogMessage(const CLogMessage& rh);
		CLogMessage(const char* lpszMsg, short nLogLevel, int nSize);
		CLogMessage(const char* lpszMsg, short nLogLevel = 0);
		CLogMessage(const std::string& sMsg, short nLogLevel = 0);
		~CLogMessage();

		short m_nLogLevel{0};

	protected:
		char* m_lpszMsg{nullptr};
		int m_nSize{0};

	public:
		const char* GetStr() const;
		int GetSize() const;

	protected:
		void CopyLpszMsg(const char* lpszMsg, int nSize);
	};
}