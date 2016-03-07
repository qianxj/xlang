#include "vllog.hpp"

using namespace xlang;

vlLog::vlLog(): output_handle_(0)
{
	openFile();
}

vlLog::~vlLog()
{
	if(output_handle_) closeFile();
}

bool vlLog::openStdout()
{
	output_handle_ = stdout;
	return true;
}

bool vlLog::openFile(const string_t pFile)
{
	 fopen_s(&output_handle_,pFile,"a+b");
	if(output_handle_ <= 0) return false; 

	fpos_t  pos;
	fgetpos(output_handle_,&pos);
	unsigned char buf[]={0xff,0xfe};
	if(pos == 0)fwrite(buf,1,2,output_handle_);
	return true;
}

bool vlLog::openFile()
{
	return openFile(get_defualtLogFile());
}

int vlLog::write(const wstring_t msg, int length)
{
	//ASSERT(output_handle_ != NULL);
	size_t rv = fwrite(msg, 2, length, output_handle_);
	//ASSERT(static_cast<size_t>(length) == rv);
	//USE(rv);
	fflush(output_handle_);
	return length;
}

int vlLog::write(const string_t msg, int length) {
	//ASSERT(output_handle_ != NULL);
	size_t rv = fwrite(msg, 1, length, output_handle_);
	//ASSERT(static_cast<size_t>(length) == rv);
	//USE(rv);
	fflush(output_handle_);
	return length;
}

void  vlLog::closeFile()
{
	fclose(output_handle_);
}

const string_t vlLog::get_defualtLogFile()const
{
	return "wpp.log";
}

vlLog *  vlLog::get_Log()
{
	static vlLog log_;
	return & log_;
}

vlLogger * vlLog::get_logger()
{
	return logger_;
}

void_t  vlLog::set_logger(vlLogger * logger)
{
	logger_ = logger;
}

const wstring_t vlLog::get_buffer()
{
	return buffer_;
}

const int_t vlLog::get_bufferSize()
{
	return sizeof(buffer_)/sizeof(buffer_[0]);
}
