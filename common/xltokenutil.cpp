#include "xltokenutil.hpp"

using namespace xl::util::token;
	
bool isAsnOperate(Operate op )
{
	switch(op)
	{
		case opAsn: 	 
		case opAddAsn:	
		case opSubAsn:	
		case opMulAsn:	
		case opDivAsn:	
		case opModAsn:	
		//bit asn operator
		case opShlAsn:	
		case opShrAsn:	
		case opBandAsn:  
		case opBorAsn:	
		case opXorAsn:	
		case opBnotAsn: 
			return true;
		default:
			return false;
	}	
}

bool isMathOperator(Operate op) { 
	switch(op)
	{
		case opAdd:
		case opSub:
		case opMul:
		case opDiv:
		case opMod:
		case opAsn:
		case opInc:
		case opDec:
		case opAddAsn:
		case opSubAsn:
		case opMulAsn:
		case opDivAsn:
		case opModAsn:
			return true;
		default:
			return false;
	}
}
bool isBitOperator(Operate op)
{
	switch(op)
	{
		case opShl:
		case opShr:
		case opBand:
		case opBor:
		case opXor:
		case opBnot:
		case opShlAsn:
		case opShrAsn:
		case opBandAsn:
		case opBorAsn:
		case opXorAsn:
		case opBnotAsn:
			return true;
		default:
			return false;
	}
}
bool isRelationOperator(Operate op)
{
	switch(op)
	{
		case opOr:	
		case opAnd:
		case opGt:	
		case opGte:
		case opLt:	
		case opLte:
		case opEq:	
		case opNeq:
		case opNot:
			return true;
		default:
			return false;
	}
}

bool isGroupOperator(Operate op)
{
	switch(op)
	{
		case opRparen:
		case opLparen:
		case opRbrace:
		case opLbrace:
		case opRbrackets:
		case opLbrackets:
			return true;
		default:
			return false;
	}
}

bool isSeparateOperator(Operate op)
{
	switch(op)
	{
		case opComma:	
		case opSemicolon:
			return true;
		default:
			return false;
	}
}
bool isUnaryOperator(Operate op)
{
	switch(op)
	{
		case opInc:
		case opDec:
		case opMul:
		case opBand:
		case opBnot:
			return true;
		default:
			return false;
	}
}
bool isPostfixOperator(Operate op)
{
	switch(op)
	{
		case opInc:
		case opDec:
		case opPointer:
		case opRef:
			return true;
		default:
			return false;
	}
}

Operate unAsnOperator(Operate op) 
{
	switch(op)
	{
		case opAddAsn: 	return opAdd;
		case opSubAsn: 	return opSub;
		case opMulAsn: 	return opMul;
		case opDivAsn:  	return opDiv;
		case opModAsn: 	return opMod;
		case opShlAsn:  	return opShl;
		case opShrAsn:  	return opShr;
		case opBandAsn: return opBand;
		case opBorAsn:   return opBor;
		case opXorAsn:  	return opXor;
		case opBnotAsn:	return opBnot;
		default:
			return opUnknown;
	}
}


Operate  GetOperate(const wchar_t* op)
{
	if(!op || !op[0]) return opUnknown;
	switch(op[0]) 
	{
	case '+':
		if(op[1]=='+') return opInc;
		if(op[1]=='=') return opAddAsn;
		return opAdd;
	case '-':
		if(op[1]=='-') return opDec;
		if(op[1]=='=') return opSubAsn;
		if(op[1]=='>') return opPointer;
		return opSub;
	case '*':
		if(op[1]=='=') return opMulAsn;
		return opMul;
	case '/':
		if(op[1]=='=') return opDivAsn;
		return opDiv;
	case '%':
		if(op[1]=='=') return opModAsn;
		return opMod;
	case '^':
		if(op[1]=='=') return opXorAsn;
		return opXor;
	case '~':
		if(op[1]=='=') return opBnotAsn;
		return opBnot;
	case ':':
		if(op[1]=='=') return opBecomes;
		if(op[1]==':') return opNest;
		return opColon;
	case '<':
		if(op[1]=='=') return opLte;
		if(op[1]=='<')
		{
			if(op[2]=='=') return opShlAsn;
			return opShl;
		}
		return opLt;
	case '>':
		if(op[1]=='=') return opGte;
		if(op[1]=='>')
		{
			if(op[2]=='=') return opShrAsn;
			return opShr;
		}
		return opGt;
	case '=':
		if(op[1]=='=') return opEq;
		if(op[1]=='>') return opArrow;
		return opAsn;
	case '|':
		if(op[1]=='=') return opBorAsn;
		if(op[1]=='|')  return opOr;
		return opBor;
	case '&':
		if(op[1]=='=') return opBandAsn;
		if(op[1]=='&')  return opAnd;
		return opBand;
	case '#':
		return opHash;
	case '$':
		return opDollar;
	case '@':
		return opSign;
	case '?':
		return opQuery;
	case '\\':
		return opBackslash;
	case '!':
		if(op[1]=='=') return opNeq;
		return opNot;
	case '(':
		return opLparen;
	case ')':
		return opRparen;
	case '[':
		return opLbrackets;
	case ']':
		return opRbrackets;
	case '{':
		return opLbrace;
	case '}':
		return opRbrace;
	case ',':
		return opComma;
	case ';':
		return opSemicolon;
	case '.':
		if(op[1]=='.')
		{
			if(op[2]=='.') return opDot3;
			return opDot2;
		}
		return opDot;
	default:
			return opUnknown;
	}
}

const wchar_t* xl::util::token::getOperateText(Operate op)
{
	switch(op)
	{
		//math operator
		case opAdd:		return L"+"; 
		case opSub:		return L"-";
		case opMul	:		return L"*";
		case opDiv:		return L"/";
		case opMod:		return L"%";
		
		//unary
		case opAsn:		return L"=";
		case opInc:		return L"++";
		case opDec:		return L"--";
		
		//bit operator
		case opShl:		return L"<<";
		case opShr:		return L">>";
		case opBand:		return L"&";
		case opBor:		return L"|";
		case opXor:		return L"^";
		case opBnot:		return L"~"; 
		
		//relation operator
		case opOr:			return L"||";
		case opAnd:		return L"&&";
		case opGt:			return L">";
		case opGte:		return L">=";
		case opLt:			return L"<";
		case opLte:		return L"<=";
		case opEq:			return L"==";
		case opNeq:		return L"!=";
		case opNot:		return L"!";
		
		
		//group operator
		case opRparen:		return L")";
		case opLparen:		return L"(";
		case opRbrace:		return L"}";
		case opLbrace:		return L"{";
		case opRbrackets:	return L"]";
		case opLbrackets:	return L"[";
		
		case opComma:		return L",";
		case opSemicolon:	return L";";
		case opSign:		 	return L"@";
		case opDollar:		 	return L"$";
		
		case opApos:			return L"\"";
		case opPath2:		return L"//";  
		case opSizeof:		return L"sizeof";
		
		case opNest:		return L"::";
		case opPointer:  	return L"->";
		case opDot3:	    return L"...";
		case opDot2:	    return L"..";
		case opIn:	    	return L"oneof"; 
		case opMref:		return L"::&";
		case opMpointer:	return L"::->";
		case opVarrow:  	return L"<-";
		case opBecomes: 	return L":=";
		case opQuery:		return L"?";
		case opColon:		return L":"; 
		case opHash:		return L"#";
		case opRef:			return L".";
		case opBackslash:	return L"\\";
		
		#if 0
		//only for see
		case opExclam:  	return L"!";
		case opPercent: 	return L"%";
		case opDot:			return L".";
		case opPeriod:		return L".";
		case opArrow:    	return L"->";
		case opPath:		return L"/";
		#endif
		
		//math asn operator
		case opAddAsn:		return L"+=";
		case opSubAsn:		return L"-=";
		case opMulAsn:		return L"*=";
		case opDivAsn:		return L"/=";
		case opModAsn:		return L"%=";
		
		//bit asn operator
		case opShlAsn:		return L"<<=";
		case opShrAsn:		return L">>=";
		case opBandAsn:		return L"&=";
		case opBorAsn:		return L"|=";
		case opXorAsn:		return L"^=";
		case opBnotAsn:		return L"~+";
		default:
			return L"unknown";
	}
}
