#include "wpcontext.hpp"

using namespace xl;
using namespace xl::wp;

 FirstSet *  WPContext::GetFirstSet(SymList * symList)
{
	FirstSet * firstSet = 0;
	SymList * t = symList;
	while(t)
	{
		if(t->term)
		{
 			/*firsSet = firstSet==0? GetFirstSet(t->term) : firstSet->Union() */
		}
		t = t->next;
	}
	return 0;
}

 FirstSet * WPContext::GetFirstSet(SymRuler * ruler)
{

	if(rulerFirstSets.find(ruler->ident) == rulerFirstSets.end())
		rulerFirstSets[ruler->ident] = GetFirstSet(ruler->rhs);
	return rulerFirstSets[ruler->ident];
}

FirstSet* WPContext::GetFirstSet(SymNode<SymKind>* term)
{
	return 0;
}


bool WPContext::AllowNill(SymNode<SymKind> * term)
{
	switch(term->kind)
	{
		case kSymOption:
			return true;
		case kSymOneof:
		{
			SymOneof * oneof = (SymOneof *)term;
			if(oneof->terms->kind != kSymList) return AllowNill(oneof->terms);
			SymList * t = (SymList *)oneof->terms;
			while(t)
			{
				if(t->term && AllowNill(t->term)) return true;
				t = t->next;
			}
			return false;
		}
		case kSymList:
		{
			SymList * t = (SymList *)term;
			while(t)
			{
				if(t->term && !(t->term->kind == kSymVar || t->term->kind == kSymAction || t->term->kind == kSymAssign))
				{
					if(!AllowNill(t->term)) break;
				}
				t=t->next;
			}
			return t? false: true;

		}
		default:
			return false;
	}
}

HSymbol  WPContext::GetSymbol(const wchar_t* term)
{
	return symLang->stringPool->Ensure(term);
}

HSymbol WPContext::GetIdentifier()
{
	return GetSymbol(L"identifier");
}

HSymbol WPContext::GetStringLiteral()
{
	return GetSymbol(L"string-literal");
}

HSymbol WPContext::GetIntegerLiteral()
{
	return GetSymbol(L"integer-literal");
}

HSymbol WPContext::GetCharLiteral()
{
	return GetSymbol(L"char-literal");
}

HSymbol WPContext::GetBoolLiteral()
{
	return GetSymbol(L"bool-literal");
}

HSymbol WPContext::GetDoubleLiteral()
{
	return GetSymbol(L"double-literal");
}
