#ifndef XLLEXERUTIL_H
#define XLLEXERUTIL_H

#include "xltoken.hpp"


namespace xl {
namespace util { namespace lexer{
	
//declare
template <class CharType> struct OperateTerm;
struct Number;	
template <class CharType> 
	OperateTerm<CharType>& OpTerm(CharType ch, util::token::Operate op);
template<class CharType> bool  isLetter(CharType ch) ;
template<class CharType> bool isDigit(CharType ch);
template<class CharType> bool isBlankLetter(CharType ch);
template<class CharType> bool isBoolConst();
template<class CharType> int eatNumber(const CharType * src, Number &val);
template<class CharType> int eatChar(const CharType* src);
template<class CharType> int eatCppOperate(const CharType* src);
template<class CharType> int eatCppComment(const CharType* src);
template<class CharType> int eatXQueryComment(const CharType* src);
template<class CharType> int eatIdent(const CharType* src);
	
//define	
template <class CharType>
struct OperateTerm
{
	CharType ch;
	OperateTerm * next;
	OperateTerm * child;
	OperateTerm * parent;
	Operate op;
	
	OperateTerm(CharType ch, util::token::Operate op) : next(0),child(0),parent(0),ch(ch),op(op){}
	OperateTerm & operator + (OperateTerm & rhs)
	{
		OperateTerm * term = this;
		while(term->next) term=term->next;
		term->next = &rhs;
		rhs.parent = parent;
		return *this;
	}
	OperateTerm & operator * (OperateTerm & rhs)
	{
		OperateTerm * term = this->child;
		if(!term)
		{
			child = &rhs;
			rhs.parent = this;
		}else
		{
			while(term->next) term=term->next;
			term->next = &rhs;
			rhs.parent = this;
		}
		return *this;
	}
	OperateTerm & operator += (OperateTerm & rhs)
	{
		return *this + rhs;
	}
	OperateTerm & operator *= (OperateTerm & rhs)
	{
		return *this * rhs;
	}
};

template <class CharType>
OperateTerm<CharType>& OpTerm(CharType ch, util::token::Operate op)
{
	return *(new OperateTerm<CharType>(ch,op));
}

struct Number
{
	enum Kind : char
	{
		kUnkown,
		kInt,
		kDouble
	};
	
	Kind kind;
	int intPart;
	int mantissa;
	int mantissaScale;
	int scale;
	
	Number() : kind(kInt),intPart(0),mantissa(0),mantissaScale(0),scale(0){}
	Number(int val) : kind(kInt),intPart(val),mantissa(0),mantissaScale(0),scale(0){}
	void Zero()
	{
		kind=kInt;
		intPart = 0;
		mantissa = 0;
		mantissaScale = 0;
		scale = 0;
	}
};
	
template<class CharType> bool  isLetter(CharType ch) 
	{return (ch>='a' && ch<='z')||(ch>='A' && ch<='Z') || ch>255;}

template<class CharType> bool isDigit(CharType ch)
	{ return ch>='0' && ch<='9'; }
	
template<class CharType> bool isBlankLetter(CharType ch)
	{ return ch=='\t' || ch=='\r' ||ch=='\n' || ch==' ';	}
	
template<class CharType> bool isBoolConst(CharType* str, int len)
{
	if(	len==4 &&
		str[0]=='t' 	&&
		str[1]=='r' 	&&
		str[2]=='u' 	&& 
		str[3]=='e' 	&&
		str[4] == 0 ) return true;
	
	return len==5 &&
		str[0]=='f' 	&&
		str[1]=='a' 	&& 
		str[2]=='l' 		&&
		str[3]=='s' 	&&
		str[4]=='e' 	&&
		str[4] ==0 ;
}

template<class CharType> int eatHex(const CharType* src, int &val)
{
	int cc = 0;
	val = 0;
	if(src[0]=='0' && src[1]=='x' || src[1]=='X')
	{
		cc +=2;
		while(true)
		{
			CharType ch = src[cc++];
			if(ch>='a' && ch<='f')
				val  =(int)((unsigned int)(val *16) + (unsigned int)(ch - 'a'+ 10));
			else if(ch>='A' && ch<='F')
				val  =(int)((unsigned int)(val *16) + (unsigned int)(ch - 'A'+ 10));
			else if(ch>='0' && ch<='9')
				val  = val  =(int)((unsigned int)(val *16) + (unsigned int)(ch - '0'));
			else
				break;
		}
	}
	return cc - 1; 
}

template<class CharType> int eatNumber(const CharType * src, Number &val)
{
	//initial
	val.Zero();
	
	int cc = 0;
	int len = eatHex(src,val.intPart);
	if(len > 0) return len;
	
	if(!isDigit(src[cc])) return 0;
	
	//process int part
	CharType ch = src[cc++];
	while(ch>='0' && ch<='9')
	{
		val.intPart = val.intPart *10 + ch - '0';
		ch = src[cc++];
	}
	
	if(ch != '.') return cc - 1;
	
	//整数类型函数
	if(src[cc] == '_' || isLetter(src[cc])) return cc - 1;
	
	//this is should number range
	if(src[cc]=='.') return cc - 1; //et 1..3
	
	//process scale part
	int e = 0;
	ch = src[cc++];
	
	val.kind = Number::kDouble;
	while (isDigit(ch))
	{
		e= e - 1;
		val.mantissa = val.mantissa*10 + ch - '0';
		ch = src[cc++];
	}
	
	if(ch=='e'||ch=='E')
	{
		int sign = 1;
		int s = 0;
		ch = src[cc++];
		if(ch=='+' || ch=='-') 
		{
			sign = ch ? 1 : -1;
			ch = src[cc++];
		}
		//if(ch <'0'|| ch >'9')
		//	error(error::err_doublescale);
		while (isDigit(ch))
		{
			s = s*10 + ch -'0';
			ch = src[cc++];
		}
		
		val.mantissaScale = e;
		val.scale = sign*s;
	}
	return  cc - 1;
}

template<class CharType> int eatString(const CharType* src)
{
	int cc=0;
	if(src[0]!='\'' && src[0]!='\"') return 0;
	
	CharType askch = src[cc++];
	while(true)
	{
		CharType ch = src[cc++];
		if(ch=='\\')
		{
			cc++; //防止匹配\"\' 
			continue;
		}
		if(!ch)break; //error
		if(ch==askch)
		{
			ch = src[cc++];
			break;
		}
	}
	return cc - 1;
}

template<class CharType> int eatCppOperate(const CharType* src, Operate& op)
{
#define branch(x,y)  if(ch==x) { ch = src[cc++]; op = y; break; }
	int cc=0;
	CharType ch = src[cc++];
	op = opUnknown;
	switch(ch) 
	{
	case '+':
		ch = src[cc++];
		branch('+',opInc);
		branch('=',opAddAsn);
		op = opAdd;
		break;
	case '-':
		ch = src[cc++];
		branch('-',opDec);
		branch('=',opSubAsn);
		branch('>',opPointer);
		op = opSub;
		break;
	case '*':
		ch = src[cc++];
		branch('=', opMulAsn);
		op = opMul;
		break;
	case '/':
		ch = src[cc++];
		branch('=', opDivAsn);
		op = opDiv;
		break;
	case '%':
		ch = src[cc++];
		branch('=', opModAsn);
		op = opMod;
		break;
	case '^':
		ch = src[cc++];
		branch('=', opXorAsn);
		op = opXor;
		break;
	case '~':
		ch = src[cc++];
		branch('=', opBnotAsn);
		op = opBnot;
		break;
	case ':':
		ch = src[cc++];
		branch('=', opBecomes);
		branch(':', opNest);
		op = opColon;
		break;
	case '<':
		ch = src[cc++];
		branch('=', opLte);
		branch('>', opNeq);
		if(ch=='<')
		{
			ch = src[cc++];
			branch('=', opShlAsn);
			op = opShl;
			break;
		}
		op = opLt;
		break;
	case '>':
		ch = src[cc++];
		branch('=', opGte);
		if(ch=='>')
		{
			ch = src[cc++];
			branch('=', opShrAsn);
			op = opShr;
			break;
		}
		op = opGt;
		break;
	case '=':
		ch = src[cc++];
		branch('=', opEq);
		branch('>', opArrow);
		op = opAsn;
		break;
	case '|':
		ch = src[cc++];
		branch('=', opBorAsn);
		branch('|', opOr);
		op = opBor;
		break;
	case '&':
		ch = src[cc++];
		branch('=', opBandAsn);
		branch('&', opAnd);
		op = opBand;
		break;
	case '#':
		ch = src[cc++];
		op = opHash;
		break;
	case '$':
		ch = src[cc++];
		op = opDollar;
		break;
	case '@':
		ch = src[cc++];
		op = opSign;
		break;
	case '?':
		ch = src[cc++];
		op = opQuery;
		break;
	case '\\':
		ch = src[cc++];
		op = opBackslash;
		break;
	case '!':
		ch = src[cc++];
		branch('=', opNeq);
		op = opNot;
		break;
	case '(':
		ch = src[cc++];
		op = opLparen;
		break;
	case ')':
		ch = src[cc++];
		op = opRparen;
		break;
	case '[':
		ch = src[cc++];
		op = opLbrackets;
		break;
	case ']':
		ch = src[cc++];
		op = opRbrackets;
		break;
	case '{':
		ch = src[cc++];
		op = opLbrace;
		break;
	case '}':
		ch = src[cc++];
		op = opRbrace;
		break;
	case ',':
		ch = src[cc++];
		op = opComma;
		break;
	case ';':
		ch = src[cc++];
		op = opSemicolon;
		break;
	case '.':
		ch = src[cc++];
		if(ch=='.')
		{
			ch = src[cc++];
			branch('.', opDot3);
			op = opDot2;
			break;
		}
		op = opDot;
		break;
	}
	return cc - 1;
	#undef branch
}	

template<class CharType> CharType nextch(const CharType* src, int &cc, int & srcRow, int & srcCol)
{
	if(!src[cc]) return 0;
	CharType ch = src[cc++];
	if(ch=='\r' && src[cc] != '\n') 
		ch='\n';
	if(ch=='n')
	{
		srcRow++;
		srcCol = 0;
	}
	else
		srcCol++;
	return ch;
}

template<class CharType> int eatBlank(const CharType* src, int & srcRow, int & srcCol)
{
	int cc=0;
	CharType ch = nextch(src,cc,srcRow,srcCol);
	while(ch && isBlankLetter(ch)) ch = nextch(src,cc,srcRow,srcCol);
	return cc - 1;		
}

template<class CharType> int eatCppComment(const CharType* src, int & srcRow, int & srcCol)
{
	if(src[0] != '/' ) return 0;
	if(src[1] != '/' && src[1] !='*') return 0; 
	
	int cc = 1;
	CharType ch = nextch(src,cc,srcRow,srcCol);
	
	//comment
	if(ch=='/')
	{
		//eat char until line end
		nextch(src,cc,srcRow,srcCol);
		while( ch && ch != '\n')
		{
			ch = nextch(src,cc,srcRow,srcCol);
		}
		if(!ch) return cc - 1;
		return cc;
	}
	//block comment
	else 
	{
		assert(ch=='*');
		ch = nextch(src,cc,srcRow,srcCol);
		while( ch && (ch != '*' || (ch == '*' && src[cc] != '/')))
		{
			ch = nextch(src,cc,srcRow,srcCol);
		}
		if(!ch) return cc - 1;
		return cc+1;
	}
}

template<class CharType> int eatXQueryComment(const CharType* src, int & srcRow, int & srcCol)
{
	if(src[0] != '(' ) return 0;
	if(src[1] != ':' ) return 0; 
	int count = 1;
	int cc = 2;
	CharType ch = nextch(src,cc,srcRow,srcCol);
	while(true)
	{
		ch = nextch(src,cc,srcRow,srcCol);
		while(ch)
		{
			if(ch=='(' && src[cc]==':') break;
			if(ch==':' && src[cc]==')') break;
			ch = src[cc++];
		}
		if(!ch) return cc - 1;
		if(ch=='(')
		{
			count++;
			cc++;
			ch = nextch(src,cc,srcRow,srcCol);
		}else 
		{
			assert(ch==':');
			count--;
			if(count==0) return cc+1;
			cc++;
			ch = nextch(src,cc,srcRow,srcCol);
		}
	}
}

template<class CharType> int eatIdent(const CharType* src)
{
	int cc = 0;
	CharType ch = src[cc++];
	if( isLetter(ch) || isDigit(ch) || ch=='$' || ch=='_' || ch=='@' || ch=='#' || ch>255)
	{
		do
		{
			ch = src[cc++];
		}while(isLetter(ch) || isDigit(ch) || ch=='$' || ch=='_' || ch=='@' || ch=='#' || ch>255 
			|| ch=='-' /* allow function-param-list */ 
			||	(ch==':' && cc > 3 && src[cc-2] =='n' && src[cc-3]=='f'));
		//macro name
		if(ch=='!')cc++; 
	}
	return cc - 1 ;
}

}} //util::lexer
	
} //namespace xl

#endif //XLLEXERUTIL_H


