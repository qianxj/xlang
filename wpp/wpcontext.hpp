#ifndef WPCONTEXT_H
#define WPCONTEXT_H

#include <map>
#include "wpsymnode.hpp"
#include "../common/util/xlbitvector.hpp"
#include "../common/util/xlstringpool.hpp"
namespace xl {namespace wp {

struct FirstSet
{
	BitVector* data;
	bool Match(Token<wchar_t> &tk);
};

//util function
bool Match(FirstSet & firstSet, Token<wchar_t> &tk);


struct WPContext
{
	/*cache fisrt set of ruler*/
	std::map<HSymbol , FirstSet *>  rulerFirstSets;

	SymLang * 	symLang;
	FirstSet* GetFirstSet(SymRuler* ruler);
	FirstSet* GetFirstSet(SymList * symList);
	FirstSet* GetFirstSet(SymNode<SymKind>* term);

	FirstSet* UnoinFirstSetWith(SymRuler* ruler, FirstSet* firstSet);
	FirstSet* UnoinFirstSetWith(SymList * symList, FirstSet* firstSet);

	bool AllowNill(SymNode<SymKind>* term);
	HSymbol GetSymbol(const wchar_t* term);
	HSymbol GetIdentifier();
	HSymbol GetStringLiteral();
	HSymbol GetIntegerLiteral();
	HSymbol GetCharLiteral();
	HSymbol GetBoolLiteral();
	HSymbol GetDoubleLiteral();
};

}} //namespace xl::wp


#endif //WPCONTEXT_H