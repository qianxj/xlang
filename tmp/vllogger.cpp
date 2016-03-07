#include "vllogger.hpp"

using namespace xlang;

const wchar_t*  vlLogger::getLoggerItemName(LoggerItem whatItem)
{
	static wchar_t* LoggerItemName[]={L"LOG_STEP",
									L"LOG_COMPLIER",
									L"LOG_FUNCTIONCALL",
									L"LOG_EVENT",
									L"LOG_WARN",
									L"LOG_ERROR",
									L"LOG_RUNSTATE",
									L"LOG_PROFILE",
									L"LOG_CHECK"};
	return LoggerItemName[whatItem];
}