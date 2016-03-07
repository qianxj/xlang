#ifndef WPP5_TOKEN_H
#define WPP5_TOKEN_H

#include "vlglobal.hpp"
#include <assert.h>

namespace xlang
{

typedef unsigned int token_type_t;
struct tokenType
{
	static const unsigned int tOp			= 0x421;
	static const unsigned int tString		= 0x422;
	static const unsigned int tChar			= 0x423;
	static const unsigned int tLn			= 0x424;
	static const unsigned int tIdent		= 0x425;
	static const unsigned int tInt			= 0x426;
	static const unsigned int tDouble		= 0x427;
	static const unsigned int tComment		= 0x428;
	static const unsigned int tBlockComment	= 0x429;
	static const unsigned int tNumber		= 0x42A;
	static const unsigned int tHex			= 0x42B;
	static const unsigned int tBlank		= 0x42C;
	static const unsigned int tBool			= 0x42D;
};

struct lexerFlag
{
	static const unsigned int tLn		= 0x0001;
	static const unsigned int tIdentdot	= 0x0002;
	static const unsigned int tIdentsub	= 0x0004;
	static const unsigned int tBnf		= 0x0008;
	static const unsigned int tBlank	= 0x0010;
	static const unsigned int tComment	= 0x0020;
	static const unsigned int tEscape	= 0x0040;
	static const unsigned int tXPath	= 0x0080;
	static const unsigned int tXQuery	= 0x0100;
	static const unsigned int tXmltext	= 0x0200;
} ;
typedef unsigned int lexer_flag_t ;
struct source_position_t
{
	//pointer source code file name or url name;
	wstring_t srcUrl;
	//insymbol at row
	int curRow;
	//insymbol at col
	int curCol;
}; 

struct opValue
{
	typedef int  valuetype;
	static const valuetype undef	= 0x00;
	static const valuetype add		= 0x01;
	static const valuetype sub		= 0x02;
	static const valuetype mul		= 0x03;
	static const valuetype div		= 0x04;
	static const valuetype shl		= 0x05;
	static const valuetype shr		= 0x06;
	static const valuetype asn		= 0x07;
	static const valuetype mod		= 0x08;
	static const valuetype inc		= 0x09;
	static const valuetype dec		= 0x10;
	static const valuetype or		= 0x12;
	static const valuetype and		= 0x13;
	static const valuetype not		= 0x14;
	static const valuetype exclam   = 0x14;
	static const valuetype band		= 0x15;
	static const valuetype bor		= 0x16;
	static const valuetype bnot		= 0x17;
	static const valuetype xor		= 0x18;
	static const valuetype eq		= 0x20;
	static const valuetype neq		= 0x21;
	static const valuetype gt		= 0x22;
	static const valuetype gte		= 0x23;
	static const valuetype lt		= 0x24;
	static const valuetype lte		= 0x25;
	static const valuetype rparen	= 0x26;
	static const valuetype lparen	= 0x27;
	static const valuetype rbrace	= 0x28;
	static const valuetype lbrace	= 0x29;
	static const valuetype rbrackets= 0x2A;
	static const valuetype lbrackets= 0x2B;
	static const valuetype comma	= 0x2C;
	static const valuetype semicolon= 0x2D;
	static const valuetype dot		= 0x2E;
	static const valuetype period	= 0x2E;
	static const valuetype ref		= 0x2E;
	static const valuetype colon	= 0x2F;
	static const valuetype hash		= 0x30;
	static const valuetype becomes  = 0x31;
	static const valuetype query	= 0x32;
	static const valuetype sign		= 0x33;
	static const valuetype backslash= 0x34;
	static const valuetype dollar	= 0x35;
	static const valuetype apos		= 0x36;
	static const valuetype quotes	= 0x37;
	static const valuetype nest		= 0x38;
	static const valuetype percent  = 0x39;
	static const valuetype pointer  = 0x3A;
	static const valuetype dot3	    = 0x3B;
	static const valuetype dot2	    = 0x3C;
	static const valuetype ln	    = 0x3D;
	static const valuetype mref		= 0x3E;
	static const valuetype mpointer = 0x3F;
	static const valuetype varrow   = 0x40;
	
	static const valuetype path		= div;
	static const valuetype path2	= 0x41;
	static const valuetype arrow	= 0x42;
	static const valuetype sizeof_	= 0x43;
	static const valuetype eof		= 0x44;

	static const valuetype _asn		= 0x80;

	static valuetype getOperateByToken(wchar_t ch);
	static valuetype getOperateByString(wchar_t* str);
	static wchar_t * getOperateText(int op);
};

struct vlToken
{
	//next char;
	wchar_t ch;
	//index at source
	int cc;
	//type of token
	int ty; 
	//token value
	union
	{
		int		inum; //int value
		double	dnum; //double value
	};
	const wchar_t* snum; //keyword, ident or string value
	
	int nlen;
	int pcc;
	int slen;

	struct
	{
		int row;
		int col;
		int len;
	} start;

	int sindex;

	int row;
	int col;

	inline void setOperate(int op)
	{
		ty = tokenType::tOp;
		inum = op;
	}
};

}//namespace xlang

#endif //WPP5_TOKEN_H
