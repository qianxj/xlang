#include "wpgen.hpp"
#include "wpcode.hpp"

using namespace xl::wp;


coder::PNode * genCond(WPContext* context, FirstSet * firstset)
{
	return 0;
}

coder::PNode *  genTerm(WPContext* context, SymDeclVar* 	symVar)
{
	//return Variant(symVar->islist, symVar->name);
	return 0;
}

coder::PNode * genTerm(WPContext* context, SymOneof* 	symOneof);
{
	if(symOneof->terms->Kind!=kSymList) return genTerm(context,symOneof->terms);
	SymList * terms = symOneof->terms;
	coder::astList * branchItems = coder::List();
	coder::pNode defaultStmt = 0;
	while(true)
	{
		if(terms->term->kind==synGuard)
		{
			coder::PNode * trueStmt = genTerm(context, terms->term);
			coder::List(brachItems, coder::BrachItem(genCond(context, getFirstSet(terms->term)),trueStmt));
		}else
		{
			defaultStmt = genTerm(context, terms->term)
		}
	}
	return coder::Branch(0,branchItems,defaultStmt);
}

coder::PNode * genTerm(WPContext* context, SymRepeat* 	symRepeat)
{
	coder::PNode* body = genTerm(context, symRepeat->term);
	coder::pNode* prebody = genCond(context, getFirstSet(symRepeat->body));
	coder::PList* stmt = coder::List(coder::List(coder::If(prebody,0,coder::Break())),body);
	return coder::Until(stmt, symRepeat->cond? genCond(context,symRepeat->cond): 0);
}

coder::PNode * genTerm(WPContext* context, SymTerm* 	symTerm)
{
	if(symTerm->name == context->GetIdentifier()) return coder::MatchIdentifier();
	else if(symTerm->name == context->GetLiteral()) return coder::MatchLiteral();
	else if(symTerm->name == context->GetStringLiteral()) return coder::MatchStringLiteral();
	else if(symTerm->name == context->GetIntegerLiteral()) return coder::MatchIntegerLiteral();
	else if(symTerm->name == context->GetCharLiteral()) return coder::MatchCharLiteral();
	else if(symTerm->name == context->GetBoolLiteral()) return coder::MatchBoolLiteral();
	else if(symTerm->name == context->GetDoubleLiteral()) return coder::MatchDoubleLiteral();
	else  return coder::MatchSymbolTerm();
}

coder::PNode * genTerm(WPContext* context, SymAction* 	symAction)
{
	SymList * terms = symAction->params;
	coder::Plist params = coder::List();
	while(terms)
	{
		coder::List(params,genTerm(context,terms->term));
		terms = terms->next;
	}
	if(symAction->ident != context->GetSymbol(L"assign"))
		return coder::Call(symAction->ident,params);
	else
	{
		return coder::Bianary(opAsn,params->term,params->next->term);
	}
}

coder::PNode * genTerm(WPContext* context, SymAssign* 	symAssign)
{
	return coder::Bianary(opAsn,coder::Term(symAssign->ident),genTerm(context,symAssign->term));
}

coder::PNode * genTerm(WPContext* context, SymLiteral* 	symLiteral)
{
	return coder::MatchLiteral(symLiteral->val);
}

coder::PNode * genTerm(WPContext* context, SymKeyword* 	symKeyword)
{
	return coder::MatchKeyword(symKeyword->name);
}

coder::PNode * genTerm(WPContext* context, SymKeyword* 	symOperate)
{
	return coder::MatchOperate(symOperate->op);
}

coder::PNode * genTerm(WPContext* context, SymNode<SymKind> * node);
{
	assert(node);
	switch(node->kind)
	{
		case kSymTerm:
			return genTerm(context, (SymTerm*)node);
		case kSymRepeat:
			return genTerm(context, (SymRepeat*)node);
		case kSymKeyword:
			return genTerm(context, (SymKeyword*)node);
		case kSymOperate:
			return genTerm(context, (SymOperate*)node);
		case kSymLiteral:
			return genTerm(context, (SymLiteral*)node);
		case kSymAction:
			return genTerm(context, (SymAction*)node);
		case kSymAssign:
			return genTerm(context, (SymAssign*)node);
		case kSymOneof:
			return genTerm(context, (SymOneof*)node);
		case kSymOption:
			return genTerm(context, (SymOption*)node);
		case kSymList:
		{
			SymList * terms = (SymList *)node;
			coder::PList * codes = coder::List();
			while(terms)
			{
				if(terms->term) coder::List(codes,genTerm(context,terms->term));
				terms = terms->next;
			}
			return codes;
		}
		default:
			assert(false);
	}
}