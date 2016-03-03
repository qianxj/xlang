#ifndef XLLEXER_H
#define XLLEXER_H

#include <assert.h>
#include "xltoken.hpp"
#include "xllexerutil.hpp"

namespace xl {
	
//using namespace util::token;
	
struct LexerFlag
{
	static const unsigned int tLn			= 0x0001;
	static const unsigned int tIdentdot	= 0x0002;
	static const unsigned int tIdentsub	= 0x0004;
	static const unsigned int tBnf			= 0x0008;
	static const unsigned int tBlank		= 0x0010;
	static const unsigned int tComment	= 0x0020;
	static const unsigned int tEscape	= 0x0040;
	static const unsigned int tXPath		= 0x0080;
	static const unsigned int tXQuery	= 0x0100;
	static const unsigned int tXmltext	= 0x0200;
} ;

template<class CharType>
class Lexer
{
	public:
		int SetSource(const CharType* src,CharType * srcUrl=0);		
		Token<CharType> & insymbol(/*unsigned int state = 0*/);
	private:
		void fillToken(TokenKind kind, int len);
	public:
		Token<CharType> token;
		//source index
		int cc;

	private:
		const CharType * source;
		const CharType * sourceUrl;
	private:
		int srcRow;
		int srcCol;
	private:
		unsigned int lexerState;
};

template<class CharType>
int Lexer<CharType>::SetSource(const CharType* src,CharType * srcUrl)
{
	source = src;
	sourceUrl = srcUrl;
	cc = 0;
	srcRow = 1;
	srcCol = 0;
	token.kind=kUnknown;
	
	return 0;
}

template<class CharType>
void  Lexer<CharType>::fillToken(TokenKind kind, int len)
{
	if(len < 1) return ;
	token.kind = kind;
	token.startcc=cc;
	token.tokenLen = len;
	token.strVal = source+cc;
	token.strLen = len;
	if(kind==kChar||kind==kString)
	{
		token.strVal++;
		token.strLen -= 2;
	}
}

template<class CharType>
Token<CharType> &  Lexer<CharType>::insymbol()
{
	//set start pos
	token.prevcc = cc;
	int len = 0;
	while(true)
	{
		len = eatBlank(source+cc,srcRow,srcCol);
		if(len) cc+=len;
		len = eatCppComment(source+cc,srcRow,srcCol);
		if(!len) break;
		cc+=len;
	}
	if(!cc)
	{
		token.kind =kEof;
		return token;
	}
	
	token.startcc = cc;
	
	//eat number
	if(isDigit(source[cc]))
	{
		Number number;
		len = eatNumber(source+cc, number);
		token.kind = number.kind == Number::kInt ? kInt : kDouble;
		if(token.kind = kInt)
			token.intVal = number.intPart;
		else
			token.doubleVal = sizeof(CharType)==2?_wtof((wchar_t*)source):atof((char*)source);
		fillToken(token.kind, len);
		cc+=len;
		return token;
	}
	
	//eat string or char
	if(source[cc]=='\'' || source[cc]=='\"')
	{
		len = eatString(source+cc);
		fillToken(source[cc]=='\'' ? kChar : kString, len);
		cc+=len;
		return token;
	}
	
	//eat ident
	len = eatIdent(source+cc);
	if(len > 0)
	{
		fillToken(kIdent,len);
		cc += len;
		return token;
	}

	//eat operate
	Operate op;
	len = eatCppOperate(source+cc,op);
	if(len > 0)
	{
		token.opVal = op;
		fillToken(kOperate,len);
		cc += len;
		return token;
	}
	
	//has error
	token.kind = kUnknown;
	cc++; //skip one char
	
	return token;
}


	
} //namespace xl

#endif //XLLEXER_H
