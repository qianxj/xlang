#ifndef WPP5_WERR_H
#define WPP5_WERR_H

#include "vlang5.h"

namespace xlang {

class VLANG_API wErr
{
public:
	virtual int adderror(const wchar_t* srcFile,int srcRow,int srcCol,int errCode,const wchar_t* errText = 0,int start =0,int len = 0) =0;
	virtual int addwarn(const wchar_t* srcFile,int srcRow,int srcCol,int errCode,const wchar_t* errText = 0,int start = 0,int len = 0) =0;
	virtual int print(wchar_t* format,...) =0;
	virtual int output(const wchar_t* srcFile,const wchar_t* msgText) =0;
};


} //// namespace xlang

#endif //WPP5_WERR_H