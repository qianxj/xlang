#include "vlasttoken.hpp"

using namespace xlang;

vlAstToken::vlAstToken(vlAstKind kind):vlAstNode(kind)
{
	realKind = akTokenNode;
}

vlAstToken::vlAstToken(vlToken & token):vlAstNode(akTokenNode),token(token)
{
	token_ = &this->token;
	realKind = akTokenNode;
}

vlAstToken::vlAstToken(vlAstKind kind,vlToken & token):vlAstNode(kind),token(token)
{
	token_ = &this->token;
	realKind = akTokenNode;
}



std::wstring vlAstToken::printOwner()
{
	if(!this) return L"";

	std::wstring str;
	//ensure tokenkind
	if(kind==akTokenNode)
	{
		wchar_t buf[255];
		if(getToken()->ty == tokenType::tString)
			str = str + L"\"" + getToken()->snum + L"\"";
		else if(getToken()->ty == tokenType::tInt)
		{
			swprintf_s(buf,255,L"%d",getToken()->inum);
			str = buf;
		}
		else if(getToken()->ty == tokenType::tDouble)
		{
			swprintf_s(buf,255,L"%f",getToken()->dnum);
			str= buf;
		}
		else if(getToken()->ty == tokenType::tChar)
		{
			swprintf_s(buf,255,L"'%c'",getToken()->inum);
			str = buf ;
		}
		else if(getToken()->ty == tokenType::tIdent)
		{
			str=  getToken()->snum;
		}
		else if(getToken()->ty == tokenType::tBool)
		{
			if(getToken()->inum)
				str = (std::wstring)L"true";
			else
				str = (std::wstring)L"false";
		} 
		else if(getToken()->ty == tokenType::tOp)
		{
			str =  (std::wstring)L"'" + opValue::getOperateText(getToken()->inum) + L"'";
		}else
			assert(false);
	}
	return str;
}