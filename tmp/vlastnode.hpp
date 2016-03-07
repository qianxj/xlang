#ifndef WPP5_ASTNODE_H
#define WPP5_ASTNODE_H

#include "vlastkind.hpp"
#include <string>

namespace xlang
{

class vlAstNode
{
public:
	vlAstNode(vlAstKind kind);
public:
	vlAstKind kind;
	vlAstKind realKind;
public:
	void * param;
public:
	virtual std::wstring  printOwner();
public:
	int getIdent();
public:
	struct vlToken  * token_;
public:
	struct vlToken	* getToken();
	void setToken(struct vlToken * pToken);  
public:
	bool		isInteger();
	bool		isDouble();
	bool		isString();
	bool		isChar();
	bool		isByte();
	bool		isBoolean();
	bool		isNumber();
	bool		isLiteral();
	bool		isOperator();
	bool		isToken();

	int				getInteger();
	int				getStringID();
	double			getDouble();
	const wchar_t *	getString();
	wchar_t			getChar();
	unsigned char	getByte();
	bool			getBoolean();
	int				getOperator();
};

}

#endif //WPP5_ASTNODE_H