#ifndef WPP5_ERRORSVR_H
#define WPP5_ERRORSVR_H

#include <vector>
#include "vlang5.h"
#include "vlwerr.hpp"

namespace xlang {

class vlErrorSvr : public wErr
{
public:
	struct Item
	{
		const wchar_t* srcFile;
		int srcRow;
		int srcCol;
		int errCode;
		const wchar_t* errText;
	};
	struct Info
	{
		const wchar_t* srcFile;
		const wchar_t* msgText;
	};
public:
	int adderror(const wchar_t* srcFile,int srcRow,int srcCol,int errCode,const wchar_t* errText = 0,int start = 0,int len = 0) override;
	int addwarn(const wchar_t* srcFile,int srcRow,int srcCol,int errCode,const wchar_t* errText = 0,int start = 0,int len = 0) override;
	int print(wchar_t* format,...) override;
	int output(const wchar_t* srcFile,const wchar_t* msgText) override;
public:
	int getErorCount();
	int getWarnCount();
	int getOutputCount();
public:
	std::vector<Item> & getErrors();
	std::vector<Item> & getWarns();
	std::vector<Info> & getInfos();
public:
	int printError(int from);
private:
	std::vector<Item> errors_;
	std::vector<Item> warns_;
	std::vector<Info> infos_;
};


} //// namespace xlang

#endif //WPP5_ERRORSVR_H