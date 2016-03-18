#include "wpgen.hpp"
#include "wpcode.hpp"
using namespace xl::wp;

void * genSynTerm(SymDeclVar* 	symVar)
{
	return Variant(symVar->islist, symVar->name);
}

void * genSynTerm(SymRepeat* 	symRepeat)
{
	void * node = genSynTerm(symRepeat->term);
	if(symRepeat->cond)
	{
		getSyntaxFirstSet(symRepeat->cond);
	}else
	{
		getSyntaxFirstSet(symRepeat->cond);
		
	}
	coder::Until(terms, coder::Literal(true));
}

void * genSynTerm(SymAction* 	symAction)
{

}

void * genSynTerm(SymAssign* 	symAssign)
{

}

void * genSynTerm(SymLiteral* 	symLiteral)
{

}

void * genSynTerm(SymKeyword* 	symKeyword)
{

}