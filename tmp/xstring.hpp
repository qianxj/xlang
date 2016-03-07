#ifndef WPP5_TYPE_STRING_H
#define WPP5_TYPE_STRING_H

#include "vlang5.h"

namespace xlang
{


struct VLANG_API xstring
{
private:
	wchar_t * v;
public:
	static wchar_t* stringDup(wchar_t* pstr);
	static int stringCmp(wchar_t* pstr1,wchar_t* pstr2);
	static wchar_t* stringCat(wchar_t* pstr1,wchar_t*  pstr2);
	static int find(wchar_t* pstr,wchar_t* pfind);
	static int findex(wchar_t* pstr,wchar_t* pfind,int pos);
	static wchar_t * left(wchar_t* pstr, int len);
	static wchar_t * right(wchar_t* pstr, int len);
	static wchar_t * space(wchar_t* pstr, int len);
	static wchar_t * replace(wchar_t* pstr, wchar_t* from, wchar_t* to,int start = 0);
	static wchar_t * mid(wchar_t* pstr, int pos,int len);
	static int toInt(wchar_t* pstr);
	static double toDouble(wchar_t* pstr);
	static wchar_t* lower(wchar_t* pstr);
	static wchar_t* trim(wchar_t* pstr);
	static wchar_t* ltrim(wchar_t* pstr);
	static wchar_t* rtrim(wchar_t* pstr);
	static wchar_t* upper(wchar_t* pstr);
	static wchar_t at(wchar_t* pstr, int pos);
	static int length(wchar_t* pstr);
	static wchar_t* castFromNumber(int number,int radix);
	static wchar_t* castFromDouble(double number,int round );
};

}//xlang

#endif //WPP5_TYPE_STRING_H