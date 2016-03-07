#include "xstring.hpp"
#include <tchar.h>
#include <vector>
#include "vlplatform.hpp"


using namespace xlang;
using namespace std;

wchar_t* xstring::stringDup(wchar_t* pstr)
{
	if(!pstr) return 0;
	return _wcsdup(pstr);
}

int xstring::stringCmp(wchar_t* pstr1,wchar_t* pstr2)
{
	if((!pstr1||!pstr1[0]) && (!pstr2||!pstr2[0])) return 0;
	if(!pstr1||!pstr1[0]) return 1;
	if(!pstr2||!pstr2[0]) return -1;
	return wcscmp(pstr1,pstr2);
}

wchar_t* xstring::stringCat(wchar_t* pstr1,wchar_t*  pstr2)
{
	if((!pstr1||!pstr1[0]) && (!pstr2||!pstr2[0])) return 0;
	if(!pstr1||!pstr1[0]) return _wcsdup(pstr2);
	if(!pstr2||!pstr2[0]) return _wcsdup(pstr1);
	int nlen1 = (int)wcslen(pstr1);
	int nlen2 = (int)wcslen(pstr2);
	wchar_t* pstr = new wchar_t[nlen1 + nlen2 + 1];
	wcscpy_s(pstr,nlen1 + 1,pstr1);
	wcscpy_s(pstr + nlen1,nlen2 + 1,pstr2);
	return pstr;
}

int xstring::find(wchar_t* pstr,wchar_t* pfind   )
{
	return findex(pstr,pfind,0);
}

int xstring::findex(wchar_t* pstr,wchar_t* pfind,int pos)
{
	if(!pstr) return -1;
	wchar_t* p = wcsstr(pstr + pos,   pfind);
	if(p) 
		return (int)(p - pstr);
	else
		return -1;
}

wchar_t * xstring::left(wchar_t* pstr, int len)
{
	if(!pstr) return 0;
	if(len<1) return 0;
	if(len > (int)wcslen(pstr)) len = (int)wcslen(pstr);
	wchar_t * p = new  wchar_t[len +1];
	p[len] = 0;
	wcsncpy_s(p,len+1,pstr,len);
	return p;
}

wchar_t * xstring::right(wchar_t* pstr, int len)
{
	if(!pstr) return 0;
	if(len<1) return 0;
	if(len >= (int)wcslen(pstr))len =   (int)wcslen(pstr);
	wchar_t * p = new  wchar_t[len + 1];
	p[len] = 0;
	wcsncpy_s(p,len+1,pstr + wcslen(pstr) - len, len);
	return p;
}

wchar_t * xstring::space(wchar_t* pstr, int len)
{
	wchar_t * p = new  wchar_t[len + 1];
	int i=0;
	while(i<len)p[i++]=' ';
	p[i] = 0;
	return p;
}

wchar_t * xstring::replace(wchar_t* pstr, wchar_t* from, wchar_t* to,int pos)
{
	if(!pstr) return 0;
	if(!from[0]) return pstr;
	if(pos<1) pos = 0;

	int len = (int)wcslen(from);
	if(len < 0) return pstr;

	vector<int> arpos;
	wchar_t* ptext = pstr;
	ptext = wcsstr(ptext + pos,from);
	if(!ptext) return pstr;
	while(ptext)
	{
		arpos.push_back((int)(ptext - pstr));
		ptext = wcsstr(ptext + len ,from);
	}

	int tolen = (int)wcslen(to);
	int inc = ((int)arpos.size())*(tolen - len);

	int nlen = (int)wcslen(pstr) + inc + 1;
	int start = 0;
	int end = 0;

	wchar_t * p = new  wchar_t[nlen];
	memset(p,0,nlen*2);
	wchar_t * np = p;
	arpos.push_back((int)wcslen(pstr));
	for(int i=0;i<(int)arpos.size();i++)
	{
		end = arpos[i];
		if(end - start > 0)
		{
			wcsncpy_s(np,end - start + 1,pstr + start, end - start);
			np += end - start ;
		}
		if(i!=(int)arpos.size() - 1)
		{
			if(tolen) 
			{
				wcsncpy_s(np,tolen + 1,to,tolen);
				np += tolen;
			}
		}
		start = end + len;
	}
	return p;
}

wchar_t * xstring::mid(wchar_t* pstr, int pos,int len)
{
	if(!pstr) return 0;
	if(pos<1) pos = 0;
	if(len < 1) return 0;
	if(pos + len > (int)wcslen(pstr)) len = (int)wcslen(pstr) - pos;
	if(len < 1) return 0;
	wchar_t * p = new  wchar_t[len +1];
	p[len]=0;
	wcsncpy_s(p,len+1,pstr+pos,len);
	return p;
}

int xstring::toInt(wchar_t* pstr)
{
	if(!pstr) return 0;
	return _wtoi(pstr);
}

double xstring::toDouble(wchar_t* pstr)
{
	if(!pstr) return 0.0;
	//__asm emms;
	double d = _wtof(pstr);
	//_mm_load_sd(&d);
	return d;
}

wchar_t* xstring::lower(wchar_t* pstr)
{
	if(!pstr)return 0;
	wchar_t *  p =_wcsdup(pstr);
	_wcslwr(p);
	return p;
}

wchar_t* xstring::trim(wchar_t* pstr)
{
	if(!pstr)return 0;
	wchar_t *  p =pstr;
	while(p && p[0]==' '||p[0]=='\t'|| p[0]=='\n' )p++;
	p = _wcsdup(p);
	int nlen = (int)wcslen(p);
	while(nlen > 1 && p[nlen - 1]==' '|| p[nlen - 1]=='\t'|| p[nlen - 1]=='\n' )nlen --;
	p[nlen] = 0;
	return p;
}

wchar_t* xstring::ltrim(wchar_t* pstr)
{
	if(!pstr)return 0;
	wchar_t *  p =pstr;
	while(p && p[0]==' '||p[0]=='\t'|| p[0]=='\n' )p++;
	return _wcsdup(p);;
}

wchar_t* xstring::rtrim(wchar_t* pstr)
{
	if(!pstr)return 0;
	wchar_t* p = _wcsdup(pstr);
	int nlen = (int)wcslen(p);
	while(nlen > 1 && p[nlen - 1]==' '|| p[nlen - 1]=='\t'|| p[nlen - 1]=='\n' )nlen --;
	p[nlen] = 0;
	return p;
}

wchar_t* xstring::upper(wchar_t* pstr)
{
	if(!pstr)return 0;
	wchar_t *  p =_wcsdup(pstr);
	_wcsupr(p);
	return p;
}

wchar_t xstring::at(wchar_t* pstr, int pos)
{
	if(!pstr) return 0;
	if(pos<0) return 0;
	if(pos>(int)wcslen(pstr))return 0;
	return   pstr[pos];
}

int xstring::length(wchar_t* pstr)
{
	if(!pstr) return  0;
	int i = 0;
	while(pstr[i])i++;
	return i;
}

wchar_t* xstring::castFromNumber(int number,int radix)
{
	wchar_t* buf = new wchar_t[48];
	_ltow_s(number,buf,48,radix);
	return buf;
}

wchar_t* xstring::castFromDouble(double number,int round )
{
	wchar_t* buf = new wchar_t[48];
	wchar_t format[48];
	swprintf_s(format,48,L"%s%df",L"%",round);
	swprintf_s(buf,48,format,number);
	return buf;
}