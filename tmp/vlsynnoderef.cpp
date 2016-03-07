#include "vlsynnoderef.hpp"

using namespace vl;

vlSynNodeRef::vlSynNodeRef(vlAstAxis * synNode, vlAstAxis * instNode): vlAstNode(akSynNodeRef),synNode(synNode),instNode(instNode)
{
}

std::wstring vlSynNodeRef::printOwner()
{
	return L"";
}