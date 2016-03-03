#ifndef XLLEXER_H
#define XLLEXER_H

#include <assert.h>
#include "xltoken.hpp"
#include "xllexerutil.hpp"

namespace xl {

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
		CharType  nextch();
		int insymbol(unsigned int state = 0);
	public:
		Token<CharType> token;
		//source index
		int cc;

	private:
		CharType * source;
		CharType * sourceUrl;
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
	token.cc = -1;
	srcRow = 0;
	srcCol = 0;
	
	return 0;
}

template<class CharType>
CharType  Lexer<CharType>::nextch()
{
	if(!source[token.cc]) return 0;

	token.cc ++;
	if(token.ch == '\n') 
	{
		srcRow++;
		srcCol = 1;
		token.ch = source[token.cc];
	} else  if(token.ch == '\r') 
	{
		if(source[token.cc + 1] == '\n') token.cc++;   //pos shift right
		token.ch = '\n';
	}
	srcCol ++;
	return   token.ch;
}
	
} //namespace xl

#endif //XLLEXER_H
