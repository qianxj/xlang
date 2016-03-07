#include "vllogmessage.hpp"
#include "vlplatform.hpp"

using namespace xlang;

vlLogMessage::vlLogMessage(vlLog* log):log_(log),used_(0)
{

}
vlLogMessage::~vlLogMessage()
{
}

void vlLogMessage::appendString(const wstring_t str)
{
	append(L"%s",str);
}

void vlLogMessage::appendChar(wchar_t ch)
{
	append(L"%c",ch);
}
void vlLogMessage::appendNumber(int_t number)
{
	append(L"%d",number);
}
void vlLogMessage::appendNumber(double_t number)
{
	append(L"%f",number);
}
void vlLogMessage::append(const wstring_t format,...)
{
	va_list args;
	va_start(args, format);
	int len = vswprintf_s((wstring_t)log_->get_buffer() + used_,log_->get_bufferSize() - used_,format,args);
	va_end(args);
	//ASSERT(pos_ <= Log::kMessageBufferSize);
	used_ += len;
}

void vlLogMessage::write()
{
	log_->write(get_buffer(),used_);
	used_ = 0;
}
