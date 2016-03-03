#ifndef XLTOKEN_H
#define XLTOKEN_H

#include "xltokenutil.hpp"
using namespace xl::util::token;

namespace xl
{

struct SrcPos
{
	int row;
	int col;
};

//typedef char CharType;
template<class CharType >
struct Token
{
	//token kind
	TokenKind kind;
	
	//prev source index
	int prevcc;
	//token start index
	int startcc;
	
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
	
	CharType* 	strVal;
	
	//method
	bool IsOperate(Operate op)		{	return isOperate(kind) &&  opVal == op;	}
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
		return !Ident[pos] && pos == strLen + 1;
	}
	
	bool IsString(CharType* str)
	{
		if(!IsString()) return false;
		int pos = 0;
		while(str[pos] == strVal[pos] && pos<=strLen)pos++; 
		return !str[pos] && pos == strLen + 1;
	}
};


} //namespace xl

#endif  //XLTOKEN_H
