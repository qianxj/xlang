#ifndef WPP_SYNNODEREF_H
#define WPP_SYNNODEREF_H

#include <string>
#include "vlastnode.hpp"
#include "vlastaxis.hpp"

namespace vl
{

struct vlSynNodeRef: public vlAstNode
{
public:
	vlSynNodeRef(vlAstAxis * synNode, vlAstAxis * instNode);
public:
	virtual std::wstring printOwner();
public:
	vlAstAxis * synNode;
	vlAstAxis * instNode;
};

}	//namespace

#endif //WPP_SYNNODEREF_H