#include "vlastnode.hpp"
#include "vlastIdentifier.hpp"
#include "vlastaxis.hpp"
#include "vlasttoken.hpp"

using namespace xlang;

vlAstNode::vlAstNode(vlAstKind kind):kind(kind),realKind(akAstNode),param(0),token_(0)
{
}

int vlAstNode::getIdent()
{
	if(realKind==akIdentifierNode)
		return ((vlAstIdentifier*)this)->ident;
	if(kind == akAxisNode)
		return ((vlAstAxis *)this)->node->getIdent();
	return 0;
}

std::wstring  vlAstNode::printOwner()
{ 
	if(!this) return L"";

	wchar_t buf[255];
	if(kind==akListNode)
	{
		swprintf_s(buf,255,L"",kind);
		//swprintf_s(buf,255,L" list ",kind);
	}
	else
		swprintf_s(buf,255,L" node[0x%x] ",kind);
	return buf;
}

vlToken	*	vlAstNode::getToken()
{
	/*if(!isToken()) return 0;
	vlAstToken * node =(vlAstToken * )(kind == akAxisNode? ((vlAstAxis *)this)->Node() : this);
	if(node) return node->token_;
	return 0;*/
	vlAstNode * node = kind == akAxisNode ? ((vlAstAxis *)this)->Node() : this;
	return node ? node->token_ : 0;
}

void vlAstNode::setToken(struct vlToken * pToken)
{
	token_ = pToken;
}

bool vlAstNode::isInteger()
{
	vlToken	* tk = getToken();
	if(tk && tk->ty ==tokenType::tInt) return true;
	return false;
}

bool	vlAstNode::isDouble()
{
	vlToken	* tk = getToken();
	if(tk && tk->ty ==tokenType::tDouble) return true;
	return false;
}

bool	vlAstNode::isString()
{
	vlToken	* tk = getToken();
	if(tk && tk->ty ==tokenType::tString) return true;
	return false;
}

bool	vlAstNode::isChar()
{
	vlToken	* tk = getToken();
	if(tk && tk->ty ==tokenType::tChar) return true;
	return false;
}

bool	vlAstNode::isByte()
{
	return false;
}

bool	vlAstNode::isBoolean()
{
	vlToken	* tk = getToken();
	if(tk && tk->ty ==tokenType::tBool) return true;
	return false;
}

bool	vlAstNode::isNumber()
{
	vlToken	* tk = getToken();
	if(tk && (tk->ty ==tokenType::tInt ||
			tk->ty ==tokenType::tDouble ||
			tk->ty ==tokenType::tChar)) return true;
	return false;
}

bool	vlAstNode::isLiteral()
{
	vlToken	* tk = getToken();
	if(tk && ( tk->ty == tokenType::tInt ||
		tk->ty == tokenType::tDouble ||
		tk->ty == tokenType::tChar ||
		tk->ty == tokenType::tBool ||
		tk->ty == tokenType::tString)) return true;
	return false;
}

bool	vlAstNode::isOperator()
{
	vlToken	* tk = getToken();
	if(tk && tk->ty == tokenType::tOp) return true;
	return false;
}

bool	vlAstNode::isToken()
{
	if(realKind==akTokenNode)
		return true;
	if(kind == akAxisNode)
		return ((vlAstAxis *)this)->node->isToken();
	return false;
}

int		vlAstNode::getInteger()
{
	vlToken	* tk = getToken();
	if(tk && tk->ty == tokenType::tIdent) return tk->inum;
	if(tk && tk->ty == tokenType::tInt) return tk->inum;
	if(tk && tk->ty == tokenType::tChar) return tk->inum;
	if(tk && tk->ty == tokenType::tDouble) return (int)tk->dnum;
	if(tk && tk->ty == tokenType::tBool) return (int)tk->inum;
	
	if(tk && tk->ty == tokenType::tString) return *(int*)&tk->snum;
	
	return 0;
}

int		vlAstNode::getStringID()
{
	vlToken	* tk = getToken();
	if(tk && tk->ty == tokenType::tString) return *(int*)&tk->inum;
	return 0;
}

double	vlAstNode::getDouble()
{
	vlToken	* tk = getToken();
	if(tk && tk->ty == tokenType::tDouble) return tk->dnum;
	if(tk && tk->ty == tokenType::tInt)	   return (double)tk->inum;
	if(tk && tk->ty == tokenType::tChar)   return (double)tk->inum;
	if(tk && tk->ty == tokenType::tBool)   return (double)tk->inum;
	return 0;
}

const wchar_t *	vlAstNode::getString()
{
	vlToken	* tk = getToken();
	if(tk && tk->ty == tokenType::tString) return tk->snum;
	return 0;
}

wchar_t		vlAstNode::getChar()
{
	vlToken	* tk = getToken();
	if(tk && tk->ty == tokenType::tChar) return tk->inum;
	return 0;
}

unsigned char vlAstNode::getByte()
{
	return 0;
}

bool	vlAstNode::getBoolean()
{
	vlToken	* tk = getToken();
	if(tk && (tk->ty == tokenType::tBool||tk->ty == tokenType::tInt)) return tk->inum ? true :false;
	return false;
}

int	vlAstNode::getOperator()
{
	vlToken	* tk = getToken();
	if(tk && tk->ty == tokenType::tOp) return tk->inum;
	return 0;
}