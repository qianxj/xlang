#include <stdio.h>
#include <string.h>
#include <string>
#include "xlvalue.hpp"

using namespace xl::util::misc;

std::wstring TValue::toString()
{
	wchar_t buf[128];
	switch(vt)
	{
		case Unknown:
			return L"";
		case Int64:
			_snwprintf(buf,128,L"%lld",val_i64); //linux "%dlld"
			return buf;
		case UInt64:
			_snwprintf(buf,128,L"%llu",val_ui64); //linux "%dllu"
			return buf;
		case Int32:
			_snwprintf(buf,128,L"%d",val_i32);
			return buf;
		case UInt32:
			_snwprintf(buf,128,L"%u",val_ui32);
			return buf;
		case Int16:
			_snwprintf(buf,128,L"%d",val_i16);
			return buf;
		case UInt16:
			_snwprintf(buf,128,L"%u",val_ui16);
			return buf;
		case Int8:
			_snwprintf(buf,128,L"%d",val_i8);
			return buf;
		case UInt8:
			_snwprintf(buf,128,L"%u",val_ui8);
			return buf;
		case Double80:
			_snwprintf(buf,128,L"%f",val_d64);
			return buf;
		case Double64:
			_snwprintf(buf,128,L"%f",val_d64);
			return buf;
		case Double32:
			_snwprintf(buf,128,L"%f",val_d32);
			return buf;
		case WString:
			return val_wstr;
		case String:
		{
			std::wstring result;
			setlocale( LC_ALL, "" );
			int size = mbstowcs(0,val_str,0);
			if(size < 126)
			{
				mbstowcs(buf,val_str,strlen(val_str)+1);
				result = buf;
			}else
			{
				wchar_t* wstr = new wchar_t[size + 1];
				mbstowcs(wstr,val_str,strlen(val_str)+1);
				result = wstr;
				delete wstr;
			}
			return result;
		}
		case Bool:
			return val_bool ? L"true":L"false";
		case Ptr:
			_snwprintf(buf,255,L"0x%p",val_ptr);
			return buf;
		case WChar:
			_snwprintf(buf,128,L"%c",val_i16);
			return buf;
		case Char:
			_snwprintf(buf,128,L"%C",val_i8);
			return buf;
		case UChar:
			_snwprintf(buf,128,L"%C",val_ui8);
			return buf;
		default:
			return L"$error";
	}
	
}

std::wstring TValue::toStringEx()
{
	std::wstring str = toString();
	switch(vt)
	{
		case WString:
		case String:
			return L"\"" + str + L"\"";
		case WChar:
		case Char:
		case UChar:
			return L"\'" + str + L"\'";
		default:
			return str;
	}
}
