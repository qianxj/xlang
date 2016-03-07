#include "vlerrors.hpp"
#include "vlerrorsvr.hpp"
#include "vlplatform.hpp"
using namespace xlang;


int vlErrorSvr::adderror(const wchar_t* srcFile,int srcRow,int srcCol,int errCode,const wchar_t* errText,int start,int len)
{
	Item item;
	item.srcFile = srcFile;
	item.srcRow  = srcRow;
	item.srcCol  = srcCol;
	item.errCode = errCode;
	item.errText = errText;
	errors_.push_back(item);
	return 1;
}

int vlErrorSvr::addwarn(const wchar_t* srcFile,int srcRow,int srcCol,int errCode,const wchar_t* errText,int start,int len)
{
	Item item;
	item.srcFile = srcFile;
	item.srcRow  = srcRow;
	item.srcCol  = srcCol;
	item.errCode = errCode;
	item.errText = errText;
	warns_.push_back(item);
	return 1;
}

int vlErrorSvr::print(wchar_t* format,...)
{
	va_list args;
	va_start(args, format);
	setlocale( LC_ALL, "" );
	vwprintf_s(format,args);
	va_end(args);
	return 1;
}

int vlErrorSvr::output(const wchar_t* srcFile,const wchar_t* msgText)
{
	Info info;
	info.srcFile = srcFile;
	info.msgText = msgText;
	infos_.push_back(info);
	return 1;
}

int vlErrorSvr::getErorCount()
{
	return (int)errors_.size();
}
int vlErrorSvr::getWarnCount()
{
	return (int)warns_.size();
}
int vlErrorSvr::getOutputCount()
{
	return (int)infos_.size();
}

std::vector<vlErrorSvr::Item> & vlErrorSvr::getErrors()
{
	return errors_;
}
std::vector<vlErrorSvr::Item> & vlErrorSvr::getWarns()
{
	return warns_;
}
std::vector<vlErrorSvr::Info> & vlErrorSvr::getInfos()
{
	return infos_;
}

int vlErrorSvr::printError(int from)
{
	for(int i=from;i<(int)errors_.size();i++)
	{
		Item& item = errors_[i];
		//print(L"\r\nError %d %d: 0x%x ",errors_[i].srcRow,errors_[i].srcCol,errors_[i].errCode);
		print(L"\r\nError (%d %d): 0x%x ",errors_[i].srcRow+1,errors_[i].srcCol,errors_[i].errCode);
		wchar_t * str = error::geterrtext((error::err)item.errCode);
		if(wcsstr(str,L"%s"))
			print(str,item.errText);
		else
			print(str);
	}
	return 1;
}
