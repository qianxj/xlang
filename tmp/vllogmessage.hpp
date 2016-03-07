#ifndef WPP5_VLOGMESSAGE_T_H
#define WPP5_VLOGMESSAGE_T_H

#include "vlang5.h"
#include "vllog.hpp"

namespace xlang {

	class VLANG_API vlLogMessage
	{
	public:
		vlLogMessage(vlLog* log);
		~vlLogMessage();
	public:
		void appendString(const wstring_t str);
		void appendChar(wchar_t ch);
		void appendNumber(int_t number);
		void appendNumber(double_t number);
		void append(const wstring_t format,...);
	public:
		void write();
	public:
		inline const wstring_t get_buffer()const {return log_->get_buffer();}
		inline int get_bufferUsed()const {return used_;}
		inline int get_bufferSize()const {return log_->get_bufferSize();}
	public:
		vlLog * log_;
		int		used_;
	};
}

#endif //WPP5_vlogmessage_t_H