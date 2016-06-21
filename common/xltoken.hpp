#ifndef XLTOKEN_H
#define XLTOKEN_H

#include <string>
#include "xltokenutil.hpp"

using namespace xl::util::token;

namespace xl
{

struct SrcPos
{
	int row;
	int col;
};

template<class CharType >
struct FToken
{
	//token kind
	TokenKind kind;
	
	//token len
	int tokenLen; 
	//string token len;
	int strLen;
	
	union  
	{
		int 		intVal;
		double 	doubleVal;
		bool 		boolVal;
		CharType	charVal;
		Operate		opVal;
	};
	
	const CharType* 	strVal;	
};

//typedef char CharType;
template<class CharType >
struct Token : public FToken<CharType>
{
	//prev source index
	int prevcc;
	//token start index
	int startcc;
	
	//method
	bool IsOperate(Operate op)		{	return isOperate(kind) &&  opVal == op;	}
	bool IsOperate()		{	return isOperate(kind); }
	bool IsIdent()				{	return isIdent(kind);	}
	bool IsString()				{	return isString(kind);	}
	bool IsBool()				{	return isBool(kind);		}
	bool IsChar(CharType ch)		{	return isChar(kind) && strVal[0] == ch;	}
	bool IsChar()				{	return isChar(kind);	}
	bool IsInt()				{	return isInt(kind);	}
	bool IsDouble()				{	return isDouble(kind);	}
	bool IsFloat()				{	return isFloat(kind);	}
	bool IsNumber()				{	return isNumber(kind); }
	bool IsLiteral()			{	return isLiteral(kind);	}
	bool IsEof()				{	return isEof(kind);	}
	bool IsComment()			{	return isComment(kind);	}
	bool IsLn()				{	return isLn(kind);	}
	bool IsBlank()				{	return isBlank(kind);	}

	bool IsIdent(CharType* Ident)
	{
		if(!IsIdent()) return false;
		int pos = 0;
		while(Ident[pos] == strVal[pos] && pos<=strLen)pos++; 
		return !Ident[pos] && pos == strLen ;
	}
	
	bool IsString(CharType* str)
	{
		if(!IsString()) return false;
		int pos = 0;
		while(str[pos] == strVal[pos] && pos<=strLen)pos++; 
		return !str[pos] && pos == strLen ;
	}
	
	std::wstring  toString()
	{
		Token * tk = this; 
		wchar_t buf[255] = {0};
		if(tk->IsOperate())
		{
			_snwprintf(buf,254,L"%s", getOperateText(opVal));
			return buf;
		}
		switch(tk->kind)  
		{
			case tkUnknown:
				return L"unkown"; 
			case tkKeyword:
			{ 
				//!!error
				wchar_t* str = (wchar_t*)tk->strVal;
				int pos = 0;
				while(pos < tk->strLen && pos<252 )buf[pos++]=*str++;
				return buf;
			}
			case tkString:
			{
				wchar_t* str = (wchar_t*)tk->strVal;
				buf[0] = '\"';
				int pos = 1;
				while(pos - 1 < tk->strLen && pos<252)buf[pos++]=*str++;
				buf[pos++] = '\"';
				buf[pos] = '\0';
				return buf;
			}
			case tkChar:
			{ 
				wchar_t* str = (wchar_t*)tk->strVal;
				buf[0] = '\'';
				int pos = 1;
				while(pos - 1 < tk->strLen && pos<252)buf[pos++]=*str++;
				buf[pos++] = '\'';
				buf[pos] = '\0';
				return buf;
			}
			case tkInt:
				_snwprintf(buf,254,L"%d",tk->intVal);  
				return buf;
			case tkDouble:
				_snwprintf(buf,254,L"%f",tk->doubleVal);  
				return buf;
			case tkIdent:
			{
				wchar_t* str = (wchar_t*)tk->strVal;
				int pos = 0;
				while(pos  < tk->strLen && pos<252)buf[pos++]=*str++;
				return buf;
			}
			case tkBool:
				return tk->intVal?L"true":L"false";
			default:
				return L"it?"; 
		}
		return L"it?"; 
	}
};


} //namespace xl

#endif  //XLTOKEN_H
