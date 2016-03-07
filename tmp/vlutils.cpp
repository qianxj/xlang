#include "vlutils.hpp"
#include <assert.h>

using namespace util;


vlHashString* util::getGlobalHashString()
{
	static vlHashString* sGlobalHashString = 0;
	if(!sGlobalHashString) sGlobalHashString = new vlHashString();
	return sGlobalHashString;
}

int util::hashString(const wstring_t str,int size)
{
	if(size)
		return getGlobalHashString()->hash_token(str,size);
	else
		return getGlobalHashString()->hash_token(str);
}

const wstring_t util::getHashString(int h)
{
	return getGlobalHashString()->get_string(h);
}

wstring_t UnXMLText(wstring_t str)
{
	wstring_t pStr = str;

	pStr = wcschr(pStr,'&');
	bool bEntity = false;
	while(pStr)
	{
		if((pStr[1]=='l' || pStr[1]=='g') && pStr[2]=='t' && pStr[3]==';')
			bEntity = true;
		else if (pStr[1]=='a' && pStr[2]=='m' && pStr[3]=='p' && pStr[4]==';')
			bEntity = true;
		else if (pStr[1]=='a' && pStr[2]=='p' && pStr[3]=='o' && pStr[4]=='s' && pStr[5]==';' )
			bEntity = true;
		else if (pStr[1]=='q' && pStr[2]=='u' && pStr[3]=='o' && pStr[4]=='t' && pStr[5]==';')
			bEntity = true;

		if(bEntity)break;
		pStr = wcschr(++pStr,'&');
	}

	pStr = new wchar_t[wcslen(str) + 1];
	wstring_t pStrEx = pStr;
	while(str[0])
	{
		if(str[0]=='&')
		{
			if(str[1]=='l' && str[2]=='t' && str[3]==';')
			{
				pStr++[0]='<';
				str += 4;
			}else if(str[1]=='g' && str[2]=='t' && str[3]==';')
			{
				pStr++[0]='>';
				str += 4;
			}else if(str[1]=='a' && str[2]=='m' &&  str[3]=='p' && str[4]==';')
			{
				pStr++[0]='&';
				str += 5;
			}else if(str[1]=='a' && str[2]=='p' &&  str[3]=='o' && str[4]=='s' && str[5]==';')
			{
				pStr++[0]='\'';
				str += 6;
			}else if(str[1]=='q' && str[2]=='u' &&  str[3]=='o' && str[4]=='t' && str[5]==';')
			{
				pStr++[0]='"';
				str += 6;
			}else
				pStr++[0] = str++[0];
		}else
			pStr++[0] = str++[0];
	}
	pStr[0] ='\0';

	return pStrEx;
}

wstring_t ToXmlText(wstring_t str)
{
	if(!str) return NULL;

	int nCount = 0;
	int nEntity = 0;
	wstring_t pStr = (wstring_t)str;
	for(;pStr[0]!='\0';pStr++)
	{
		if(pStr[0]=='<'||pStr[0]=='>'||pStr[0]=='\''||pStr[0]=='"'||pStr[0]=='&')
		{
			nEntity++;
		}
		nCount++;
	}
	if(nEntity==0) return NULL;

	pStr =  new wchar_t[nEntity*5 + nCount + 1];
	wstring_t pStrEx = pStr ;
	
	int nPos = 0;
	for(;str[0]!='\0';str++)
	{
		if(str[0]=='&')
		{
			pStr[nPos++]='&';
			pStr[nPos++]='a';
			pStr[nPos++]='m';
			pStr[nPos++]='p';
			pStr[nPos++]=';';
		}
		else if(str[0]=='<')
		{
			pStr[nPos++]='&';
			pStr[nPos++]='l';
			pStr[nPos++]='t';
			pStr[nPos++]=';';
		}
		else if(str[0]=='>')
		{
			pStr[nPos++]='&';
			pStr[nPos++]='g';
			pStr[nPos++]='t';
			pStr[nPos++]=';';
		}
		else if(str[0]=='\'')
		{
			pStr[nPos++]='&';
			pStr[nPos++]='a';
			pStr[nPos++]='p';
			pStr[nPos++]='o';
			pStr[nPos++]='s';
			pStr[nPos++]=';';
		}
		else if(str[0]=='"')
		{
			pStr[nPos++]='&';
			pStr[nPos++]='q';
			pStr[nPos++]='u';
			pStr[nPos++]='o';
			pStr[nPos++]='t';
			pStr[nPos++]=';';
		}
		else
			pStr[nPos++] = str[0];
	}
	pStr[nPos]='\0';
	return pStrEx;
}

void_t FreeXmlText(wstring_t str)
{
	if(str)delete str;
}

bool TrimString(wstring_t lpsz,wstring_t lpFilter)
{
	// 将字符串lpsz的尾部出现过的lpFilter提换为\0
	// Example lpsz="chinese\r\t" lpFilter="\t\r\n"
	// 执行后lpsz="chinese\0\0"
	int i;
	bool trimed = false;
	for(i=(int)::wcslen(lpsz);i>=0;i--)
	{
		if(::wcschr(lpFilter,lpsz[i]))
		{
			lpsz[i]='\0';
			if(!trimed) trimed = true;
		}
		else 
			break;
	}
	// 调整lpsz 使前部分也不出现lpFilter 中的字符
	for(i= 0; i<=(int)::wcslen(lpsz);i++)
	{
		if(!::wcschr(lpFilter,lpsz[i]))break;
	}
	if(i != 0)
	{
		::memcpy(lpsz,lpsz+i,::wcslen(lpsz)*sizeof(wchar_t));
		if(!trimed) trimed = true;
	}
	return trimed;
}


vlDynVector::vlDynVector():data(0),length(0),eleSize(0),growth_factor(2),capacity(0)
{

}

vlDynVector::~vlDynVector()
{

}



unsigned char * vlDynVector::getElement(int Index)
{
	assert(Index>=0);
	assert(Index<length);
	return data + Index*eleSize;
}

unsigned char * vlDynVector::newElemnt(int count)
{
	assert(count > 0 );
	if(length + count >=capacity)
	{
		if(!length)
		{
			capacity = 4 + count;
			data = new unsigned char[eleSize * capacity];
			memset(data,0,eleSize * capacity);
		}else
		{
			if(capacity * this->growth_factor < length + count)
				capacity = length + count + 4;
			else
				capacity *= this->growth_factor;
			unsigned char * temp = new unsigned char[eleSize * capacity];
			memset(temp,0,eleSize * capacity);
			memcpy_s(temp,eleSize * capacity,data,length * eleSize);
			delete data;
			data = temp;
		}
	}
	length += count;
	return data + (length - count)*eleSize;
}

unsigned char * vlDynVector::newElemnt()
{
	if(length==capacity)
	{
		if(!length)
		{
			capacity = 4;
			data = new unsigned char[eleSize * capacity];
			memset(data,0,eleSize * capacity);
		}else
		{
			capacity *= this->growth_factor;
			unsigned char * temp = new unsigned char[eleSize * capacity];
			memset(temp,0,eleSize * capacity);
			memcpy_s(temp,eleSize * capacity,data,length * eleSize);
			delete data;
			data = temp;
		}
	}
	length++;
	return data + (length - 1)*eleSize;
}

void vlDynVector::pop()
{
	if(length)length--;
}



vlTapeChar::vlTapeChar() : length(0)
{ 
	data = new vlDynVector();
	data->setEleSize(1);
}

vlTapeChar::~vlTapeChar()
{
}

void vlTapeChar::Enter(int v)
{
	*(int *)data->newElemnt(sizeof(int)) = v;
	length += sizeof(int);
}
void vlTapeChar::Enter(double v)
{
	*(double *)data->newElemnt(sizeof(double)) = v;
	length += sizeof(double);
}
void vlTapeChar::Enter(wchar_t v)
{
	*(wchar_t *)data->newElemnt(sizeof(wchar_t)) = v;
	length += sizeof(wchar_t);
}
void vlTapeChar::Enter(short v)
{
	*(short *)data->newElemnt(sizeof(short)) = v;
	length += sizeof(short);
}
void vlTapeChar::Enter(char v)
{
	*(char *)data->newElemnt(sizeof(char)) = v;
	length += sizeof(char);
}
void vlTapeChar::Enter(unsigned char data[],int len)
{
	assert(len>0);
	memcpy_s(this->data->newElemnt(len),len,data,len);
	length += len;
}
void vlTapeChar::Enter(vlTapeChar*  tp)
{
	assert(tp);
	if(tp->getLength())
	{
		memcpy_s(data->newElemnt(tp->getLength()),tp->getLength(),tp->getData(),tp->getLength());
		length += tp->getLength();
	}
}

