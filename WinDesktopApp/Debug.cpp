#include "Debug.h"
#include <vector>
#include <fstream>
#include <chrono>
#include <ctime>
#include <time.h>
#include <restrictederrorinfo.h>
#include <roerrorapi.h>
#include <comdef.h>
#include "Settings.h"
#include "Globals.h"

namespace Debug
{
	const short MAX_LOG_LENGTH = 1000;
}

namespace Debug
{
	static std::vector<CLogMessage> vecFrameLog;
	static FILE* pLogFile;

	void Initialize()
	{
		if(fopen_s(&pLogFile, LOG_FILE_PATH, "w") != 0 || !pLogFile)
		{
			LogNow("Debug::Initialize:[ERROR]: Cannot create log file!");
			return;
		}
	}

	void Shutdown()
	{
		if(pLogFile)
			fclose(pLogFile);
		pLogFile = nullptr;
	}

	static bool LogMsgToFile(CLogMessage& msg)
	{
		if(fwrite(msg.GetStr(), msg.GetSize(), 1, pLogFile) != 1)
		{
			fclose(pLogFile);
			pLogFile = nullptr;
			LogNow("Debug::LogMsgToFile:[ERROR]: Failed to log file on frame end!\n");
			return false;
		}
		return true;
	}

	static void LogMessage(CLogMessage& msg)
	{
		if(msg.m_nLogLevel <= Settings::g_debug.nConsoleLogLevel)
			OutputDebugStringA(msg.GetStr());
		if(pLogFile && msg.m_nLogLevel <= Settings::g_debug.nFileLogLevel)
			LogMsgToFile(msg);
	}

#define PASS_VALIST(func) {	va_list valist;	va_start(valist, lpszFormat);	func;	va_end(valist); }

	void LogNow(const char* lpszFormat, ...)
	{
		PASS_VALIST(Log(0, lpszFormat, valist))
	}

	void LogNow(short nLogLevel, const char* lpszFormat, ...)
	{
		PASS_VALIST(Log(nLogLevel, lpszFormat, valist))
	}

	void Log(short nLogLevel, const char* lpszFormat, va_list& list)
	{
		char szBuff[Debug::MAX_LOG_LENGTH];
		if(vsprintf_s(szBuff, sizeof(szBuff), lpszFormat, list) != -1)
		{
			CLogMessage msg(szBuff, nLogLevel);
			LogMessage(msg);
		}
	}

	void AddToFrameLog(const char* lpszFormat, ...)
	{
		PASS_VALIST(AddToFrameLog(0, lpszFormat, valist))
	}

	void AddToFrameLog(short nLogLevel, const char* lpszFormat, ...)
	{
		PASS_VALIST(AddToFrameLog(nLogLevel, lpszFormat, valist))
	}

	void AddToFrameLog(short nLogLevel, const char* lpszFormat, va_list& list)
	{
		char szBuff[Debug::MAX_LOG_LENGTH];
		if(vsprintf_s(szBuff, sizeof(szBuff), lpszFormat, list) != -1)
			vecFrameLog.emplace_back(szBuff, nLogLevel);
	}

	void OnEndOfFrame()
	{
		for(auto& msg : vecFrameLog)
		{
			if(msg.m_nLogLevel <= Settings::g_debug.nConsoleLogLevel)
				OutputDebugStringA(msg.GetStr());
			if(pLogFile && msg.m_nLogLevel <= Settings::g_debug.nFileLogLevel)
			{
				if(!LogMsgToFile(msg))
					break;
			}
		}
		vecFrameLog.clear();
	}

	static bool bFirstReturn{true};
	std::string GetErrorDesc(HRESULT hResult, bool bFromReturn)
	{
		if(!bFirstReturn && bFromReturn)
			return "";
		return GetErrorDesc(hResult);
	}

	std::string GetErrorDesc(HRESULT hResult)
	{
		IRestrictedErrorInfo* pInfo{nullptr};
		if(::GetRestrictedErrorInfo(&pInfo) == S_OK)
		{
			if(pInfo)
			{
				BSTR bstrErrDesc;
				if(pInfo->GetErrorDetails(&bstrErrDesc, &hResult, nullptr, nullptr) == S_OK)
				{
					if(bstrErrDesc)
					{
						_bstr_t b(bstrErrDesc);
						return (char*)b;
					}
				}
			}
		}

		WCHAR* wlpszError = new WCHAR[Debug::MAX_LOG_LENGTH];
		if(FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, HRESULT_CODE(hResult), 0, wlpszError, Debug::MAX_LOG_LENGTH, nullptr) != 0)
		{
			_bstr_t b(wlpszError);
			delete[] wlpszError;
			return (char*)b;
		}

		delete[] wlpszError;
		return "";
	}



//////////////////////////////////////////////////////////////////////////////
///////  CLogMessage

	static const int nTimeLength = 10;	//"hh:mm:ss: "

	CLogMessage::CLogMessage(const CLogMessage& rh) : CLogMessage(rh.GetStr() + nTimeLength, rh.m_nLogLevel, rh.GetSize() - 1 - nTimeLength)
	{
	}

	CLogMessage::CLogMessage(const char* lpszMsg, short nLogLevel, int nSize)
	{
		m_nLogLevel = nLogLevel;
		CopyLpszMsg(lpszMsg, nSize);
	}

	CLogMessage::CLogMessage(const char* lpszMsg, short nLogLevel /*= 0*/)
	{
		m_nLogLevel = nLogLevel;

		while(m_nSize < Debug::MAX_LOG_LENGTH && lpszMsg[m_nSize] != '\0')
			++m_nSize;

		CopyLpszMsg(lpszMsg, m_nSize);
	}

	CLogMessage::CLogMessage(const std::string& sMsg, short nLogLevel /*= 0*/)
	{
		if(sMsg.empty())
			return;

		m_nLogLevel = nLogLevel;
		CopyLpszMsg(sMsg.c_str(), sMsg.size());
	}

	CLogMessage::~CLogMessage()
	{
		delete[] m_lpszMsg;
	}

	void CLogMessage::CopyLpszMsg(const char* lpszMsg, int nSize)
	{
		if(nSize == 0)
			return;

		m_nSize = nSize + 1 + nTimeLength;

		if(m_nSize > Debug::MAX_LOG_LENGTH)
			m_nSize = Debug::MAX_LOG_LENGTH;

		m_lpszMsg = new char[m_nSize];

		std::time_t time;
		std::time(&time);
		tm time_tm;
		localtime_s(&time_tm, &time);

		std::strftime(m_lpszMsg, m_nSize, "%T: ", &time_tm);

		int nAvailable = m_nSize - nTimeLength;
		memcpy_s(m_lpszMsg + nTimeLength, nAvailable, lpszMsg, nSize > nAvailable ? nAvailable : nSize);
		m_lpszMsg[m_nSize - 1] = '\n';
	}

	const char* CLogMessage::GetStr() const  { return m_lpszMsg; }
	int CLogMessage::GetSize() const { return m_nSize; }
}
