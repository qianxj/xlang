#ifndef WPP5_ASTIDENTIFIER_H
#define WPP5_ASTIDENTIFIER_H

#include "vlutils.hpp"
#include "vlastnode.hpp"

namespace xlang{ 

class vlAstIdentifier : public vlAstNode
{
public:
	vlAstIdentifier(vlAstKind kind,int ide);
	vlAstIdentifier(int ide);
public:
	int ident;
public:
	virtual std::wstring printOwner();
};

} //namespace xlang

#endif //WPP5_ASTIDENTIFIER_H