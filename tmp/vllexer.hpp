#ifndef WPP5_BSLEXER_H
#define WPP5_BSLEXER_H

#include "vltoken.hpp"
#include <vector>

namespace xlang
{

class vlLexer
{
public:
	vlLexer();
	~vlLexer();
public:
	virtual int setSource(const wstring_t pSrc,wstring_t pUrl=0);
public:
	virtual int insymbol(lexer_flag_t state = 0);
	virtual wchar_t nextch();
public:
	//忽略n个符号
	void skipSym(int n, unsigned int state = 0);
	//pos是相对于当前的位置
	wchar_t getCharAt(int pos);
public:
	int error(unsigned int errcode);
public:
	//cache token
	int save();
	int restore();
	int consume();
	int push();
	int pop();
	int push(vlToken & tk);
	//get item at stacktop - index;
	vlToken & item(int index);
public:
	//forword until find token which type is tk
	int find(int tk);	
public:
	//code souce and len
	wstring_t  source;      
	int sourceLen;

	//token at source postion
	source_position_t srcPosition;
	//lexer  state
	lexer_flag_t lexerState;
	//current token
	vlToken token; 
	//cache token
	std::vector<vlToken> tokenes;
};

} //namespace xlang


#endif //VLEXER_H