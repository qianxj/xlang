#ifndef WPP5_LOG_H
#define WPP5_LOG_H

#include "vlang5.h"
#include "vlglobal.hpp"

#include "vllogger.hpp"

namespace xlang
{

	class VLANG_API vlLog
	{
	public:
		vlLog();
		~vlLog();
	public:
		const string_t get_defualtLogFile()const;
		bool_t openStdout();
		bool_t openFile(const string_t pFile);
		bool_t openFile();
		void_t closeFile();
	public:
		int_t write(const string_t  msg, int length);
		int_t write(const wstring_t msg, int length);

	public:
		vlLogger * get_logger();
		void_t  set_logger(vlLogger * logger);
	private:
		vlLogger * logger_;
		FILE* output_handle_;
		wchar_t buffer_[4096];
	public:
		const wstring_t get_buffer();
		const int_t get_bufferSize();
	public:
		static vlLog *  get_Log();
	};

}


#endif WPP5_LOG_H