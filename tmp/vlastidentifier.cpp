#include "vlastidentifier.hpp"
using namespace xlang;

vlAstIdentifier::vlAstIdentifier(vlAstKind kind,int ide):vlAstNode(kind),ident(ide){
		realKind = akIdentifierNode;
}

vlAstIdentifier::vlAstIdentifier(int ide):vlAstNode(akIdentifierNode),ident(ide){
		realKind = akIdentifierNode;
}

std::wstring vlAstIdentifier::printOwner()
{
	if(!this) return L"";
	const wchar_t * str = util::getHashString(ident);
	if(kind==akBnfIdentifier)
		return (std::wstring)L":" +  util::getHashString(ident);
	else
		return util::getHashString(ident);
}
