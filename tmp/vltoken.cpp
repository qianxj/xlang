#include "vltoken.hpp"
#include <tchar.h>

using namespace xlang;

opValue::valuetype opValue::getOperateByToken(wchar_t ch)
{
	switch(ch)
	{
	case '+':
		return add;
	case '-':
		return sub;
	case '*':
		return mul;
	case '/': 
		return div;
	case '%':
		return mod;
	case '=':
		return asn;
	case '|':
		return bor;
	case '~':
		return bnot;
	case '!':
		return not;
	case '^':
		return xor;
	case '&':
		return band;
	case '>':
		return gt;
	case '<':
		return lt;
	case ')':
		return rparen;
	case '(':
		return lparen;
	case '{':
		return lbrace;
	case '}':
		return rbrace;
	case '[':
		return lbrackets;
	case ']':
		return rbrackets;
	case ',':
		return comma;
	case ';':
		return semicolon;
	case '.':
		return dot;
	case ':':
		return colon;
	case '#':
		return hash;
	case '?':
		return query;
	case '@':
		return sign;
	case '\\':
		return backslash;
	case '$':
		return dollar;
	case '\'':
		return apos;
	case '\"':
		return quotes;
	default:
		printf("error1 get \"%C\" operate",ch);
		return undef;
	}
}

opValue::valuetype opValue::getOperateByString(wchar_t* str)
{
	if(wcscmp(str,L"<<")==0) return shl;
	if(wcscmp(str,L">>")==0) return shr;
	if(wcscmp(str,L"++")==0) return inc;
	if(wcscmp(str,L"--")==0) return dec;
	if(wcscmp(str,L"||")==0) return or;
	if(wcscmp(str,L"&&")==0) return and;
	if(wcscmp(str,L"==")==0) return eq;
	if(wcscmp(str,L"!=")==0) return neq;
	if(wcscmp(str,L">=")==0) return gte;
	if(wcscmp(str,L"<=")==0) return lte;
	if(wcscmp(str,L"::")==0) return nest;
	if(wcscmp(str,L"->")==0) return pointer;
	if(wcscmp(str,L":=")==0) return becomes;
	if(wcscmp(str,L"<-")==0) return varrow;
	if(wcscmp(str,L"//")==0) return path2;
	if(wcscmp(str,L"=>")==0) return arrow;

	if(wcscmp(str,L"+=")==0) return _asn|add;
	if(wcscmp(str,L"-=")==0) return _asn|sub;
	if(wcscmp(str,L"*=")==0) return _asn|mul;
	if(wcscmp(str,L"/=")==0) return _asn|div;
	if(wcscmp(str,L"%=")==0) return _asn|mod;
	if(wcscmp(str,L"^=")==0) return _asn|xor;
	if(wcscmp(str,L"~=")==0) return _asn|bnot;
	if(wcscmp(str,L"&=")==0) return _asn|band;
	if(wcscmp(str,L"|=")==0) return _asn|bor;
	if(wcscmp(str,L">>=")==0) return _asn|shr;
	if(wcscmp(str,L"<<=")==0) return _asn|shl;
	if(wcscmp(str,L"->*")==0) return mpointer;
	if(wcscmp(str,L"...")==0) return dot3;
	if(wcscmp(str,L"..")==0) return dot2;
	if(wcscmp(str,L".*")==0) return mref;
	if(wcscmp(str,L"ln")==0) return ln;

	printf("error2 get \"%S\" operate",str);
	return undef;
}


wchar_t * opValue::getOperateText(int op)
{
	switch(op)
	{
	case _asn|add: return L"+=";
	case _asn|sub: return L"-=";
	case _asn|mul: return L"*=";
	case _asn|div: return L"/=";
	case _asn|mod: return L"%=";
	case _asn|xor: return L"^=";
	case _asn|band: return L"&=";
	case _asn|bor: return L"|=";
	case _asn|shr: return L">>=";
	case _asn|shl: return L"<<=";
	case add: return L"+";
	case sub: return L"-";
	case mul: return L"*";
	case div: return L"/";
	case shl: return L"<<";
	case shr: return L">>";
	case asn: return L"=";
	case mod: return L"%";
	case inc: return L"++";
	case dec: return L"--";
	case or:  return L"||";
	case and: return L"&&";
	case not: return L"!";
	case band: return L"&";
	case bor: return L"|";
	case bnot: return L"~";
	case xor: return L"^";
	case eq: return L"==";
	case neq: return L"!=";
	case gt: return L">";
	case gte: return L">=";
	case lt: return L"<";
	case lte: return L"<=";			
	case rparen:	return L")";
	case lparen:	return L"(";
	case rbrace:	return L"}";
	case lbrace:	return L"{";
	case lbrackets: return L"[";
	case rbrackets: return L"]";
	case comma:		return L",";
	case semicolon: return L";";
	case dot:		return L".";	
	case colon:		return L":";
	case hash:		return L"#";
	case becomes:	return L":=";
	case query:		return L"?";
	case sign:		return L"@";
	case backslash: return L"\\";
	case dollar:	return L"$";
	case apos:		return L"'";
	case quotes:	return L"\"";
	case nest:		return L"::";
	case percent:	return L".";
	case pointer:	return L"->";
	case dot3:		return L"...";
	case dot2:		return L"..";
	case ln:		return L"ln";
	case mref:		return L".*";
	case mpointer:	return L"->*";
	case varrow:	return L"<-";
	case path2:		return L"//";
	case arrow:		return L"=>";
	}
	return L"undef";
}

