#include <assert.h>
#include "termanalyse.hpp"

namespace trap {
namespace complier {

int lookupVariant(LangContext & context,std::wstring name)
{
	int i = (int)context.variants.size() - 1;
	while(i>=0)
	{
		if(context.variants[i]==name) break;
		i--;
	}
	return i;
}

int sureVariant(LangContext & context,std::wstring name)
{
	int index = lookupVariant(context,name);
	if(index==-1)
	{
		context.variants.push_back(name);
		context.variantsTypes.push_back(L"TNode*");
		context.variantsInitials.push_back(0);
		return (int)context.variants.size() - 1;
	}
	return index;
}


coder::astExpr* getBranchCond(LangContext & context, std::set<int>& firstSet)
{
	coder::astNode* cond = 0; 
	std::set<int>::const_iterator tr = firstSet.begin();
	
	coder::astList* items = coder::List();
	while(tr != firstSet.end())
	{
		int pos = *tr;
		coder::astExpr* tm = (coder::astExpr*)makeMatchToken(context,pos);
		cond = cond ? ((coder::astExpr*)coder::Binary(opOr,cond,tm)) : tm;
		tr++;
	}
	return (coder::astExpr*)cond;
}

int analysePushSpendingSyntaxTerm(LangContext & context, int term)
{
	std::vector<int>::iterator iter = context.spendingSyntaxTerms.begin();
	for(;iter!=context.spendingSyntaxTerms.end();iter++)
	{
		if(*iter==term)break;
	}
	if(iter==context.spendingSyntaxTerms.end())context.spendingSyntaxTerms.push_back(term);
	return 1;
}

coder::astNode* makeMatchToken(LangContext & context,int pos)
{
	if(context.data[pos]==tkKeyword)
	{
		return coder::Call(L"parser->isKeyword",coder::List(coder::List(coder::ContextTerm(L"tk")),
			coder::Literal(_wcsdup(getKeywordName(context,context.data[pos+1]).c_str()))));
	}
	else if(context.data[pos]==tkIdent)
	{
		coder::astNode * node = coder::Call(L"parser->isIdent",coder::List(coder::ContextTerm(L"tk")));
		int apos = nextSyntaxToken(context,-1,pos);
		while(context.data[apos]==synAssign || context.data[apos]==synVariant|| 
				context.data[apos]==synConstructor)apos = nextSyntaxToken(context,-1,apos);
		if(context.data[apos]==tkKeyword)
			return coder::Binary(opAnd,node,coder::Call(L"parser->isKeyword",coder::List(coder::List(
			coder::Call(L"parser->pickToken",coder::List(0))),
				coder::Literal(_wcsdup(getKeywordName(context,context.data[apos+1]).c_str())))));
		else
			return node;
	}
	else if(context.data[pos]==tkLiteral)
	{
		return coder::Call(L"parser->isLiteral",coder::List(coder::ContextTerm(L"tk")));
	}
	else if(context.data[pos]==tkInteger)
	{
		return coder::Call(L"parser->isInteger",coder::List(coder::ContextTerm(L"tk")));
	}else if(context.data[pos]==tkBool)
	{
		return coder::Call(L"parser->isBool",coder::List(coder::ContextTerm(L"tk")));
	}else if(context.data[pos]==tkDouble)
	{
		return coder::Call(L"parser->isDouble",coder::List(coder::ContextTerm(L"tk")));
	}else if(context.data[pos]==tkChar)
	{
		return coder::Call(L"parser->isChar",coder::List(coder::ContextTerm(L"tk")));
	}else if(context.data[pos]==tkString)
	{
		return coder::Call(L"parser->isString",coder::List(coder::ContextTerm(L"tk")));
	}else if(context.data[pos] >=0x20)
	{
		const wchar_t * op = Token::getOperatorString((tokenKind_t)context.data[pos]);
		if(op[1])
			return coder::Call(L"parser->isOperator",coder::List(coder::List(coder::ContextTerm(L"tk")),
					coder::Literal(op)));
		else
			return coder::Call(L"parser->isOperator",coder::List(coder::List(coder::ContextTerm(L"tk")),
					coder::Literal(op[0])));
	}
	else
	{
		//assert(false);
		return 0;
	}
}

const wchar_t* getVariantType(LangContext & context,int pos)
{
	/*if(context.data[pos]==tkKeyword)
		return L"int";
	else if(context.data[pos]==tkIdent)
		return L"int";
	else if(context.data[pos]==tkLiteral)
		return L"coder::astLiteral *";
	else if(context.data[pos] >= 0x20 && context.data[pos] < 0xff)
		return L"int";
	else
		return L"coder::astNode *";*/
	if(context.data[pos] < 0x100)
		return L"TTokenNode*";
	//else
		return L"TNode*";
}
coder::astNode* getVariantValue(LangContext & context,int pos)
{
	/*if(context.data[pos]==tkKeyword)
		return coder::Call(L"getKeyword",coder::List(coder::ContextTerm(L"tk")));
	else if(context.data[pos]==tkIdent)
		return coder::Call(L"getIdent",coder::List(coder::ContextTerm(L"tk")));
	else if(context.data[pos]==tkLiteral)
		return coder::Call(L"getLiteral",coder::List(coder::ContextTerm(L"tk")));
	else if(context.data[pos] >= 0x20 && context.data[pos] < 0xff)
		return coder::Call(L"getOperator",coder::List(coder::ContextTerm(L"tk")));
	else
		return 0;*/
	if(context.data[pos]==tkKeyword)
		return coder::Call(L"parser->TokenKeyword",coder::List(coder::ContextTerm(L"tk")));
	else if(context.data[pos] < 0x100)
		return coder::Call(L"parser->TokenNode",coder::List(coder::ContextTerm(L"tk")));
	else
		return 0;
}

coder::astNode* optionSureSyntax(LangContext & context, int term, int pos,coder::astNode* stmtFalseProcess)
{
	if(context.data[pos] == synNill) return 0;
	std::vector<coder::astNode *> nodes;
	int pirorPos = 0;
	int pickIndex = 0;
	while(true)
	{
		if(pos && context.data[pos] == synSyntax)
		{
			int term = context.data[pos+1];
			const std::wstring &  name = getTermName(context,term);
			if(name==L"identifier")
			{
				coder::astNode * node = coder::Call(L"parser->isIdent",coder::List(coder::ContextTerm(L"tk")));
				nodes.push_back(coder::If((coder::astExpr*)coder::Unary(opNot,node),stmtFalseProcess,0));
				nodes.push_back(coder::Call(L"parser->pickToken",
					coder::List(coder::ContextTerm(L"tk"))));
			}else
			{
				coder::astList* param = coder::List(coder::ContextTerm(L"ctx"));
				coder::List(param, coder::ContextTerm(L"parser"));
				coder::List(param, coder::ContextTerm(L"tk"));
				nodes.push_back(
					coder::If((coder::astExpr*)coder::Unary(opNot,coder::Call(_wcsdup((L"sure_" + name).c_str()),param)),
					stmtFalseProcess,0));
				//analysePushSpendingSyntaxTerm(context,(int)context.mTerm[name]);
			}
		}
		else if(pos && context.data[pos] == synOneOf)
		{
			int apos = pos + 1;
			coder::astList* stmt = coder::List();
			coder::List(stmt,coder::Variant((coder::astType*)coder::ContextTerm(L"bool"),
				L"$match_",coder::Literal(false)));
			while(true)
			{
				if(context.data[apos]==synGuard)apos++;
				coder::List(stmt,coder::Loop(coder::Unary(opNot,coder::ContextTerm(L"$match_")),
					coder::List(coder::List(optionSureSyntax(context,term, apos,coder::Break())),
					coder::Binary(opAsn,
						coder::ContextTerm(L"$match_"),coder::Literal(true)))));
				apos = nextSyntaxPiece(context,term,apos);
				if(context.data[apos] == synNill)break;
			}
			coder::List(stmt,coder::If((coder::astExpr *)coder::Unary(opNot,coder::ContextTerm(L"$match_")),
					stmtFalseProcess,0));
			nodes.push_back(coder::CompStmt(stmt));
		}
		else if(pos && context.data[pos] == synOption)
		{
			int apos = pos + 1;
			coder::astList* stmt = coder::List();
			coder::List(stmt,coder::Variant((coder::astType*)coder::ContextTerm(L"bool"),
				L"$match_",coder::Literal(false)));

			coder::astList*  tnode = coder::List(optionSureSyntax(context,term, apos,coder::Break()));
			coder::List(tnode,coder::Binary(opAsn,coder::ContextTerm(L"$match_"),coder::Literal(true)));

			coder::List(stmt,coder::Until(tnode,coder::Literal(false)));
			nodes.push_back(coder::CompStmt(stmt));
		}
		else if(pos && context.data[pos] == synList)
		{
			int apos = pos + 1;
			std::set<int> firstSet;
			coder::astList* stmt = coder::List();
			coder::List(stmt,coder::Variant((coder::astType*)coder::ContextTerm(L"bool"),
				L"$match_",coder::Literal(false)));
			
			coder::astList* tnode = coder::List(optionSureSyntax(context,term, apos,coder::Break()));
			coder::List(tnode,coder::Binary(opAsn,
						coder::ContextTerm(L"$match_"),coder::Literal(true)));
			apos = nextSyntaxPiece(context,term,apos);
			if(context.data[apos]!=synNill)
			{
				getSyntaxFirstSet(context,term,apos, firstSet);
				coder::astNode * cond = getBranchCond(context, firstSet);
				coder::List(tnode,coder::If((coder::astExpr*)coder::Unary(opNot,cond),coder::List(coder::Break()),0));
				coder::List(tnode,coder::Call(
					L"parser->pickToken",
						coder::List(coder::ContextTerm(L"tk"))));
				
			}else
			{
				getSyntaxFirstSet(context,term,pos + 1, firstSet);
				coder::astNode * cond = getBranchCond(context, firstSet);
				tnode = coder::List(coder::List(coder::If((coder::astExpr*)coder::Unary(opNot,cond),coder::List(coder::Break()),0)),tnode);
			}
			coder::List(stmt,coder::Until(tnode, coder::Literal(true)));
			coder::List(stmt,coder::If((coder::astExpr *)coder::Unary(opNot,coder::ContextTerm(L"$match_")),
					stmtFalseProcess,0));
			nodes.push_back(coder::CompStmt(stmt));
		}
		else if(isPrimaryToken(context.data,pos) ||
			isOperatorToken(context.data,pos))
		{

			nodes.push_back(coder::If((coder::astExpr*)coder::Unary(opNot,makeMatchToken(context,pos)),
				stmtFalseProcess,0));
			nodes.push_back(coder::Call(L"parser->pickToken",
				coder::List(coder::ContextTerm(L"tk"))));
		}
		pirorPos = pos;
		pos = nextSyntaxToken(context, term, pos);
		if(context.data[pos]==synNill) break;
	}

	coder::astList* stmt = coder::List();
	for(std::size_t i=0;i<nodes.size();i++)
	{
		coder::List(stmt,nodes[i]);
	}
	//if(stmtFalseProcess->kind = coder::kReturn)
	//	coder::List(stmt,coder::Return((coder::astExpr*)coder::Literal(true)));
	return stmt;
}

coder::astNode* sureSyntax(LangContext & context, int term, int pos, bool bbreak,bool bsure)
{
	return coder::List(coder::List(
			optionSureSyntax(context,term,pos,coder::Return(coder::Literal(false)))),
			coder::Return((coder::astExpr*)coder::Literal(true)));

	//analyse code and construct tree of ast node
	if(context.data[pos] == synNill) return 0;
	coder::astList* stmt = coder::List();
	std::vector<coder::astNode *> nodes;
	int pirorPos = 0;
	int pickIndex = 0;
	while(true)
	{
		if(context.data[pos] == synVariant)
		{
		}
		else if(pos && context.data[pos] == synParam)
		{
		}
		else if(pos && context.data[pos] == synSyntax)
		{
			int term = context.data[pos+1];
			const std::wstring &  name = getTermName(context,term);
			coder::astList* param = coder::List(coder::ContextTerm(L"ctx"));
			coder::List(param, coder::ContextTerm(L"parser"));
			coder::List(param, coder::ContextTerm(L"tk"));
			nodes.push_back(
				coder::If((coder::astExpr*)coder::Unary(opNot,coder::Call(_wcsdup((L"sure_" + name).c_str()),param)),
				coder::Return((coder::astExpr*)coder::Literal(false)),0));
			//analysePushSpendingSyntaxTerm(context,(int)context.mTerm[name]);
		}
		else if(pos && context.data[pos] == synAssign)
		{
		}
		else if(pos && context.data[pos] == synConstructor)
		{
		}
		else if(pos && context.data[pos] == synOption)
		{
			std::set<int> firstSet;
			getSyntaxFirstSet(context,term,pos + 1, firstSet);
			coder::astExpr* cond = getBranchCond(context, firstSet);
			coder::astList* trueStmt = coder::List();
			coder::List(trueStmt, sureSyntax(context,term, pos+1,false,false));
			nodes.push_back(coder::If(cond, trueStmt, 0));
		}
		else if(pos && context.data[pos] == synOneOf)
		{
			int apos = pos + 1;
			coder::astList * branchItems = coder::List();
			coder::astNode* defaultStmt = 0;
			while(true)
			{
				std::set<int> firstSet;
				if(context.data[apos]!=synGuard)
				{
					coder::astList* trueStmt = coder::List();
					getSyntaxFirstSet(context,term,apos, firstSet);
					coder::List(trueStmt, sureSyntax(context,term, apos,false,false/*bsure*/));

					coder::List(branchItems,coder::BranchItem(getBranchCond(context,firstSet),
						trueStmt));
				}else
				{
					apos++;
					defaultStmt =  sureSyntax(context,term, apos,false,true/*bsure*/);
				}
				apos = nextSyntaxPiece(context,term,apos);
				if(context.data[apos] == synNill)break;
			}
			nodes.push_back(coder::Branch(0,branchItems,defaultStmt));
		}
		else if(pos && context.data[pos] == synList)
		{
			int apos = pos + 1;
			std::set<int> firstSet;
			coder::astList * tnode = 
				coder::List(analyseSyntax(context,term, apos,context.data[apos]==synOneOf?true:false)); 
			apos = nextSyntaxPiece(context,term,apos);
			if(context.data[apos]!=synNill)
			{
				getSyntaxFirstSet(context,term,apos, firstSet);
				coder::astNode * cond = getBranchCond(context, firstSet);
				coder::List(tnode,coder::If((coder::astExpr*)coder::Unary(opNot,cond),coder::List(coder::Break()),0));
				coder::List(tnode,coder::Call(
					L"parser->pickToken",
						coder::List(coder::ContextTerm(L"tk"))));
			}else
			{
				getSyntaxFirstSet(context,term,pos + 1, firstSet);
				coder::astNode * cond = getBranchCond(context, firstSet);
				tnode = coder::List(coder::List(coder::If((coder::astExpr*)coder::Unary(opNot,cond),coder::List(coder::Break()),0)),tnode);
			}
			nodes.push_back(coder::Until(tnode, coder::Literal(true)));
		}
		else if(isPrimaryToken(context.data,pos) ||
			isOperatorToken(context.data,pos))
		{

			nodes.push_back(coder::If((coder::astExpr*)coder::Unary(opNot,makeMatchToken(context,pos)),
				coder::Return((coder::astExpr*)coder::Literal(false)),0));

			int apos = nextSyntaxToken(context, term, pos);
			if(context.data[apos]==synAssign)
				nodes.push_back(getVariantValue(context,pos));

			nodes.push_back(coder::Call(L"parser->pickToken",
				coder::List(coder::ContextTerm(L"tk"))));
		}
		pirorPos = pos;
		pos = nextSyntaxToken(context, term, pos);
		if(context.data[pos]==synNill) break;
	}
	for(std::size_t i=0;i<nodes.size();i++)
	{
		coder::List(stmt,nodes[i]);
	}
	return stmt;
}

coder::astNode* analyseSyntax(LangContext & context, int term, int pos, bool bbreak,bool bsure)
{
	//analyse code and construct tree of ast node
	if(context.data[pos] == synNill) return 0;
	coder::astList* stmt = coder::List();
	std::vector<coder::astNode *> nodes;
	int pirorPos = 0;
	while(true)
	{
		if(context.data[pos] == synVariant)
		{
			const wchar_t*  name = (const wchar_t*) context.data[pos+1];
			const wchar_t*  type = (const wchar_t*) context.data[pos+2];
			
			//set variant initial value
			coder::astNode * node = 0;
			if(wcsstr(type,L"TNodeList")) node = coder::Call(L"parser->makeList",coder::List());

			//update variant context for lookup variant by name
			context.variants.push_back(name);
			context.variantsTypes.push_back(type? type:L"TNode*");
			context.variantsInitials.push_back(0);

			nodes.push_back(coder::Variant((coder::astType*)coder::ContextTerm(type),name,node));
		}
		else if(pos && context.data[pos] == synParam)
		{
			int paramIndex = context.data[pos+1];
			int paramPos = context.data[context.data[getSynTermHole(context,term,synParam)] + paramIndex];
			if(pos == paramPos) break;//dead loop
			nodes.push_back(analyseSyntax(context,term, paramPos));
		}
		else if(pos && context.data[pos] == synSyntax)
		{
			int term = context.data[pos+1];
			const std::wstring &  name = getTermName(context,term);
			coder::astList* param = coder::List(coder::ContextTerm(L"ctx"));
			coder::List(param, coder::ContextTerm(L"parser"));
			coder::List(param, coder::ContextTerm(L"tk"));
			nodes.push_back(coder::Call(_wcsdup((L"parse_" + name).c_str()),param));
			analysePushSpendingSyntaxTerm(context,(int)context.mTerm[name]);
		}
		else if(pos && context.data[pos] == synAssign)
		{
			assert(nodes.size());
			std::size_t index = nodes.size() -1;

			if(index >= 1 &&  nodes[index]->kind==coder::kCall)
			{
				coder::astCall * term = (coder::astCall *)nodes[index];
				if(term->name && wcscmp(term->name,L"parser->nextToken")==0) index--;
			}
			const wchar_t*  name = (const wchar_t*) context.data[pos+1] ;
			int varIndex = lookupVariant(context,name);
			if(varIndex <0)
			{
				varIndex = sureVariant(context,name);
				nodes[index] = coder::Variant((coder::astType*)coder::ContextTerm(getVariantType(context,pirorPos)),name,nodes[index]);
			}else
			{
				if(wcsstr(context.variantsTypes[varIndex].c_str(),L"TNodeList"))
					nodes[index] = coder::Call(L"parser->makeList",
						coder::List(coder::List(coder::ContextTerm(name)),nodes[index]));
				else
					nodes[index]= coder::Binary(opAsn,
						coder::ContextTerm(name),nodes[index]);
			}
		}
		else if(pos && context.data[pos] == synConstructor)
		{
			coder::astList* params = coder::List(coder::ContextTerm(L"ctx"));
			int apos = pos +1;
			const wchar_t* nodeVar = (const wchar_t*) context.data[apos++];
			while(context.data[apos])
				coder::List(params,coder::ContextTerm((const wchar_t*) context.data[apos++]));
			const wchar_t* nodeKind = nodeVar[0]=='$'?((coder::astTokenNode*)context.variantsInitials[lookupVariant(context,nodeVar)])->tk.str : nodeVar;
			wchar_t buf[128] = {0};
			int len = wcslen(nodeKind)>122? 122: (int)wcslen(nodeKind) +1; 
			wcsncpy(buf,nodeKind,len);
			buf[len] = 0;
			if(wcscmp(buf,L"assign")==0)
			{
				coder::astList* tm = params;
				if(!tm->term && tm->next) tm = tm->next;
				tm = tm->next; //skip ctx;

				const wchar_t*  name = ((coder::astTerm *)tm->term)->name;

				int varIndex = lookupVariant(context,name);
				if(varIndex <0)
				{
					varIndex = sureVariant(context,name);
					nodes.push_back(coder::Variant((coder::astType*)coder::ContextTerm(getVariantType(context,pirorPos)),name,tm->next->term));
				}else
				{
					if(wcsstr(context.variantsTypes[varIndex].c_str(),L"TNodeList"))
						nodes.push_back(coder::Call(L"parser->makeList",tm));
					else
						nodes.push_back(coder::Binary(opAsn,tm->term,tm->next->term));
				}

			}else
				nodes.push_back(coder::Call(wcsdup(buf),params));
		}
		else if(pos && context.data[pos] == synOption)
		{
			std::set<int> firstSet;
			getSyntaxFirstSet(context,term,pos + 1, firstSet);
			coder::astExpr* cond = getBranchCond(context, firstSet);
			coder::astList* trueStmt = coder::List();
			coder::List(trueStmt, analyseSyntax(context,term, pos+1,false,false/*bsure*/));

			nodes.push_back(coder::If(cond, trueStmt, 0));
		}
		else if(pos && context.data[pos] == synOneOf)
		{
			int apos = pos + 1;
			coder::astList * branchItems = coder::List();
			coder::astNode* defaultStmt = 0;
			while(true)
			{
				std::set<int> firstSet;
				if(context.data[apos]!=synGuard)
				{
					coder::astList* trueStmt = coder::List();
					getSyntaxFirstSet(context,term,apos, firstSet);
					coder::List(trueStmt, analyseSyntax(context,term, apos,false,false/*bsure*/));

					coder::List(branchItems,coder::BranchItem(getBranchCond(context,firstSet),
						trueStmt));
				}else
				{
					apos++;
					defaultStmt =  analyseSyntax(context,term, apos,false,true/*bsure*/);
				}

				apos = nextSyntaxPiece(context,term,apos);
				if(context.data[apos] == synNill)break;
			}
			//nodes.push_back(coder::Branch(0,branchItems,bbreak?coder::List(coder::Break()):0));
			nodes.push_back(coder::Branch(0,branchItems,defaultStmt));
		}
		else if(pos && context.data[pos] == synList)
		{
			int apos = pos + 1;
			std::set<int> firstSet;
			coder::astList * tnode = 
				coder::List(analyseSyntax(context,term, apos,context.data[apos]==synOneOf?true:false)); 
			apos = nextSyntaxPiece(context,term,apos);
			if(context.data[apos]!=synNill)
			{
				getSyntaxFirstSet(context,term,apos, firstSet);
				coder::astNode * cond = getBranchCond(context, firstSet);
				coder::List(tnode,coder::If((coder::astExpr*)coder::Unary(opNot,cond),coder::List(coder::Break()),0));
				coder::List(tnode,coder::Call(L"parser->nextToken",
						coder::List(coder::ContextTerm(L"tk"))));
			}else
			{
				getSyntaxFirstSet(context,term,pos + 1, firstSet);
				coder::astNode * cond = getBranchCond(context, firstSet);
				tnode = coder::List(coder::List(coder::If((coder::astExpr*)coder::Unary(opNot,cond),coder::List(coder::Break()),0)),tnode);
			}
			nodes.push_back(coder::Until(tnode, coder::Literal(true)));
		}
		else if(isPrimaryToken(context.data,pos) ||
			isOperatorToken(context.data,pos))
		{
			if(true)//if(bsure)
			{
				nodes.push_back(coder::Call(L"Ensure",
					coder::List(coder::List(
					coder::List(makeMatchToken(context,pos)),
					coder::Literal(context.data[pos])),
					context.data[pos]==tkKeyword?coder::Literal(getKeywordName(context,context.data[pos+1]).c_str()):
					coder::Literal(context.data[pos]>= 0x20? 0 : context.data[pos+1]))
					));
				bsure = false;
			}

			int apos = nextSyntaxToken(context, term, pos);
			if(context.data[apos]==synAssign)
				nodes.push_back(getVariantValue(context,pos));

			nodes.push_back(coder::Call(L"parser->nextToken",
				coder::List(coder::ContextTerm(L"tk"))));
		}
		pirorPos = pos;
		pos = nextSyntaxToken(context, term, pos);
		if(context.data[pos]==synNill) break;
	}
	for(std::size_t i=0;i<nodes.size();i++)
	{
		/*if(nodes[i]->kind==coder::kCall)
		{
			coder::astCall * term = (coder::astCall *)nodes[i];
			if(term->name && wcscmp(term->name,L"coder::TokenNode")==0) continue;
		}*/
		coder::List(stmt,nodes[i]);
	}
	return stmt;
}

int analyseTermSyntax(LangContext & context, int term)
{
	int pos = getSynTermHole(context,term,synSyntax);
	if(!pos) return 0;

	pos = context.data[pos];
	context.cTerm[term] = coder::List(coder::List(analyseSyntax(context, term, pos)),
		coder::Return((coder::astExpr *)coder::ContextTerm(L"$result")));
	return 1;
}

int analyseTermContainer(LangContext & context, int term)
{
	int i = context.data[getSynTermHole(context,term,synContainer)];

	std::vector<std::set<int>> firstSets;
	std::vector<std::wstring> terms;
	//fetch term and term firstset
	while(context.data[i] != synNill)
	{
		int term = context.data[i];
		terms.push_back(getTermName(context,term));
		analyseTerm(context,term);

		//process started keyword
		int pos = getSynTermHole(context,term,synSyntax);
		if(!pos) break;
		pos = context.data[pos];

		while(pos && context.data[pos] == synParam)
		{
			int paramIndex = context.data[pos+1];
			int paramPos = context.data[context.data[getSynTermHole(context,term,synParam)] + paramIndex];
			if(pos == paramPos) break;//dead loop
			pos = paramPos;
		}
		std::set<int> firstSet;
		getSyntaxFirstSet(context, term, pos, firstSet);
		firstSets.push_back(firstSet);
		i++;
	}
	
	coder::astList * branchItems = coder::List();
	for(int i=0;i<(int)firstSets.size();i++)
	{
		std::set<int>::const_iterator tr = firstSets[i].begin();
		int pos = *tr;
		coder::astList* param = coder::List(coder::ContextTerm(L"ctx"));
		coder::List(param, coder::ContextTerm(L"parser"));
		coder::List(param, coder::ContextTerm(L"tk"));
		coder::List(branchItems,coder::BranchItem(getBranchCond(context,firstSets[i]),
			coder::List(coder::Return(coder::Call(_wcsdup(((std::wstring)L"parse_" + terms[i]).c_str()),param)))));
	}
	coder::astList * stmt = coder::List();
	coder::List(stmt,coder::Variant((coder::astType*)coder::ContextTerm(L"TNodeList*"),
		L"$result",coder::Call(L"parser->makeList",0)));
	coder::List(stmt,coder::Branch(0,branchItems,0));
	coder::List(stmt,coder::Call(L"parser->skipLine",coder::List(0)));
	coder::List(stmt,coder::Return((coder::astExpr *)coder::ContextTerm(L"$result")));
	context.cTerm[term] = stmt;
	
	return 1;
}

int analyseTerm(LangContext & context, int term)
{
	context.variants.clear();
	context.variantsTypes.clear();
	context.variantsInitials.clear();
	int pos = getSynTermHole(context,term,synContainer);
	if(pos)
	{
		analyseTermContainer(context, term);
	}
	else
	{
		pos = getSynTermHole(context,term,synSyntax);
		if(pos) analyseTermSyntax(context,term);
	}

	return 1;
}


int analyse(LangContext & context)
{
	int ret = 1;
	context.spendingSyntaxTermIndex = 0;
	if(context.mTerm.find(L"unit")!= context.mTerm.end())
	{
		context.spendingSyntaxTerms.clear();
		analyseTerm(context,(int)context.mTerm[L"unit"]);
	}
	while(context.spendingSyntaxTermIndex < (int)context.spendingSyntaxTerms.size())
	{
		context.variants.clear();
		context.variantsTypes.clear();
		context.variantsInitials.clear();
		int t = context.spendingSyntaxTerms[context.spendingSyntaxTermIndex++];
		int pos = context.aTerm[t];
		if(pos)
		context.cTerm[t] = coder::List(coder::List(analyseSyntax(context,-1,pos)),
			coder::Return((coder::astExpr *)coder::ContextTerm(L"$result")));
	}
	return ret;
}

}//complier
}//trap
