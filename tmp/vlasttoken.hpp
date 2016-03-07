#ifndef WPP5_ASTTOKEN_H
#define WPP5_ASTTOKEN_H

#include <string>
#include "vltoken.hpp"
#include "vlastnode.hpp"

namespace xlang
{

struct vlAstToken : public vlAstNode
{
public:
	vlAstToken(vlAstKind kind);
	vlAstToken(vlToken & token);
	vlAstToken(vlAstKind kind,vlToken & token);
public:
	virtual std::wstring printOwner();
public:
	vlToken token;
};

}	//namespace

#endif