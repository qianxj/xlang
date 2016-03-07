/*
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "v8.h"
#define V8_TARGET_ARCH_IA32

#include "disasm.h"
*/

#include "vlcontext.hpp"
#include "vlutils.hpp"
#include "vltoken.hpp"
#include "vlastToken.hpp"
#include "vlastfactory.hpp"
#include "vlparser.hpp"
#include "vlapply.hpp"

#include "process_wpp.hpp"

using namespace xlang;
extern int dbcselect(const wstring_t sqlStr, const wstring_t link);

bool vlApply::Initial()
{
	return true;
}

vlApply::vlApply(int ide):ident(ide)
{
	hashContext.enter_scope();
}

vlApply::~vlApply()
{
	hashContext.leave_scope(); 
}

void vlApply::addItem(int ide, int contextid,vlAstAxis* lhs ,vlAstAxis * rhs,vlAstAxis * code,int varIndex)
{
	Context context;
	context.lhs = lhs;
	context.rhs = rhs;
	context.code = code;
	context.id =  contextid;
	context.varCount = 0;
	context.varIndex = 0;
	if(code)
	{
		context.varCount = (int)variants.size() - varIndex;
		if(context.varCount)context.varIndex = varIndex;
	}
	this->items.push_back(context);
	int index = (int)this->items.size();
	this->mItems.insert(std::pair<int,int>(ide,index));
}

void vlApply::addAlias(int ident, vlAstAxis * lhs)
{
	this->mAlias.insert(std::pair<int,vlAstAxis*>(ident,lhs));
}

vlApply::Context & vlApply::getItem(int index)
{
	return items[ index - 1];
}

std::multimap<int, int > & vlApply::getMap()
{
	return mItems;
}

vlAstAxis * vlApply::makeLiteralNode(vlAstFactory* astFactory,vlAstNode * node)
{
	return astFactory->CreateAxis(node);
}

vlAstAxis * vlApply::makeVarItemNode(vlAstFactory* astFactory,int ide)
{
	return astFactory->CreateAxis(astFactory->CreateAstIdentifier(akProcVarItem,ide));
}

vlAstAxis * vlApply::makeItemNode(vlAstFactory* astFactory,int ide)
{
	return astFactory->CreateAxis(astFactory->CreateAstIdentifier(akProcItem,ide));
}
vlAstAxis * vlApply::makeCallNode(vlAstFactory* astFactory,int ide)
{
	return astFactory->CreateAxis(astFactory->CreateAstIdentifier(akProcCall,ide));
}

vlAstAxis * vlApply::makeCompBlock(vlAstFactory* astFactory)
{
	return astFactory->CreateAxis(astFactory->CreateAstNode(akCompBlock));
}

vlAstAxis * vlApply::makeNillNode(vlAstFactory* astFactory)
{
	return astFactory->CreateAxis(astFactory->CreateAstNode(akProcNill));
}
vlAstAxis * vlApply::makeThisNode(vlAstFactory* astFactory)
{
	return astFactory->CreateAxis(astFactory->CreateAstNode(akProcThis));
}
vlAstAxis * vlApply::makeBaseTypeNode(vlAstFactory* astFactory)
{
	return astFactory->CreateAxis(astFactory->CreateAstNode(akProcBaseType));
}

vlAstAxis * vlApply::makeNameNode(vlAstFactory* astFactory,int ide)
{
	return astFactory->CreateAxis(astFactory->CreateAstIdentifier(akProcName,ide));	
}

vlAstAxis * vlApply::makeNameNode(vlAstFactory* astFactory,wchar_t* name)
{
	return makeNameNode(astFactory,util::hashString(name));
}

vlAstAxis * vlApply::makeBoolToken(vlAstFactory* astFactory,bool v)
{	
	vlToken token;
	token.ty = tokenType::tBool;
	token.inum = v? 1: 0;
	return astFactory->CreateAxis(astFactory->CreateAstToken(token));
}

vlAstAxis * vlApply::makeIntegerToken(vlAstFactory* astFactory,int v)
{
	vlToken token;
	token.ty = tokenType::tInt;
	token.inum = v;
	return astFactory->CreateAxis(astFactory->CreateAstToken(token));
}

vlAstAxis * vlApply::makeDoubleToken(vlAstFactory* astFactory,double v)
{
	vlToken token;
	token.ty = tokenType::tDouble;
	token.dnum = v;
	return astFactory->CreateAxis(astFactory->CreateAstToken(token));
}

vlAstAxis * vlApply::makeIdentToken(vlAstFactory* astFactory,int ident)
{
	vlToken token;
	token.ty = tokenType::tIdent;
	token.inum = ident;
	token.snum = util::getHashString(ident);
	return astFactory->CreateAxis(astFactory->CreateAstIdentifier(akIdentifierNode,ident));
}

vlAstAxis * vlApply::makeStringToken(vlAstFactory* astFactory,const wchar_t* v)
{
	vlToken token;
	token.ty = tokenType::tString;
	token.inum = util::hashString((wchar_t *)v);
	token.snum = util::getHashString(token.inum);
	return astFactory->CreateAxis(astFactory->CreateAstToken(token));
}

vlAstAxis * vlApply::makeOperateToken(vlAstFactory* astFactory,const wchar_t* v)
{
	vlToken token;
	token.ty = tokenType::tOp; 
	if(v[1])
		token.inum = opValue::getOperateByString((wchar_t *)v);
	else
		token.inum = opValue::getOperateByToken(v[0]);
	token.snum = 0;
	return astFactory->CreateAxis(astFactory->CreateAstToken(token));
}

vlAstAxis * vlApply::makeIdentNode(vlAstFactory* astFactory,int ide)
{
	return astFactory->CreateAxis(astFactory->CreateAstIdentifier(akBnfIdentifier,ide));
}


vlAstAxis * vlApply::makeLiteralNode(class vlContext* context,vlAstNode * node)
{
	return makeLiteralNode(context->getAstFactory(),node);
}
vlAstAxis * vlApply::makeItemNode(class vlContext* context,int ide)
{
	return makeItemNode(context->getAstFactory(),ide);
}
vlAstAxis * vlApply::makeVarItemNode(class vlContext* context,int ide)
{
	return makeVarItemNode(context->getAstFactory(),ide);
}
vlAstAxis * vlApply::makeCallNode(class vlContext* context,int ide)
{
	return makeCallNode(context->getAstFactory(),ide);
}
vlAstAxis * vlApply::makeCompBlock(class vlContext* context)
{
	return makeCompBlock(context->getAstFactory());
}
vlAstAxis * vlApply::makeNillNode(class vlContext* context)
{
	return makeNillNode(context->getAstFactory());
}
vlAstAxis * vlApply::makeThisNode(class vlContext* context)
{
	return makeThisNode(context->getAstFactory());
}
vlAstAxis * vlApply::makeBaseTypeNode(class vlContext* context)
{
	return makeBaseTypeNode(context->getAstFactory());
}
vlAstAxis * vlApply::makeNameNode(class vlContext* context,int ide)
{
	return makeNameNode(context->getAstFactory(),ide);
}
vlAstAxis * vlApply::makeNameNode(class vlContext* context,wchar_t* name)
{
	return makeNameNode(context->getAstFactory(),name);
}
vlAstAxis * vlApply::makeBoolToken(class vlContext* context,bool v)
{
	return makeBoolToken(context->getAstFactory(),v);
}
vlAstAxis * vlApply::makeIntegerToken(class vlContext* context,int v)
{
	return makeIntegerToken(context->getAstFactory(),v);
}
vlAstAxis * vlApply::makeDoubleToken(class vlContext* context,double v)
{
	return makeDoubleToken(context->getAstFactory(),v);
}
vlAstAxis * vlApply::makeStringToken(class vlContext* context,const wchar_t* v)
{
	return makeStringToken(context->getAstFactory(),v);
}
vlAstAxis * vlApply::makeOperateToken(class vlContext* context,const wchar_t* v)
{
	return makeOperateToken(context->getAstFactory(),v);
}


int vlApply::proc_variant_item(vlContext* context,vlAstAxis * node,Variant &v,vlParser* parser)
{
	if(node->getIdent()!=util::hashString(L"apply-declare-item"))return -1;
	int ide = 0;
	int typ = 0;
	bool blist = false;
	int colList = 0;
	int colName = 0;
	int colType = 2;

	if(node->ChildNext(colList) && node->ChildNext(colList)->Node()->kind==akTokenNode)
	{
		vlAstToken * tk = (vlAstToken *)node->ChildNext(colList)->node;
		if(tk->getToken()->ty==tokenType::tIdent && tk->getToken()->inum==util::hashString(L"list"))
			blist = true;
		else 
			return -1;
	}
	if(blist)
	{
		colName++;
		colType++;
	}
	if(node->ChildNext(colName))
		ide = node->ChildNext(colName)->LastChild()->getIdent();

	if(!ide) return -1;

	v.id = ide;
	v.list = blist;

	//os_t::print(L"\n%s", node->printOwner().c_str());
	if(node->Next(colName + 2))
	{
		if(blist)colName--;
		if(node->Next(colName + 1)->Node()->kind==akTokenNode)
		{
			vlAstToken * tk = (vlAstToken *)node->Next(colName + 1)->node;
			if(tk->getToken()->ty==tokenType::tOp && tk->getToken()->inum==opValue::lparen)
			{
				//step to list
				vlAstAxis * tnode = node->Next(colName + 2)->Child(1);
				//os_t::print(L"\n%s", tnode->printOwner().c_str());
				while(tnode)
				{
					Variant field(0);
					if(proc_variant_item(context,tnode,field,parser)>=0)
					{
						if(!v.fields)v.fields = new std::vector<Variant>;
						v.fields->push_back(field);
					}
					tnode = tnode->getSibling();
				}
			}
		}
	}
	
	if(node->ChildNext(colType))
	{
		typ = node->ChildNext(colType)->LastChild()->getIdent();
		Handle hType = context->trap->getTypeHandle(typ);
		if(!hType.index)
		{
			os_t::print(L"\n%s类型未定义",util::getHashString(typ));
			//assert(false); //类型未定义
		}
		if(typ) v.typ = hType;
	}
	return 1;
}

int vlApply::proc_variant(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	//:apply-statement(:apply-declare-statement(var :apply-declare-item( list :apply-declare-itemname($items)) ';'))
	//:apply-declare-statement(var :apply-declare-item(:apply-declare-itemname($name) ':' :apply-declare-itemtype(string)) ';'))
	//:apply-declare-statement(var :apply-declare-item(:apply-declare-itemname($name)) ';'))
	//:apply-statement(:apply-declare-statement(var :apply-declare-item(:apply-declare-itemname($item))
	//		'(' [:apply-declare-item(:apply-declare-itemname(name)),:apply-declare-item(:apply-declare-itemname(typ)),:apply-declare-item(:apply-declare-itemname(value))] ')' ';'))
	Variant v(0);
	if(node->getIdent()!=util::hashString(L"apply-declare-statement"))return -1;
	//step to apply-declare-item
	int r = proc_variant_item(context,node->ChildNext(1),v,parser);
	if(r > 0)
	{
		//if(hashContext.find_token(v.id)<0)
		//{
			this->variants.push_back(v);
			int index = (int)variants.size();
			hashContext.push_word(index,v.id);
		//}else
		//{
			//int index = hashContext.get_word(hashContext.find_token(v.id));
			//variants[index - 1].val = 0;
		//}
	}
	return r;
}

vlAstAxis * vlApply::proc_statement(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	//process control statement
	//process expression statement
	//process declare statement
	
	//step into type statement
	vlAstAxis * result = 0;
	vlAstAxis * r = 0;
	if(!node) return 0;
	node = node->Child();
	int ide = node->getIdent();
	if(ide == util::hashString(L"apply-compound-statement"))
	{
		//step into statement list;
		node = node->ChildNext(1)->Child();
		while(node)
		{
			r = proc_statement(context,node,parser);
			if(r)result = result? result->appendSibling(r) : r;
			node = node->getSibling();
		}
		if(result && result->getSibling())
		{
			vlAstAxis * compBlock = makeCompBlock(context->getAstFactory());
			compBlock->setChild(result,false);
			result = compBlock;
		}
		return result;
	}
	else if(ide == util::hashString(L"apply-declare-statement"))
	{
		proc_variant(context,node,parser);
		return 0;
	}
	else if(ide == util::hashString(L"apply-expression-statement"))
	{
		return proc_expressionlist(context,node->Child(),parser);
	}
	else if(ide == util::hashString(L"apply-statement-if"))
	{
		return proc_stmt_if(context,node,parser);
	}
	else if(ide == util::hashString(L"apply-statement-while"))
	{
		return proc_stmt_while(context,node,parser);
	}
	else if(ide == util::hashString(L"apply-statement-break"))
	{
		return proc_stmt_break(context,node,parser);
	}
	else if(ide == util::hashString(L"apply-statement-continue"))
	{
		return proc_stmt_continue(context,node,parser);
	}
	else if(ide == util::hashString(L"apply-statement-return"))
	{
		return proc_stmt_return(context,node,parser);
	}
	else if(ide == util::hashString(L"apply-statement-let"))
	{
		return proc_stmt_let(context,node,parser);
	}
	else if(ide == util::hashString(L"apply-statement-foreach"))
	{
		return proc_stmt_foreach(context,node,parser);
	}
	else if(ide == util::hashString(L"apply-statement-context"))
	{
		//apply-statement-context
		vlAstAxis * result = makeCallNode(context->getAstFactory(),util::hashString(L"fn:enterContextEx"));
		vlAstAxis * arg = makeItemNode(context->getAstFactory(),node->ChildNext(2)->getIdent());
		arg->setParent(result);
		return result;
	}
	
	return 0;
}

vlAstAxis * vlApply::proc_stmt_if(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	//"if" '(' condition ')' trueblock "else" falseblcok ;'
	vlAstAxis * cond = node->ChildNext(2);
	vlAstAxis * trueblock = node->ChildNext(4);
	vlAstAxis * falseblock = 0;

	if(node->ChildNext(5) && node->ChildNext(5)->LastChild()->Node()->kind==akTokenNode)
	{
		vlAstToken * tk = (vlAstToken *)node->ChildNext(5)->LastChild()->Node();
		if(tk->getToken()->ty==tokenType::tIdent && tk->getToken()->inum==util::hashString(L"else"))
		{
			falseblock = node->ChildNext(6);
		}
	}

	//os_t::print(L"\n%s",cond->printOwner().c_str());

	cond = proc_expression(context,cond,parser);
	if(trueblock)trueblock = proc_statement(context,trueblock,parser);
	if(falseblock)falseblock = proc_statement(context,falseblock,parser);

	vlAstAxis * result = makeCallNode(context->getAstFactory(),util::hashString(L"fn:branch"));
	cond->setParent(result);
	cond->appendSibling(trueblock);
	if(falseblock)cond->appendSibling(falseblock);

	return result;
}

vlAstAxis * vlApply::proc_stmt_while(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	//"while" '(' condition ')' stmt ;'
	vlAstAxis * cond = node->ChildNext(2);
	vlAstAxis * stmt = node->ChildNext(4);

	cond = proc_expression(context,cond,parser);
	if(stmt)stmt = proc_statement(context,stmt,parser);

	vlAstAxis * result = makeCallNode(context->getAstFactory(),util::hashString(L"fn:while"));
	cond->setParent(result);
	cond->appendSibling(stmt);

	return result;
}

vlAstAxis * vlApply::proc_stmt_break(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	return makeCallNode(context->getAstFactory(),util::hashString(L"fn:break"));
}
vlAstAxis * vlApply::proc_stmt_continue(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	return makeCallNode(context->getAstFactory(),util::hashString(L"fn:continue"));
}

vlAstAxis * vlApply::proc_stmt_return(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	//"return" statement'
	vlAstAxis * stmt = node->ChildNext(1);

	vlAstAxis * stmtblock = proc_statement(context,stmt,parser);
	vlAstAxis * result = makeCallNode(context->getAstFactory(),util::hashString(L"fn:return"));
	stmtblock->setParent(result);
	return result;
}


vlAstAxis * vlApply::proc_stmt_foreach(class vlContext* context,vlAstAxis * node,vlParser* parser)
{
	//"for" identifier ["reverse"] "in"  apply-expression apply-statement  ;
	vlAstAxis * result = 0;
	
	//variant
	int ide = node->ChildNext(1)->getIdent();
	Variant v(0);
	v.id = ide;
	variants.push_back(v);
	int index = (int)variants.size();
	hashContext.push_word(index,v.id);

	if(node->ChildNext(2)->getToken() && parser->getTokenIdent(*node->ChildNext(2)->getToken())==util::hashString(L"reverse"))
	{
		result = makeCallNode(context->getAstFactory(),util::hashString(L"fn:vforeach"));
		result->appendChild(makeVarItemNode(context->getAstFactory(),hashContext.get_word(hashContext.find_token(ide))));
		result->appendChild(proc_expression(context,node->ChildNext(4),parser));
		result->appendChild(proc_statement(context,node->ChildNext(5),parser));
	}else
	{
		result = makeCallNode(context->getAstFactory(),util::hashString(L"fn:foreach"));
		result->appendChild(makeVarItemNode(context->getAstFactory(),hashContext.get_word(hashContext.find_token(ide))));
		result->appendChild(proc_expression(context,node->ChildNext(3),parser));
		result->appendChild(proc_statement(context,node->ChildNext(4),parser));
	}
	return result;
}
vlAstAxis * vlApply::proc_stmt_let(class vlContext* context,vlAstAxis * node,vlParser* parser)
{
	//"let" identifier '=' apply-expression ';';
	vlAstAxis * result = makeCallNode(context->getAstFactory(),util::hashString(L"fn:bind"));
	int ide = node->ChildNext(1)->getIdent();
	if(util::getHashString(ide)[0]=='$')
	{
		if(hashContext.find_token(ide)<0)
		{
			//parser->format_output(L"\n变量%s未定义.",util::getHashString(ide));
			//return makeNillNode(context->getAstFactory());
			Variant v(ide);
			this->variants.push_back(v);
			int index = (int)variants.size();
			hashContext.push_word(index,v.id);
		}
		vlAstAxis * t = makeVarItemNode(context->getAstFactory(),hashContext.get_word(hashContext.find_token(ide)));
		result->appendChild(t);
		result->appendChild(proc_expression(context,node->ChildNext(3),parser));
		return result;
	}
	else
	{
		parser->format_output(L"\n%s不是变量.",util::getHashString(ide));
		return makeNillNode(context->getAstFactory());
	}
}

vlAstAxis * vlApply::proc_action(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	//:apply-action
	if(node->getIdent() != util::hashString(L"apply-action"))return 0;
	//step into apply-statement
	node = node->Child();

	hashContext.enter_scope();
	vlAstAxis * result = proc_statement(context,node,parser);
	hashContext.leave_scope();
	return result;
}

vlAstAxis * vlApply::proc_field(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	//apply-call | identifier) { '[' identifier|integer-literal ']' }
	//os_t::print(L"\n%s",node->printOwner().c_str());
	vlAstAxis * result = 0;
	vlAstAxis * r = 0;
	int ide = node->getChild()->getIdent();
	if(ide == util::hashString(L"app-call"))
	{
		return proc_call(context,node->getChild(),parser);
	}else
	{
		//os_t::print(L"\npush %s",util::getHashString(ide));
		//os_t::print(L"\n%s",node->printOwner().c_str());
		if(util::getHashString(ide)[0]=='$')
		{
			if(hashContext.find_token(ide)<0)
			{
				parser->format_output(L"\n变量%s未定义.",util::getHashString(ide));
			}
			result = makeVarItemNode(context->getAstFactory(),hashContext.get_word(hashContext.find_token(ide)));
		}else
			result = makeItemNode(context->getAstFactory(),ide);


		if(node->ChildNext(1))
		{
			vlAstAxis * fnode = node->ChildNext(1);
			if(fnode->Node()->kind==akListNode)
			{
				fnode = fnode->Child();
				while(fnode)
				{
					//step into list
					int field = fnode->Next()->getIdent();
					//os_t::print(L".%s",util::getHashString(field));
					if(field)
						r = makeItemNode(context->getAstFactory(),field);
					else
						r = makeLiteralNode(context->getAstFactory(),fnode->Next()->Node());
					result->LastChild()->appendChild(r);
					fnode = fnode->getSibling();
				}
			}
		}
	}
	return result;
}

vlAstAxis * vlApply::proc_primary_expression(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	//apply-primary-expression		= literal | '(' apply-expression ')'  | apply-call | apply-field;
	int ide = node->Child()->getIdent();
	if(node->Child()->Node()->kind==akTokenNode)
	{
		vlToken & tk = *((vlAstToken *)node->Child()->Node())->getToken();
		if(tk.ty==tokenType::tBool  || tk.ty==tokenType::tDouble ||
			tk.ty==tokenType::tChar || tk.ty==tokenType::tInt ||
			tk.ty==tokenType::tString)
		{//match literal
			//push node->Child()->Node();
			//os_t::print(L"\npush %s;", node->Child()->Node()->printOwner().c_str());
			return makeLiteralNode(context->getAstFactory(),node->Child()->Node());
			
		}else if( tk.ty==tokenType::tOp && tk.inum == opValue::lparen)
		{
			return proc_expression(context,node->ChildNext(1),parser);
		}
	}
	else if(ide == util::hashString(L"apply-call"))
	{
		return proc_call(context,node->Child(),parser);
	}
	else if(ide == util::hashString(L"apply-field"))
	{
		return proc_field(context,node->Child(),parser);
	}

	return 0;
}

vlAstAxis * vlApply::proc_expression(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	return proc_primary_expression(context,node->Child(),parser);
}

vlAstAxis * vlApply::proc_expressionlist(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	vlAstAxis * result = 0;
	vlAstAxis * r = 0;
	if(!node->Child()) return 0;
	if(node->Child()->Node()->kind==akListNode)
	{
		vlAstAxis * expr = node->Child(2);
		while(expr)
		{
			r = proc_expression(context,expr,parser);
			if(r)result = result? result->appendSibling(r) : r;
			expr = expr->getSibling();
		}
	}else
		result = proc_expression(context,node->Child(),parser);
	return result;
}

vlAstAxis * vlApply::proc_call(vlContext* context,vlAstAxis * node,vlParser* parser)
{
	vlAstAxis * result = 0;
	vlAstAxis * r = 0;
	int ide = node->getChild()->getIdent();
	//os_t::print(L"\n%s",node->printOwner().c_str());
	result = makeCallNode(context->getAstFactory(),ide);

	//step into param_item
	vlAstAxis* paramList = node->ChildNext(2); //id '(' paramlist
	vlAstAxis* paramItem = paramList->Child(2);
	int argcount = 0;
	while(paramItem)
	{
		int ide = paramItem->getChild()->getIdent();
		bool nameparam = false;
		if(ide==util::hashString(L"apply-call-preargument"))
		{
			//os_t::print(L"\n%s",util::getHashString(v));
			nameparam = true;
		}
		if(nameparam)
		{
			//skip name?
			int name = paramItem->Child(2)->getIdent();
			r = makeNameNode(context->getAstFactory(),name);
			proc_expression(context,paramItem->ChildNext(1),parser)->setParent(r);
		}else
		{
			r = proc_expression(context,paramItem->Child(),parser);
		}
		argcount++;
		paramItem = paramItem->getSibling();
		if(!r) r=makeNillNode(context->getAstFactory());
		result->appendChild(r);
	}

	return result;
}

vlAstAxis * vlApply::eval(int & controlState,vlAstAxis * code,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind,bool child,bool bfirst)
{
	enum State : int { stNormal = 0,stReturn = 1, stContinue = 2,stBreak = 3};

	if(child)curNode = curNode->Child();
	child = false;

	std::vector<vlAstAxis *>  args;
	vlAstAxis * result = 0;
	while(code)
	{
		if(controlState) return result;

		args.clear();
		if(code->Node()->kind==akCompBlock)
		{
			vlAstAxis * cd = code->getChild();
			while(cd)
			{
				result = eval(controlState,cd,parser,context,curNode,paramBind,child,false);
				if(controlState) return result;
				cd = cd->getSibling();
			}
		}
		else if(code->Node()->kind==akProcCall)
		{
			int ide = code->getIdent();
			if(ide==util::hashString(L"fn:branch"))
			{
				vlAstAxis * node  = code->getChild();
				result = eval(node,parser,context,curNode,paramBind,child,false);
				
				if(result && (result->Node()->kind==akProcItem || result->Node()->kind==akProcVarItem))
					loadItemValue(context,result,paramBind);
				if(result && result->Node() && !(result->getToken() && !result->getToken()->inum))
				{
					result = eval(controlState,node->getSibling(),parser,context,curNode,paramBind,child,false);
					if(controlState) return result;
				}
				else if(node->getSibling() && node->getSibling()->getSibling())
				{
					result = eval(controlState,node->getSibling()->getSibling(),parser,context,curNode,paramBind,child,false);
					if(controlState) return result;
				}
			}
			else if(ide==util::hashString(L"fn:while"))
			{
				vlAstAxis * node  = code->getChild();
				while(true)
				{
					result = eval(node,parser,context,curNode,paramBind,child,false);
					if(result && (result->Node()->kind==akProcItem || result->Node()->kind==akProcVarItem))
						loadItemValue(context,result,paramBind);
					if(result && !(result->getToken() && !result->getToken()->inum))
					{
						result = eval(controlState,node->getSibling(),parser,context,curNode,paramBind,child,false);
						if(controlState==stReturn) return result;
						if(controlState==stBreak) break;
						if(controlState==stContinue)controlState = stNormal;
					}else
						break;
				}
				controlState = stNormal; 
			}
			else if(ide==util::hashString(L"fn:foreach"))
			{
				vlAstAxis * node  = code->getChild();
				if(node->Sibling(1)->Node()->kind==akProcVarItem &&variants[node->Sibling(1)->getIdent()-1].list)
				{
					int i = 0;
					vlAstAxis ** ptdata = ensureItemAddr(context,node->Sibling(1)); 
					int nLen = ((vlDynVector *)ptdata)->getLength();
					while(i < nLen)
					{
						vlAstAxis ** ptVar = ensureItemAddr(context,node); 
						* ptVar = *(vlAstAxis **)((vlDynVector *)ptdata)->getElement(i);
						result = eval(controlState,node->Sibling(2),parser,context,curNode,paramBind,child,false);
						if(controlState==stReturn) return result;
						if(controlState==stBreak) break;
						if(controlState==stContinue)controlState = stNormal;
						i++;
					}
					controlState = stNormal; 
				}else
				{
					if(node->Sibling(1))
					{
						vlAstAxis * expr = eval(controlState, node->Sibling(1),parser,context,curNode,paramBind,child,false);
						loadItemValue(context,expr,paramBind);
						vlAstAxis * anode = expr->Child();
						while(anode)
						{
							vlAstAxis ** ptVar = ensureItemAddr(context,node); 
							*ptVar = anode;
							result = eval(controlState, node->Sibling(2),parser,context,curNode,paramBind,child,false);
							if(controlState==stReturn) return result;
							if(controlState==stBreak) break;
							if(controlState==stContinue)controlState = stNormal;
							if(!anode->getSibling())break;
							anode = anode->getSibling();
						}
						controlState = stNormal;  
					}
				}
			} 
			else if(ide==util::hashString(L"fn:vforeach"))
			{
				vlAstAxis * node  = code->getChild();
				if(node->Sibling(1)->Node()->kind==akProcVarItem &&variants[node->Sibling(1)->getIdent()-1].list)
				{
					vlAstAxis ** ptdata = ensureItemAddr(context,node->Sibling(1)); 
					int nLen = ((vlDynVector *)ptdata)->getLength();
					int i = nLen;
					while(i > 0)
					{
						vlAstAxis ** ptVar = ensureItemAddr(context,node); 
						* ptVar = *(vlAstAxis **)((vlDynVector *)ptdata)->getElement(i - 1);
						result = eval(controlState,node->Sibling(2),parser,context,curNode,paramBind,child,false);
						if(controlState==stReturn) return result;
						if(controlState==stBreak) break;
						if(controlState==stContinue)controlState = stNormal;
						i--;
					}
					controlState = stNormal; 
				}else
				{
					if(node->Sibling(1))
					{
						vlAstAxis * expr = eval(controlState, node->Sibling(1),parser,context,curNode,paramBind,child,false);
						loadItemValue(context,expr,paramBind);
						vlAstAxis * anode = expr->Child();
						vector<vlAstAxis *> nodes;
						while(anode)
						{
							nodes.push_back(anode);
							anode = anode->getSibling();
						}
						int nLen = (int)nodes.size();
						int i = nLen;
						while(i>0)
						{
							vlAstAxis ** ptVar = ensureItemAddr(context,node); 
							*ptVar = nodes[i - 1];
							result = eval(controlState, node->Sibling(2),parser,context,curNode,paramBind,child,false);
							if(controlState==stReturn) return result;
							if(controlState==stBreak) break;
							if(controlState==stContinue)controlState = stNormal;
							i--;
						}
						controlState = stNormal;  
					}
				}
			} 
			else if(ide==util::hashString(L"fn:return"))
			{
				vlAstAxis * node  = code->getChild();
				result = eval(node,parser,context,curNode,paramBind,child,false);
				controlState = stReturn;
				return result;
			}else if(ide==util::hashString(L"fn:break"))
				controlState = stBreak; 
			else if(ide==util::hashString(L"fn:continue"))
				controlState = stContinue; 
			else
			{
				vlAstAxis * node  = code->getChild();
				while(node)
				{
					if(node->Node()->kind == akProcLiteral)
						args.push_back(node);
					else if(node->Node()->kind == akProcVarItem)
					{
						args.push_back(node);
					}
					else if(node->Node()->kind == akProcItem)
					{
						args.push_back(node);
						int ide = node->getIdent();
						if(getMap().find(ide) != getMap().end())
						{
							if(!getItem(getMap().lower_bound(ide)->second).lhs->ChildNext(2)) //no param
							{
								vlAstAxis *  r = parser->unify_apply(context,this,node);
								args.back() = r;
							}
						}
						else
						{
							int i = 0;
							vlApply * applyext = 0;
							for(i=0;i<(int)mProcessores.size();i++)
							{
								applyext  = context->applysvr.getApply(mProcessores[i].id);
								if(applyext && applyext->getMap().find(ide) != applyext->getMap().end())
								{
									if(!applyext->getItem(applyext->getMap().lower_bound(ide)->second).lhs->ChildNext(2)) //no param
									{
										vlAstAxis *  r = parser->unify_apply(context,applyext,node);
										args.back() = r;
										break;
									}
								}
							}
						}
					}
					else if(node->Node()->kind==akProcThis)
						args.push_back(node);
					else if(node->Node()->kind==akProcBaseType)
						args.push_back(node);
					else 
						args.push_back(eval(node,parser,context,curNode,paramBind,child,false));
					node = node->getSibling();
				}
				result = doCall(code,args,parser,context,curNode,paramBind);
			}
		}
		else if(code->Node()->kind==akProcLiteral)
			result = code;
		else if(code->Node()->kind==akProcVarItem)
			result = code;
		else if(code->Node()->kind==akProcItem)
			result = code;
		else if(code->Node()->kind==akProcThis)
			result = code;
		else if(code->Node()->kind==akProcName)
			result = eval(code->Child(),parser,context,curNode,paramBind);
		else if(code->Node()->kind==akProcBaseType)
			result = code;
		if(!bfirst) break;
		code = code->getSibling();
	}
	args.resize(0);
	return result ;
}

vlAstAxis * vlApply::eval(vlAstAxis * code,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind,bool child,bool bfirst)
{
	int controlState  = 0;
	return eval(controlState,code,parser,context,curNode,paramBind,child,bfirst);
}

vlAstAxis * vlApply::doEntryPointCall(vlAstAxis * node,std::vector<vlAstAxis *> &args,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind) throw(...)
{
	int ide = node->getIdent();
	if(ide==util::hashString(L"fn:epSureProcessApplyEntryPoint"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->sureProcessApplyEntryPoint(args[0]->getInteger()));
	}
	else if(ide==util::hashString(L"fn:epSureContextApplyEntryPoint"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->sureContextApplyEntryPoint(args[0]->getInteger()));
	}
	else if(ide==util::hashString(L"fn:epAddMatchCount"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->addMatchCount((void *)args[0]->getInteger(),args[1]->getInteger()));
	}else if(ide==util::hashString(L"fn:epAddMatchApplyName"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->addMatchApplyName((void *)args[0]->getInteger(),
			args[1]->getIdent()?args[1]->getIdent(): args[1]->getInteger()));
	}
	else if(ide==util::hashString(L"fn:epAddMatchLiteral"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->addMatchLiteral((void *)args[0]->getInteger(),
			args[1]->getInteger(),args[2]));
	}
	else if(ide==util::hashString(L"fn:epAddMatchName"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->addMatchName((void *)args[0]->getInteger(),
			args[1]->getInteger(),args[2]->getIdent()?args[2]->getIdent(): args[2]->getInteger()));
	}else if(ide==util::hashString(L"fn:epAddMatchAny"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->addMatchAny((void *)args[0]->getInteger(),
			args[1]->getInteger()));
	}else
	{
		wchar_t* str = util::getHashString(ide);
		parser->format_output(L"\n 函数%s未定义!",util::getHashString(ide));
	}
	return 0;

}

vlAstAxis * vlApply::doSymCall(vlAstAxis * node,std::vector<vlAstAxis *> &args,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind) throw(...)
{
	int ide = node->getIdent();
	if(ide==util::hashString(L"fn:symGetContext"))
	{
		return makeIntegerToken(context->getAstFactory(),(int)context);
	}
	else if(ide==util::hashString(L"fn:symGetNamespaceName"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),util::getHashString(context->trap->getNamespace(args[0]->getInteger())->name));
	}
	else if(ide==util::hashString(L"fn:symGetOleMethod"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),((SymOleMethod *)context->trap->getMethod(args[0]->getInteger()))->nMethod);
	}
	else if(ide==util::hashString(L"fn:symGetOlePropGet"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),((SymOleField *)context->trap->getField(args[0]->getInteger()))->propGet);
	}
	else if(ide==util::hashString(L"fn:symGetOlePropPut"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),((SymOleField *)context->trap->getField(args[0]->getInteger()))->propPut);
	}
	else if(ide==util::hashString(L"fn:symIsOleItem"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(!args[0]) return 0;
		return makeBoolToken(context->getAstFactory(),context->trap->isOleItem(args[0]->getInteger()));
	}
	else if(ide==util::hashString(L"fn:symGetTypeOleName"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),util::getHashString(context->trap->getTypeNSOleTerm(args[0]->getInteger())->oleName));
	}
	else if(ide==util::hashString(L"fn:symGetOleTypeInfo"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->getTypeOleTerm(args[0]->getInteger())->typeInfo);
	}
	else if(ide== util::hashString(L"fn:symGetSymTable"))
	{
		return makeIntegerToken(context->getAstFactory(),(int) &context->trap->symTable);
	}
	else if(ide == util::hashString(L"fn:symGetCodePc"))
	{
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->getCoder()->getCodePc());
	}
	else if(ide == util::hashString(L"fn:symGetMethodHandle"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int ide = args[1]->getIdent();
		return makeIntegerToken(context->getAstFactory(),(unsigned int)context->trap->getMethodHandle(args[0]->getInteger(),ide));
	}
	else if(ide == util::hashString(L"fn:symGetTypeKind"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(unsigned int)context->trap->getTypeKind(args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetMethodVariantHandle"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int ide = args[1]->getIdent();
		return makeIntegerToken(context->getAstFactory(),(unsigned int)context->trap->getMethodVariantHandle(args[0]->getInteger(),ide));
	}
	else if(ide == util::hashString(L"fn:symGetMethodParamHandle"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int ide = args[1]->getIdent();
		return makeIntegerToken(context->getAstFactory(),(unsigned int)context->trap->getMethodParamHandle(args[0]->getInteger(),ide));
	}
	else if(ide == util::hashString(L"fn:symGetMethodParamHandleByIndex"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(unsigned int)context->trap->getMethodParamHandleByIndex(args[0]->getInteger(),
			args[1]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetParamSpec"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),context->trap->getParam(args[0]->getInteger())->spec);
	}
	else if(ide == util::hashString(L"fn:symGetParamInitValue"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),context->trap->getParam(args[0]->getInteger())->initValue);
	}
	else if(ide == util::hashString(L"fn:symAddNamespace"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int ide = args[1]->getIdent();
		if(!ide) ide= args[1]->getInteger();
		return makeIntegerToken(context->getAstFactory(),(unsigned int)doAddNamespace(context,args[0]->getInteger(),ide));
	}
	else if(ide == util::hashString(L"fn:symAddType"))
	{
		assert(args.size()==3||args.size()==4);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int ide = args[2]->getIdent();
		if(!ide) ide= args[2]->getInteger();
		return makeIntegerToken(context->getAstFactory(),(unsigned int)doAddType(context,args[0]->getInteger(),args[1]->getInteger(),ide,
			args.size()==4?args[3]->getBoolean():true));
	}
	else if(ide == util::hashString(L"fn:symMakeTypeRef"))
	{
		return makeIntegerToken(context->getAstFactory(),(int)doMakeTypeRef(context));
	}
	else if(ide == util::hashString(L"fn:symGetMethodParamCount"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),context->trap->getMethod(args[0]->getInteger())->paramCount);
	}

	else if(ide == util::hashString(L"fn:symSetMethodPCode"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		context->trap->getMethod(args[0]->getInteger())->pcode = args[1];
		return 0;
	}
	else if(ide == util::hashString(L"fn:symGetMethodPCode"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return (vlAstAxis *)context->trap->getMethod(args[0]->getInteger())->pcode;
	}
	else if(ide == util::hashString(L"fn:symAddCompTypeItem"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		doAddCompTypeItem(context,args[0]->getInteger());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symAddCompTypeRank"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		doAddCompTypeRank(context,args[0]->getInteger());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symInsertCompTypeItem"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		doInsertCompTypeItem(context,args[0]->getInteger(),args[1]->getInteger());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symInsertCompTypeRank"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		doInsertCompTypeRank(context,args[0]->getInteger(),
				args[1]->getInteger());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symGetCompTypeItem"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),context->trap->getCompTypeItem(args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetCompTypeRank"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),context->trap->getCompTypeRank(args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symStepCompType"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),context->trap->stepCompType(args[0]->getInteger()));
	}

	else if(ide == util::hashString(L"fn:symAddField"))
	{
		assert(args.size()==4);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int ide = args[1]->getIdent();
		if(!ide) ide= args[1]->getInteger();
		return makeIntegerToken(context->getAstFactory(),(unsigned int)doAddField(context,args[0]->getInteger(),ide,args[2]->getInteger(),
			args[3]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symAddMethod"))
	{
		assert(args.size()==4);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int ide = args[1]->getIdent();
		return makeIntegerToken(context->getAstFactory(),(unsigned int)doAddMethod(context,args[0]->getInteger(),
			ide ? ide : args[1]->getInteger(),args[2]->getInteger(),
			args[3]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symAddParam"))
	{
		assert(args.size()==4);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int ide = args[1]->getIdent();
		return makeIntegerToken(context->getAstFactory(),(unsigned int)doAddParam(context,args[0]->getInteger(),ide,
			args[2]->getInteger(),args[3]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symAddVariant"))
	{
		assert(args.size()==4);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int ide = args[1]->getIdent();
		return makeIntegerToken(context->getAstFactory(),(unsigned int)doAddVariant(context,args[0]->getInteger(),ide,
			args[2]->getInteger(),args[3]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetTypeHandle"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(unsigned int)doGetTypeHandle(context,args[0]->isToken()?args[0]->getInteger():args[0]->getIdent()));
	}
	else if(ide == util::hashString(L"fn:symGetGlobalNamespace"))
	{
		assert(args.size()==0);
		return makeIntegerToken(context->getAstFactory(),(unsigned int)context->trap->getGlobalNamespace());
	}
	else if(ide == util::hashString(L"fn:symSetTypeKind"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		context->trap->getType(args[0]->getInteger())->kind = (xlang::TypeKind)args[1]->getInteger();
		return 0;
	}
	else if(ide == util::hashString(L"fn:symGetTypeKind")) 
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(unsigned int)context->trap->getType(args[0]->getInteger())->kind);
	}

	else if(ide == util::hashString(L"fn:symDump"))
	{
		parser->format_output((wstring_t)context->trap->dump().c_str());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symDumpType"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		parser->format_output((wstring_t)context->trap->dumpType(args[0]->getInteger()).c_str());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symDumpMethod"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		parser->format_output((wstring_t)context->trap->dumpMethod(args[0]->getInteger()).c_str());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symGetFieldHandle")) 
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(unsigned int)doGetFieldHandle(context,
			args[0]->getInteger(),args[1]->getIdent()));
	}
	else if(ide == util::hashString(L"fn:symGetTypeMethodHandle")) 
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(unsigned int)doGetTypeMethodHandle(context,
			args[0]->getInteger(),args[1]->getIdent()));
	}
	else if(ide == util::hashString(L"fn:symGetTypeAllocSize"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),doGetTypeAllocSize(context,args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetFieldAllocSize"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)doGetFieldAllocSize(context,args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetOffsetOfField"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)doGetOffsetOfField(context,args[0]->getInteger(),args[1]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symSetNSOleTerm"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		context->trap->setNSOleTerm(args[0]->getInteger(),args[1]->getInteger());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symAddNSOleTerm"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),context->trap->addNSOleTerm(args[0]->getInteger(),0,0));
	}
	else if(ide == util::hashString(L"fn:symGetOffsetOfTerm"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)doGetOffsetOfTerm(context,args[0]->getInteger(),args[1]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symAddBase"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		doAddBase(context,args[0]->getInteger(),args[1]->getInteger());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symGetTermNameID"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),context->trap->getTermNameID(args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetTermName"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(!args[0]) return 0;
		return makeStringToken(context->getAstFactory(),(wchar_t *)context->trap->getTermName(args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetTermType"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(!args[0]) return 0;
		return makeIntegerToken(context->getAstFactory(),(unsigned int)context->trap->getTermType(args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetTermSpec"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(!args[0]) return 0;
		return makeIntegerToken(context->getAstFactory(),context->trap->getTermSpec(args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetTermInitValue"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(!args[0]) return 0;
		return makeIntegerToken(context->getAstFactory(),context->trap->getTermInitValue(args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symSetTermInitValue"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(!args[0]) return 0;
		context->trap->setTermInitValue(args[0]->getInteger(),args[1]->getInteger());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symGetTypeTermFrameSize"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(!args[0]) return 0;
		//bug
		return makeIntegerToken(context->getAstFactory(),context->trap->getTypeTermFrameSize(args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symDumpType"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),(wchar_t *)context->trap->dumpType(args[0]->getInteger()).c_str());
	}
	else if(ide == util::hashString(L"fn:symLookupTypeItem"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(args[1]->getIdent())
			return makeIntegerToken(context->getAstFactory(),(int)context->trap->lookupItem(args[0]->getInteger(),
				args[1]->getIdent()));
		else
			return makeIntegerToken(context->getAstFactory(),(int)context->trap->lookupItem(args[0]->getInteger(),
				args[1]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symLookupType"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(args[1]->getIdent())
			return makeIntegerToken(context->getAstFactory(),(int)context->trap->lookupType(args[0]->getInteger(),
				args[1]->getIdent()));
		else
			return makeIntegerToken(context->getAstFactory(),(int)context->trap->lookupType(args[0]->getInteger(),
				args[1]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symLookupMethod"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(args[1]->getIdent())
			return makeIntegerToken(context->getAstFactory(),(int)context->trap->lookupMethod(args[0]->getInteger(),
				args[1]->getIdent()));
		else
			return makeIntegerToken(context->getAstFactory(),(int)context->trap->lookupMethod(args[0]->getInteger(),
				args[1]->getInteger()));

	}
	else if(ide == util::hashString(L"fn:symLookupItem"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(args[1]->getIdent())
			return makeIntegerToken(context->getAstFactory(),(int)context->trap->lookupItem(args[0]->getInteger(),
				args[1]->getIdent()));
		else
			return makeIntegerToken(context->getAstFactory(),(int)context->trap->lookupItem(args[0]->getInteger(),
				args[1]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetParent"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->getParent(args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symIsSuperClass"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->isSuperClass(args[0]->getInteger(),
			args[1]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symAddReal"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->addReal(args[0]->getDouble()));
	}
	else if(ide == util::hashString(L"fn:symGetRealPtr"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->getRealPtr(args[0]->getDouble()));
	}
	else if(ide == util::hashString(L"fn:symDumpMethod"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),(wchar_t *)context->trap->dumpMethod(args[0]->getInteger()).c_str());
	}
	else if(ide == util::hashString(L"fn:symDumpField"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),(wchar_t *)context->trap->dumpField(args[0]->getInteger()).c_str());
	}
	else if(ide == util::hashString(L"fn:symDumpParam"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),(wchar_t *)context->trap->dumpParam(args[0]->getInteger()).c_str());
	}
	else if(ide == util::hashString(L"fn:symDumpVariant"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),(wchar_t *)context->trap->dumpVariant(args[0]->getInteger()).c_str());
	}
	else if(ide == util::hashString(L"fn:symDumpTypeParam"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),(wchar_t *)context->trap->dumpTypeParam(args[0]->getInteger()).c_str());
	}
	else if(ide == util::hashString(L"fn:symGetMethodAddr"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->getMethodAddr(args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetMethodSpec"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->getMethod(args[0]->getInteger())->spec);
	}
	else if(ide == util::hashString(L"fn:symSetMethodAddr"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		context->trap->setMethodAddr((unsigned int)args[0]->getInteger(),(void *)args[1]->getInteger());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symSetMethodNativeFrom"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		context->trap->setMethodNativeFrom((unsigned int)args[0]->getInteger(),args[1]->getStringID());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symSetMethodNativeAlias"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		context->trap->setMethodNativeAlias((unsigned int)args[0]->getInteger(),args[1]->getStringID());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symGetMethodNativeFrom"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),context->trap->getMethodNativeFrom((unsigned int)args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetMethodNativeAlias"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),context->trap->getMethodNativeAlias((unsigned int)args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symIsMethodNative"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeBoolToken(context->getAstFactory(),context->trap->isMethodNative((unsigned int)args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symSetMethodNative"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		context->trap->setMethodNative((unsigned int)args[0]->getInteger(),0,0);
		return 0;
	}
	//------
	else if(ide == util::hashString(L"fn:symSetTypeNativeFrom"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		context->trap->setTypeNativeFrom((unsigned int)args[0]->getInteger(),args[1]->getStringID());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symSetTypeNativeAlias"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		context->trap->setTypeNativeAlias((unsigned int)args[0]->getInteger(),args[1]->getStringID());
		return 0;
	}
	else if(ide == util::hashString(L"fn:symGetTypeNativeFrom"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),context->trap->getTypeNativeFrom((unsigned int)args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetTypeNativeAlias"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),context->trap->getTypeNativeAlias((unsigned int)args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symFindOleChild"))
	{
		assert(args.size()==2);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),context->trap->findOleChild(args[0]->getInteger(),args[1]->getIdent()));
	}
	else if(ide == util::hashString(L"fn:symFindMatchedMethod")) 
	{
		for(int i=0; i<(int)args.size();i++)
		{
			if(i==3)continue;
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		
		vlAstAxis ** ptdata = ensureItemAddr(context,args[3]); 
		int nlen =  ((vlDynVector *)ptdata)->getLength();
		int start = args[4]->getInteger();
		std::vector<HType> vArgs;
		for(int i=start;i<nlen;i++)
		{
			vArgs.insert(vArgs.begin(),(*(vlAstAxis **)((vlDynVector *)ptdata)->getElement(i))->getInteger());
		}
		int ident = context->trap->getMethod(args[2]->getInteger())->name;
		return makeIntegerToken(context->getAstFactory(),(unsigned int)context->trap->findMatchedMethod(
			/*args[0]->getInteger(),*/
			context->trap->getParent(args[2]->getInteger()),
			args[1]->getInteger(),ident, nlen - start,vArgs));
	}
	else if(ide == util::hashString(L"fn:symGetMethodSpec")) 
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),context->trap->getMethodSpec((unsigned int)args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetMethodVariantSize"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),context->trap->getMethodVariantSize((unsigned int)args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:symGetMethodParamSize"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),context->trap->getMethodParamSize((unsigned int)args[0]->getInteger()));
	}else
	{
		wchar_t* str = util::getHashString(ide);
		parser->format_output(L"\n 函数%s未定义!",util::getHashString(ide));
	}
	return 0;
}

vlAstAxis * vlApply::doCall(vlAstAxis * node,std::vector<vlAstAxis *> &args,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind) throw(...)
{
	int ide = node->getIdent();
	const wchar_t * str = util::getHashString(ide);
	if(ide && getMap().find(ide) != getMap().end())
	{
		args.insert(args.begin(),node);
		args[0] = doMakeNode(node,context,args,paramBind);
		while((int)args.size()>1) args.pop_back();
		//paramBind.clear();
		return doApply(node,args,parser,context,curNode,paramBind);
	}else if(ide && this->mProcessores.size())
	{
		int i = 0;
		vlApply * applyext = 0;
		for(i=0;i<(int)mProcessores.size();i++)
		{
			applyext  = context->applysvr.getApply(mProcessores[i].id);
			if(applyext && applyext->getMap().find(ide) != applyext->getMap().end())
			{
				args.insert(args.begin(),node);
				args[0] = doMakeNode(node,context,args,paramBind);
				while((int)args.size()>1) args.pop_back();
				//paramBind.clear();
				return applyext->doApply(node,args,parser,context,curNode,paramBind);
			}
		}
	}

	if(ide==util::hashString(L"fn:wpp_process"))
	{
		return WppProcess(context,this,parser,node,args,paramBind);
	}
	else if(ide==util::hashString(L"fn:enterContextEx"))
	{
		assert(args.size()==1);
		if(context->contexts.size()) context->contexts.pop_back();
		if(args[0]->getIdent()!=util::hashString(L"nill"))
			context->contexts.push_back(args[0]->getIdent());
		return 0;
	}
	if(ide==util::hashString(L"fn:enterContext"))
	{
		assert(args.size()==1);
		context->contexts.push_back(args[0]->getIdent());
		return 0;
	}
	else if(ide==util::hashString(L"fn:leaveContext"))
	{
		assert(args.size()==1);
		context->contexts.pop_back();
		return 0;
	}
	else if(ide==util::hashString(L"fn:Disassemble"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		std::wstring str;
		if(context->trap->getCoder()->Disassemble((unsigned char *)context->trap->getMethodAddr(args[0]->getInteger()),str))
		{
			return this->makeStringToken(context->getAstFactory(),(wchar_t *)str.c_str());
		}
		return this->makeStringToken(context->getAstFactory(),L"");
	}
	else if(wcsstr(util::getHashString(ide),L"fn:sym"))
	{
		return doSymCall(node,args,parser,context,curNode,paramBind);
	}
	else if(wcsstr(util::getHashString(ide),L"fn:ep"))
	{
		return doEntryPointCall(node,args,parser,context,curNode,paramBind);
	}
	else if(ide == util::hashString(L"fn:bind"))
	{
		doBind(node,context,args,paramBind);
		return 0;
	}
	else if(ide == util::hashString(L"fn:push"))
	{
		doPush(node,context,args,paramBind);
		return 0;
	}
	else if(ide == util::hashString(L"fn:pop"))
	{
		doPop(node,context,args,paramBind);
		return 0;
	}
	else if(ide == util::hashString(L"fn:list"))
	{
		return doMakeListNode(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:node"))
	{
		return doMakeNode(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:print"))
	{
		doPrint(node,context,args,paramBind,parser);
		return 0;
	}
	else if(ide == util::hashString(L"fn:islist"))
	{
		loadItemValue(context,args[0],paramBind);
		vlAstAxis * node  = args[0];
		if(node->Node()->kind==akProcVarItem &&variants[node->getIdent() - 1].list)
			return makeBoolToken(context->getAstFactory(),true);
		if(node->Node()->kind==akListNode) 
			return makeBoolToken(context->getAstFactory(),true);
		return makeBoolToken(context->getAstFactory(),false);
	}
	else if(ide == util::hashString(L"fn:opt"))
	{
		for(int i=1;i<(int)args.size();i++)
		{
			loadItemValue(context,args[i],paramBind);
		}
		int ide = args[0]->getIdent();
		if(paramBind.find(ide)==paramBind.end())
		{
			if((int)args.size()==2)
				args[0] = args[2];
			else
				args[0] = makeIntegerToken(context->getAstFactory(),0);
			paramBind.insert(std::pair<int,vlAstAxis*>(ide,args[0]));
			return args[0];
		}
		loadItemValue(context,args[0],paramBind);
		return args[0];
	}

	else if(ide == util::hashString(L"fn:trace"))
	{
		doTrace(node,context,args,paramBind,parser); 
		return 0;
	}
	else if(ide == util::hashString(L"fn:throw"))
	{
		loadItemValue(context,args[0],paramBind);
		throw(args[0]->getString());
		return 0;
	}
	else if(ide == util::hashString(L"fn:printex"))
	{
		doTypeText(node,context,args,paramBind);
		return 0;
	}
	else if(ide == util::hashString(L"fn:apply"))
	{
		return doApply(node,args,parser,context,curNode,paramBind);
	}
	else if(ide == util::hashString(L"fn:vapply"))
	{
		return doVApply(node,args,parser,context,curNode,paramBind);
	}
	else if(ide == util::hashString(L"fn:getRealPtr"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)context->trap->getRealPtr(args[0]->getDouble()));
	}
	else if(ide == util::hashString(L"fn:Gt"))
	{
		return makeBoolToken(context->getAstFactory(),Gt(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:Gte"))
	{
		return makeBoolToken(context->getAstFactory(),Gte(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:Lt"))
	{
		return makeBoolToken(context->getAstFactory(),Lt(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:Lte"))
	{
		return makeBoolToken(context->getAstFactory(),Lte(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:Eq"))
	{
		return makeBoolToken(context->getAstFactory(),Eq(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:Neq"))
	{
		return makeBoolToken(context->getAstFactory(),Neq(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:Nil"))
	{
		return makeBoolToken(context->getAstFactory(),Nil(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:Not"))
	{
		return makeBoolToken(context->getAstFactory(),Not(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:And"))
	{
		return makeBoolToken(context->getAstFactory(),And(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:Or"))
	{
		return makeBoolToken(context->getAstFactory(),Or(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:toString"))
	{
		return toString(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:concat"))
	{
		return Concat(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:sqlselect"))
	{
		return SqlSelect(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Add"))
	{
		return Add(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Sub"))
	{
		return Sub(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Mul"))
	{
		return Mul(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Div"))
	{
		return Div(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Mod"))
	{
		return Mod(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Shr"))
	{
		return Shr(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Shl"))
	{
		return Shl(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Band"))
	{
		return Band(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Bor"))
	{
		return Bor(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Bnot"))
	{
		return Bnot(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Xor"))
	{
		return Xor(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Inc"))
	{
		return Inc(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Dec"))
	{
		return Dec(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:Neg"))
	{
		return Neg(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:IsIdentifier"))
	{
		return makeBoolToken(context->getAstFactory(),doIsIdentifier(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:IsLiteral"))
	{
		return makeBoolToken(context->getAstFactory(),doIsLiteral(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:IsNumber"))
	{
		return makeBoolToken(context->getAstFactory(),doIsNumber(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:IsDouble"))
	{
		return makeBoolToken(context->getAstFactory(),doIsDouble(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:IsInteger"))
	{
		return makeBoolToken(context->getAstFactory(),doIsInteger(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:IsChar"))
	{
		return makeBoolToken(context->getAstFactory(),doIsChar(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:IsBool"))
	{
		return makeBoolToken(context->getAstFactory(),doIsBool(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:IsString"))
	{
		return makeBoolToken(context->getAstFactory(),doIsString(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:IsCall"))
	{
		return makeBoolToken(context->getAstFactory(),doIsCall(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:IsTerm"))
	{
		return makeBoolToken(context->getAstFactory(),doIsTerm(node,context,args,paramBind));
	}
	else if(ide == util::hashString(L"fn:top"))
	{
		return doTop(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:topex"))
	{
		return doTopEx(node,context,args,paramBind);
	}
	else if(ide == util::hashString(L"fn:item"))
	{
		int i = 1;
		if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
			loadItemValue(context,args[i],paramBind);
		
		if(args[0]->Node()->kind==akProcVarItem &&variants[args[0]->getIdent()-1].list)
		{
			vlAstAxis ** ptdata = ensureItemAddr(context,args[0]); 
			int nIndex = ((vlAstToken*)args[1]->Node())->getToken()->inum;
			int nLen = ((vlDynVector *)ptdata)->getLength();
			assert(nLen - 1 - nIndex >= 0);
			ptdata =  (vlAstAxis **)((vlDynVector *)ptdata)->getElement(nIndex);
			return *ptdata;
		}else
		{
			loadItemValue(context,args[0],paramBind);
			int index = args[1]->getInteger();
			if(index==0) return args[0]->getChild();
			vlAstAxis * node = args[0]->getChild();
			while(index>0 && node)
			{
				node = node->getSibling();
				index--;
			}
			return node;
		}
	}
	else if(ide == util::hashString(L"fn:itemex"))
	{
		int i = 1;
		if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
			loadItemValue(context,args[i],paramBind);
		
		if(args[0]->Node()->kind==akProcVarItem &&variants[args[0]->getIdent()-1].list)
		{
			vlAstAxis ** ptdata = ensureItemAddr(context,args[0]); 
			int nIndex = ((vlAstToken*)args[1]->Node())->getToken()->inum;
			int nLen = ((vlDynVector *)ptdata)->getLength();
			assert(nLen - 1 - nIndex >= 0);
			ptdata =  (vlAstAxis **)((vlDynVector *)ptdata)->getElement(nIndex);
			return *ptdata;
		}else
		{
			loadItemValue(context,args[0],paramBind);
			int index = args[1]->getInteger();
			if(index==0) return args[0]->getChild();
			vlAstAxis * node = args[0]->getChild();
			while(index>0 && node)
			{
				node = node->getNext();
				index--; 
			}
			return node;
		}
	}
	else if(ide == util::hashString(L"fn:sizeex"))
	{
		assert(args.size()==1);
		if(args[0]->Node()->kind==akProcVarItem &&variants[args[0]->getIdent()-1].list)
		{
			vlAstAxis ** ptdata = ensureItemAddr(context,args[0]); 
			int nLen = ((vlDynVector *)ptdata)->getLength();
			return makeIntegerToken(context->getAstFactory(),nLen);
		}else
		{
			loadItemValue(context,args[0],paramBind);
			vlAstAxis * node = args[0]->getChild();
			int count = 0;
			while(node)
			{
				count++;
				node = node->getNext();
			}
			return makeIntegerToken(context->getAstFactory(),count);
		}
	}
	else if(ide == util::hashString(L"fn:size"))
	{
		assert(args.size()==1);
		if(args[0]->Node()->kind==akProcVarItem &&variants[args[0]->getIdent()-1].list)
		{
			vlAstAxis ** ptdata = ensureItemAddr(context,args[0]); 
			int nLen = ((vlDynVector *)ptdata)->getLength();
			return makeIntegerToken(context->getAstFactory(),nLen);
		}else
		{
			loadItemValue(context,args[0],paramBind);
			vlAstAxis * node = args[0]->getChild();
			int count = 0;
			while(node)
			{
				count++;
				node = node->getSibling();
			}
			return makeIntegerToken(context->getAstFactory(),count);
		}
	}
	else if(ide == util::hashString(L"fn:setsize"))
	{
		assert(args.size()==2);
		int i = 1;
		if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
			loadItemValue(context,args[i],paramBind);
		
		vlAstAxis ** ptdata = ensureItemAddr(context,args[0]); 
		if(!args[1]->getToken())
		{
			parser->format_output(L"\nset size error %s :",util::getHashString(args[1]->getIdent()));
		}
		assert(args[1]->getInteger() < ((vlDynVector *)ptdata)->getLength());
		((vlDynVector *)ptdata)->setLength(args[1]->getInteger());
		return 0;
	}
	else if(ide == util::hashString(L"fn:settop"))
	{
		 doSetTop(node,context,args,paramBind);
		 return 0;
	}
	else if(ide == util::hashString(L"fn:hash"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeIntegerToken(context->getAstFactory(),(int)doHash(args[0]->getString()));
	}
	else if(ide == util::hashString(L"fn:getHashString"))
	{
		assert(args.size()==1);
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return makeStringToken(context->getAstFactory(),(wchar_t * )doGetHashString(args[0]->getInteger()));
	}
	else if(ide == util::hashString(L"fn:SavePoint"))
	{
		context->getAstFactory()->checkpoint();
		return 0;
	}
	else if(ide == util::hashString(L"fn:RestorePoint"))
	{
		context->getAstFactory()->restorepoint();
		return 0;
	}
	else if(ide == util::hashString(L"fn:AllocPages"))
	{
		return makeIntegerToken(context->getAstFactory(),context->getAstFactory()->blocks_->size());
	}
	else if(ide == util::hashString(L"fn:MakeTypeDataNode"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  MakeTypeDataNode(context,args,paramBind);
	}else if(ide == util::hashString(L"fn:MakeDataBufferNode"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  MakeDataBufferNode(context,args,paramBind);
	}else if(ide == util::hashString(L"fn:removeChildNode"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  makeBoolToken(context->getAstFactory(),args[0]->removeChildNode(args[1]));
	}else if(ide == util::hashString(L"fn:AppendSiblingNode"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  AppendSiblingNode(args[0],args[1]);
	}else if(ide == util::hashString(L"fn:AppendNextNode"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  AppendNextNode(args[0],args[1]);
	}else if(ide == util::hashString(L"fn:AppendChildNode"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  AppendChildNode(args[0],args[1]);
	}
	else if(ide == util::hashString(L"fn:makeAxis"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return context->getAstFactory()->CreateAxis(args[0]->Node());
	}
	else if(ide == util::hashString(L"fn:setValue"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(args[0]->getToken())
		{
			args[0]->node = args[1]->node;
			return args[0];
		}else
			return 0;
	}else if(ide == util::hashString(L"fn:sibling"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  getSibling(args[0]);
	}else if(ide == util::hashString(L"fn:next"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  getNext(args[0]);
	}else if(ide == util::hashString(L"fn:name"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  makeIntegerToken(context->getAstFactory(),args[0]->getIdent());
	}
	else if(ide == util::hashString(L"fn:identNode"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(args[0]->getIdent())
			return  makeIdentNode(context->getAstFactory(),args[0]->getIdent());
		else
			return  makeIdentNode(context->getAstFactory(),args[0]->getInteger());

	}else if(ide == util::hashString(L"fn:ident"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(args[0]->getIdent())
			return  makeIdentToken(context->getAstFactory(),args[0]->getIdent());
		else
			return  makeIdentToken(context->getAstFactory(),args[0]->getInteger());

	}else if(ide == util::hashString(L"fn:child"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  getChild(args[0]);
	}
	else if(ide == util::hashString(L"fn:getNode"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  args[0];
	}	
	else if(ide == util::hashString(L"fn:parent")) 
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  getParent(args[0]); 
	}	
	else if(ide == util::hashString(L"fn:TextUrl")) 
	{
		return makeStringToken(context->getAstFactory(),parser->pirorSoureUrl.c_str());
		//return makeStringToken(context->getAstFactory(),parser->lexer->srcPosition.srcUrl);
	}	
	else if(ide == util::hashString(L"fn:TextRow"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(args[0]->LastFirstChild()->getToken())
			return makeIntegerToken(context->getAstFactory(),args[0]->LastFirstChild()->getToken()->row + 1);
		return makeIntegerToken(context->getAstFactory(),-1);
	}	
	else if(ide == util::hashString(L"fn:TextCol"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if(args[0]->LastFirstChild()->getToken())
			return makeIntegerToken(context->getAstFactory(),args[0]->LastFirstChild()->getToken()->col);
		return makeIntegerToken(context->getAstFactory(),-1);
	}	
	else if(ide == util::hashString(L"fn:LastChild"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  LastChild(args[0]);
	}	
	else if(ide == util::hashString(L"fn:ChildNext"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  ChildNext(args[0],args[1]->getInteger());
	}	
	else if(ide == util::hashString(L"fn:Child"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  Child(args[0],args[1]->getInteger());
	}	
	else if(ide == util::hashString(L"ChildSibling"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  Child(args[0],args[1]->getInteger());
	}else if(ide == util::hashString(L"fn:curRegister"))
	{
		return  makeIntegerToken(context->getAstFactory(),context->trap->getCoder()->curRegister());
	}else if(ide == util::hashString(L"fn:askRegister"))
	{
		return  makeIntegerToken(context->getAstFactory(),context->trap->getCoder()->askRegister());
	}else if(ide == util::hashString(L"fn:freeRegister"))
	{
		context->trap->getCoder()->freeRegister();
		return  0;
	}else if(ide == util::hashString(L"fn:clearRegister"))
	{
		context->trap->getCoder()->clearRegister();
		return  0;
	}else if(ide == util::hashString(L"fn:getRegisterCount"))
	{
		return  makeIntegerToken(context->getAstFactory(),context->trap->getCoder()->getRegisterCount());
	}

	else if(ide == util::hashString(L"fn:context_enter_scope"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		context_enter_scope(context);
		return 0;
	}	
	else if(ide == util::hashString(L"fn:context_leave_scope"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		context_leave_scope(context);
		return 0;
	}	
	else if(ide == util::hashString(L"fn:ptr"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  makeIntegerToken(context->getAstFactory(),(int)args[0]);
	}	
	else if(ide == util::hashString(L"fn:vptr"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return (vlAstAxis *)args[0]->getInteger();
	}	
	else if(ide == util::hashString(L"fn:context_push_word"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int ide = args[1]->getIdent();
		return  makeIntegerToken(context->getAstFactory(),context_push_word(context,
			args[0]->getInteger(),ide ? ide : args[1]->getInteger()));
	}	
	else if(ide == util::hashString(L"fn:context_get_word"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  makeIntegerToken(context->getAstFactory(),context_get_word(context,args[0]->getInteger()));
	}	
	else if(ide == util::hashString(L"fn:context_find_token"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  makeIntegerToken(context->getAstFactory(),context_find_token(context,args[0]->getIdent()?args[0]->getIdent():args[0]->getInteger()));
	}	
	else if(ide == util::hashString(L"fn:context_find_next_token"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  makeIntegerToken(context->getAstFactory(),context_find_next_token(context,args[0]->getInteger(),args[1]->getIdent()));
	}	
	else if(ide == util::hashString(L"fn:emitInteger"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  makeIntegerToken(context->getAstFactory(),context->trap->getCoder()->emit(args[0]->getInteger()));
	}	
	else if(ide == util::hashString(L"fn:emitChar"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  makeIntegerToken(context->getAstFactory(),context->trap->getCoder()->emit((unsigned char)args[0]->getInteger()));
	}	
	else if(ide == util::hashString(L"fn:emitShort"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		return  makeIntegerToken(context->getAstFactory(),context->trap->getCoder()->emit((short)args[0]->getInteger()));
	}	
	else if(ide == util::hashString(L"fn:emitPtr"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int v = args[0]->getInteger();
		assert(sizeof(void*)==sizeof(int));
		return  makeIntegerToken(context->getAstFactory(),context->trap->getCoder()->emit(*(void **)&v));
	}	
	else if(ide == util::hashString(L"fn:debugbreak"))
	{
		//DebugBreak();
		return  0;
	}else if(ide ==	util::hashString(L"fn:label"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if((int)args.size()==2)
			context->trap->getCoder()->label(context->sureLabel(args[0]->getIdent()),args[1]->getBoolean());
		else
			context->trap->getCoder()->label(context->sureLabel(args[0]->getIdent()));
		return 0;
	}else if(ide ==	util::hashString(L"fn:labelas"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int v1 = args[0]->getInteger();
		int v2 = args[1]->getInteger();
		context->trap->getCoder()->labelas(v1,v2);
		return 0;
	}
	else if(ide ==	util::hashString(L"fn:sureLabel"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		int v = args[0]->getIdent();
		return makeIntegerToken(context->getAstFactory(),context->sureLabel(v));
	}
	else if(ide ==	util::hashString(L"fn:toLabel"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind); 
		}
		int v = args[0]->getInteger();
		if((int)args.size()==2)
			context->trap->getCoder()->toLabel(v,args[1]->getBoolean());
		else
			context->trap->getCoder()->toLabel(v);
		return 0;
	}
	else if(ide ==	util::hashString(L"fn:function"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		if((int)args.size()==2)
			return makeIntegerToken(context->getAstFactory(),(int)os_t::load_module(util::getHashString(args[0]->getIdent())
						,util::getHashString(args[1]->getIdent())));
		else if((int)args.size()==1)
		{
			if(context->trap->mFunctions.find(args[0]->getIdent())!= context->trap->mFunctions.end())
				return makeIntegerToken(context->getAstFactory(),*(int *)&context->trap->mFunctions[args[0]->getIdent()]);
			else
				return 0;
		}
		else
			return 0;
	}
	else if(ide ==	util::hashString(L"fn:code-eval"))
	{
		for(int i=0;i<(int)args.size();i++)
		{
			if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
				loadItemValue(context,args[i],paramBind);
		}
		/*
		vlCoder::Label * p = context->findLabel(args[0]->getIdent());
		if(p)
		{
			if(p->initial)

			{
				void * faddr = context->trap->getCoder()->hCodeSpace->code + p->dest;
				_asm call faddr;
			}
		}
		*/
		return 0;
	}
	else
	{
		wchar_t* str = util::getHashString(ide);
		parser->format_output(L"\n 函数%s未定义!",util::getHashString(ide));
		//throw "函数%s未定义!";
	}

	return 0; 
}

int vlApply::doTypeText(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	for(int i=0;i<(int)args.size();i++)
	{
		loadItemValue(context,args[i],paramBind);
		if(args[i])os_t::print(L"%s",args[i]->Node()->printOwner().c_str());
	}
	return 0;
}

int vlApply::doPrint(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind,vlParser* parser)
{
	for(int i=0;i<(int)args.size();i++)
	{
		if(args[i])
		{
			loadItemValue(context,args[i],paramBind);
			std::wstring str;
			if(args[i] && args[i]->Node()->kind==akTokenNode)
			{
				vlToken & token = *args[i]->Node()->getToken(); 
				wchar_t buf[255];
				if(token.ty == tokenType::tString)
				{
					 if(token.snum) str = str + token.snum ;
				}
				else if(token.ty == tokenType::tInt)
				{
					swprintf_s(buf,255,L"%d",token.inum);
					str = buf;
				}
				else if(token.ty == tokenType::tDouble)
				{
					swprintf_s(buf,255,L"%f",token.dnum);
					str= buf;
				}
				else if(token.ty == tokenType::tBool)
				{
					if(token.inum)
						str=L"true";
					else
						str=L"false"; 
				}
				else if(token.ty == tokenType::tChar)
				{
					swprintf_s(buf,255,L"%c",token.inum);
					str = buf ;
				}
				else if(token.ty == tokenType::tIdent)
				{
					str=  token.snum;
				}
				else if(token.ty == tokenType::tOp)
				{
					str =  opValue::getOperateText(token.inum);
				}
				parser->format_output(L"%s",str.c_str());
			}else
			{
				parser->format_output(L"%s",args[i]->Node()->printOwner().c_str());
			}
		}
	}
	return 0;
}

int vlApply::doTrace(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind,vlParser* parser)
{
	for(int i=0;i<(int)args.size();i++)
	{
		if(args[i])
		{
			loadItemValue(context,args[i],paramBind);
			parser->format_output(L"%s",args[i]->printOwnerOnly().c_str());
		}
	}
	return 0;
}

int vlApply::doBind(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	/*int id = args[0]->getIdent();
	int index = hashContext.find_token(id);
	assert(index >0);
	*/
	int i = 1;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
		loadItemValue(context,args[i],paramBind);
	
	*(this->ensureItemAddr(context,args[0])) = args[1];

	return 1;
}

int vlApply::doPush(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	//int id = args[0]->getIdent();
	//int index = hashContext.find_token(id);
	//assert(index >0);
	
	//assert(args[1]->Node()->kind==akProcVarItem);

	vlDynVector * v = (vlDynVector *)this->ensureItemAddr(context,args[0]);
	int len = v->getEleSize();
	vlAstAxis** ele = (vlAstAxis**)v->newElemnt();
	if(args[1] && (args[1]->Node()->kind==akProcItem || args[1]->Node()->kind==akProcVarItem))
	{
		vlAstAxis** src = this->ensureItemAddr(context,args[1]);
		if(src)
			memcpy_s(ele,len,src,len);
		else
		{
			assert(len == 4);
			loadItemValue(context,args[1],paramBind);
			memcpy_s(ele,len,&args[1],len);
		}
	}else
	{
		assert(len == 4);
		memcpy_s(ele,len,&args[1],len);
	}

	return 1;
}

int vlApply::doPop(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	assert(args[0]->Node()->kind==akProcVarItem);

	vlDynVector * v = (vlDynVector *)this->ensureItemAddr(context,args[0]);
	if(v)v->pop();

	return 0;
}

bool vlApply::doIsIdentifier(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	int i = 0;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
		loadItemValue(context,args[i],paramBind);

	bool unify = false;
	if(!args[0]->LastChildNode())
	{
		if(args[0]->kind==akIdentifierNode)unify = true;
		if(args[0]->kind==akTokenNode)
		{
			vlToken & tk = *args[0]->Node()->getToken();
			if(tk.ty == tokenType::tIdent)unify = true;
		}
	}
	else if(args[0]->LastChildNode()->kind==akIdentifierNode)unify = true;
	else if(args[0]->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *args[0]->LastChildNode()->getToken();
		if(tk.ty == tokenType::tIdent)unify = true;
	}
	return unify ;
}

bool vlApply::doIsLiteral(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	int i = 0;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
		loadItemValue(context,args[i],paramBind);

	bool unify = false;
	if(!args[0]->LastChildNode())
	{
		if(args[0]->kind==akTokenNode)
		{
			vlToken & tk = *args[0]->Node()->getToken();
			if(tk.ty == tokenType::tBool || tk.ty == tokenType::tDouble ||
				tk.ty == tokenType::tInt || tokenType::tString || tk.ty==tokenType::tChar)
				unify = true;
		}
	}
	else if(args[0]->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *args[0]->LastChildNode()->getToken();
		if(tk.ty == tokenType::tBool || tk.ty == tokenType::tDouble ||
			tk.ty == tokenType::tInt || tokenType::tString || tk.ty==tokenType::tChar)
			unify = true;
	}

	return unify ;
}

bool vlApply::doIsNumber(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	int i = 0;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
		loadItemValue(context,args[i],paramBind);

	bool unify = false;
	if(args[0]->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *args[0]->Node()->getToken();
		if(tk.ty == tokenType::tDouble ||
			tk.ty == tokenType::tInt)
			unify = true;
	}

	return unify ;
}
bool vlApply::doIsDouble(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	int i = 0;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
		loadItemValue(context,args[i],paramBind);

	bool unify = false;
	if(args[0]->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *args[0]->Node()->getToken();
		if(tk.ty == tokenType::tDouble )
			unify = true;
	}

	return unify ;
}
bool vlApply::doIsInteger(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	int i = 0;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
		loadItemValue(context,args[i],paramBind);

	bool unify = false;
	if(args[0]->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *args[0]->Node()->getToken();
		if(tk.ty == tokenType::tInt )
			unify = true;
	}

	return unify ;
}
bool vlApply::doIsChar(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	int i = 0;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
		loadItemValue(context,args[i],paramBind);

	bool unify = false;
	if(args[0]->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *args[0]->Node()->getToken();
		if(tk.ty==tokenType::tChar)
			unify = true;
	}
	return unify ;
}

bool vlApply::doIsBool(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	int i = 0;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
		loadItemValue(context,args[i],paramBind);

	bool unify = false;
	if(args[0]->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *args[0]->Node()->getToken();
		if(tk.ty == tokenType::tBool)
			unify = true;
	}

	return unify ;
}
bool vlApply::doIsString(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	int i = 0;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
		loadItemValue(context,args[i],paramBind);

	bool unify = false;
	if(args[0]->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *args[0]->Node()->getToken();
		if(tk.ty == tokenType::tString)
			unify = true;
	}
	return unify ;
}

bool vlApply::doIsCall(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	int i = 0;
	if(args[i] && args[i]->Node()->kind == akProcCall) return true;

	return false ;
}

bool vlApply::doIsTerm(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	int i = 0;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem)) return true;

	return false ;
}

vlAstAxis * vlApply::doTop(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);

	int i = 1;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
		loadItemValue(context,args[i],paramBind);
	
	if(args[0]->Node()->kind==akProcVarItem &&variants[args[0]->getIdent()-1].list)
	{
		vlAstAxis ** ptdata = ensureItemAddr(context,args[0]); 
		int nIndex = ((vlAstToken*)args[1]->Node())->getToken()->inum;
		int nLen = ((vlDynVector *)ptdata)->getLength();
		assert(nLen - 1 - nIndex >= 0);
		if(nLen - 1 - nIndex < 0)
		{
			std::wstring msg;
			wchar_t buf[60];
			swprintf_s(buf,60,L"%d",nIndex);
			msg += (std::wstring)L"fn:top out bound of index " + buf;
			if(context->stackItems.size())
				msg += (wstring)L" at apply '" + context->stackItems.back().ide +L"'.";
			throw wcsdup(msg.c_str());
		}
		ptdata =  (vlAstAxis **)((vlDynVector *)ptdata)->getElement(nLen - 1 - nIndex);
		return *ptdata;
	}else
	{
		loadItemValue(context,args[0],paramBind);
		//if(args[1]->getInteger() == 0) return args[0]->getChild();
		int count = 0;
		vlAstAxis * node = args[0]->getChild();
		while(node)
		{
			count++;
			node = node->getSibling();
		}
		int index = count - 1 - args[1]->getInteger();
		int i = 0;
		node = args[0]->getChild();
		while(node)
		{
			if(i==index) return node;
			node = node->getSibling();
			i++;
		}
		return 0;
	}
	
	
}

vlAstAxis * vlApply::doTopEx(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);

	int i = 1;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
		loadItemValue(context,args[i],paramBind);
	
	if(args[0]->Node()->kind==akProcVarItem &&variants[args[0]->getIdent()-1].list)
	{
		vlAstAxis ** ptdata = ensureItemAddr(context,args[0]); 
		int nIndex = ((vlAstToken*)args[1]->Node())->getToken()->inum;
		int nLen = ((vlDynVector *)ptdata)->getLength();
		assert(nLen - 1 - nIndex >= 0);
		if(nLen - 1 - nIndex < 0)
		{
			std::wstring msg;
			wchar_t buf[60];
			swprintf_s(buf,60,L"%d",nIndex);
			msg += (std::wstring)L"fn:top out bound of index " + buf;
			if(context->stackItems.size())
				msg += (wstring)L" at apply '" + context->stackItems.back().ide +L"'.";
			throw wcsdup(msg.c_str());
		}
		ptdata =  (vlAstAxis **)((vlDynVector *)ptdata)->getElement(nLen - 1 - nIndex);
		return *ptdata;
	}else
	{
		loadItemValue(context,args[0],paramBind);
		//if(args[1]->getInteger() == 0) return args[0]->getChild();
		int count = 0;
		vlAstAxis * node = args[0]->getChild();
		while(node)
		{
			count++;
			node = node->getNext();
		}
		int index = count - 1 - args[1]->getInteger();
		int i = 0;
		node = args[0]->getChild();
		while(node)
		{
			if(i==index) return node;
			node = node->getNext();
			i++;
		}
		return 0;
	}
	
	
}


int vlApply::doSetTop(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	int i = 1;
	if(args[i] && (args[i]->Node()->kind==akProcItem||args[i]->Node()->kind==akProcVarItem))
		loadItemValue(context,args[i],paramBind);
	
	vlAstAxis ** ptdata = ensureItemAddr(context,args[0]); 
	//int nIndex = ((vlAstToken*)args[1]->Node())->getToken()->inum;
	int nLen = ((vlDynVector *)ptdata)->getLength();
	assert(nLen  >= 0);
	ptdata =  (vlAstAxis **)((vlDynVector *)ptdata)->getElement(nLen - 1 );
	
	*ptdata = args[1];
	return 0;
}


vlAstAxis * vlApply::doApply(vlAstAxis * node,std::vector<vlAstAxis *> &args,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	int id = args[0]->getIdent();
	vlAstAxis * anode= 0;
	if(!args[0]) return 0;
	if(args[0])loadItemValue(context,args[0],paramBind);
	anode = args[0];
	if(!anode)
	{
		std::wstring msg;
		msg += (std::wstring)L"apply nill node ";
		if(context->stackItems.size())
			msg += (wstring)L" at apply '" + context->stackItems.back().ide +L"'.";
		throw wcsdup(msg.c_str());
	}
	if(anode->Node()->kind==akIdentifierNode ||
		anode->Node()->kind==akTokenNode) return anode;

	vlAstAxis *  r = parser->unify_apply(context,this,anode);
	
	//if(!r) os_t::print(L"\n%s",paramBind[id]->printOwner().c_str());
	return r;
}

vlAstAxis * vlApply::doVApply(vlAstAxis * node,std::vector<vlAstAxis *> &args,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	int id = args[0]->getIdent();
	vlAstAxis * anode= 0;
	if(!args[0]) return 0;
	if(args[0])loadItemValue(context,args[0],paramBind);
	anode = args[0];
	/*
	if( paramBind.find(id)== paramBind.end())
	{
		//os_t::print(L"%s",util::getHashString(id));
		//assert(false);
		//return 0;
		anode = args[0];
	}else
		anode = paramBind[id];*/
	if(anode->Node()->kind==akIdentifierNode ||
		anode->Node()->kind==akTokenNode) return anode;

	if(anode->node->kind == akListNode)
	{
		std::vector<vlAstAxis*> nodes;

		vlAstAxis* node = anode->getChild();
		vlAstAxis* rnode = 0;
		while(node)
		{
			nodes.push_back(node);
			node = node->getSibling();
		}
		for(int i=(int)nodes.size() - 1;i>=0;i--)
		{
			node = nodes[i];
			if(node->Node()->kind == akTokenNode || node->Node()->kind == akIdentifierNode)
				rnode = parser->unify_apply_call(context,this,anode,node,paramBind,false);
			else
				rnode = parser->unify_apply(context,this,node);
		}
		return rnode;
	} else
	{
		vlAstAxis *  r = parser->unify_apply(context,this,anode);
		return r;
	}
}

void *  vlApply::makeVariantFrame(vlContext* context,Variant & v)
{
	int size = sizeof(void*);
	if(v.typ)
	{
		Handle h(v.typ);
		if(h.kind=skCompType)
		{
			//this only process x[]
			v.list = true;
			Handle htype(skType,context->trap->symTable.comptypes[h.index]);
			size = context->trap->getAllocSize(htype);
		}else
			size = context->trap->getAllocSize(h);
	}else
	{
		if(!v.fields && !v.list) return 0; //不创建对象引用
		if(v.fields)size = (int)v.fields->size() * (int)sizeof(void *);
	}
	if(v.list)
	{
		vlDynVector * t = new vlDynVector();
		t->setEleSize(size);
		return t;
	}else
	{
		 unsigned char* p = new unsigned char[size];
		 memset(p,0,size);
		 return p;
	}
}

vlAstAxis ** vlApply::ensureItemAddr(vlContext* context, vlAstAxis * arg)
{
	if(arg && arg->Node()->kind!=akProcVarItem) return 0;
	vlAstAxis ** ptdata = 0;
	//int ide = arg->getIdent();
	//int index = hashContext.find_token(ide);
	int index = arg->getIdent();
	Variant  vt(0);
	if(index>0)
	{
		int cindex = 1;
		Variant* v = &variants[index - 1];
		if( context->stackItems.size() && context->stackItems.back().varcount &&
			items[context->stackItems.back().item].varCount &&
			items[context->stackItems.back().item].varIndex + 1 <= index &&
			items[context->stackItems.back().item].varIndex + 1 + items[context->stackItems.back().item].varCount > index)
		{
			//local variant
			if((v->typ || v->fields || v->list)) 
			{
				if(!context->variants[context->stackItems.back().variant +
					index - 1 - items[context->stackItems.back().item].varIndex].astAxis)
				{
					context->variants[context->stackItems.back().variant +
						index - 1- items[context->stackItems.back().item].varIndex].astAxis = (xlang::vlAstAxis *)makeVariantFrame(context,*v);
				}
			}
			ptdata = (v->typ || v->fields || v->list) ? 
				(vlAstAxis **)context->variants[context->stackItems.back().variant +
				index - 1 - items[context->stackItems.back().item].varIndex].astAxis :
				& context->variants[context->stackItems.back().variant +
					index - 1 - items[context->stackItems.back().item].varIndex].astAxis;
		}else
		{  
			if((v->typ || v->fields || v->list) && !v->val)
				v->val = (vlAstAxis *)makeVariantFrame(context,*v);
			ptdata = (v->typ || v->fields || v->list) ? (vlAstAxis **)v->val : (vlAstAxis **)&v->val;
		}

		bool blist = true;
		while(arg->Child(cindex))
		{
			int ide = arg->Child(cindex)->getIdent();
			if(!ide)//vector index
			{
				assert(v->list);
				int nIndex = ((vlAstToken*)arg->Child(cindex)->Node())->getToken()->inum;
				ptdata =  (vlAstAxis **)((vlDynVector *)ptdata)->getElement(nIndex - 1);
				cindex++;
				blist = false;
			}else
			{
				if(v->list & blist)
				{
					//last position is current position
					int nIndex = ((vlDynVector *)ptdata)->getLength();
					assert(((vlDynVector *)ptdata)->getLength());
					ptdata =  (vlAstAxis **)((vlDynVector *)ptdata)->getElement(nIndex - 1);
				}
				blist = true;
				if(v->typ)
				{
					Handle h(v->typ);
					bool tolist = false;
					if(h.kind==skCompType)
						h = Handle(skType,context->trap->symTable.comptypes[h.index]);

					Handle hField = context->trap->getFieldHandle(h,ide);
					Handle ht(context->trap->getField(h)->hType);
					
					vt.id = ide;
					vt.typ = ht;
					vt.fields = 0;
					if(ht.kind==skCompType)
					{
						vt.list = true;
						ht = Handle(skType,context->trap->symTable.comptypes[h.index]);
					}
					if(vt.list || (context->trap->getType(ht)->kind==tkClass))
					{
						
					}
				}
				else if(v->fields)
				{
					int i = 0;
					int len = (int)v->fields->size();
					for(i=0;i<(int)v->fields->size();i++)
					{
						if((*(v->fields))[i].id==ide)
						{
							cindex++;
							ptdata = ptdata +  i ;
							v = &(*(v->fields))[i];
							if((v->fields || v->list) )
							{
								if(!*ptdata)
									*ptdata = (xlang::vlAstAxis *)makeVariantFrame(context,*v);
								ptdata = (vlAstAxis **)*ptdata;
							}
							break;
						}
					}
					if(i==len)
					{
						assert(false);
						break;
					}
				}
			}
		}
	}
	return ptdata;
}

int vlApply::loadItemValue(vlContext * context,vlAstAxis * & arg,std::map<int,vlAstAxis*> & paramBind)
{
	if(arg && (arg->Node()->kind==akProcItem || arg->Node()->kind==akProcVarItem))
	{
		if(arg->Node()->kind==akProcVarItem)
		{
			int index = arg->getIdent();//hashContext.find_token(ide);
			if(index>0)
			{
				vlAstAxis ** ptdata = (vlAstAxis **)ensureItemAddr(context,arg);
				arg = *ptdata;
			}
		}
		else
		{
			int ide = arg->getIdent();
			if(paramBind.find(ide)!=paramBind.end())
				arg = paramBind[ide];
			/*else
			{
				os_t::print(L"\n--undef param %s.",util::getHashString(ide));
				arg = makeIntegerToken(0);
			}*/
		}
	}
	return 1;
}

vlAstAxis * vlApply::doMakeListNode(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	const wchar_t * str = util::getHashString(args[0]->getIdent());
	vlAstAxis * result = context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstNode(akListNode));
	for(int i=0;i<(int)args.size();i++)
	{
		if(args[i])
		{
			loadItemValue(context,args[i],paramBind);

			vlAstAxis * t = context->getAstFactory()->CreateAxis(args[i]->Node());
			t->child = args[i]->child;
			result->appendChild(t);
		}
	}
	return result;
}

vlAstAxis * vlApply::doMakeNode(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	const wchar_t * str = util::getHashString(args[0]->getIdent());
	vlAstAxis * result = context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstIdentifier(akBnfIdentifier,args[0]->getIdent()));
	for(int i=1;i<(int)args.size();i++)
	{
		loadItemValue(context,args[i],paramBind);
		if(args[i])
		{
			vlAstAxis * t = context->getAstFactory()->CreateAxis(args[i]->Node());
			t->child = args[i]->child;
			result->appendChildNext(t);
		}else
		{
			std::wstring msg;
			wchar_t buf[60];
			swprintf_s(buf,60,L"%d",i);
			msg += (std::wstring)L"make node '" + str +L"'argument " + buf;
			if(context->stackItems.size())
				msg += (wstring)L" at apply '" + context->stackItems.back().ide +L"'.";
			throw wcsdup(msg.c_str());
		}
	}
	return result;
}

std::wstring vlApply::dumpCode(vlAstAxis * code,bool bfirst)
{
	std::wstring str;
	
	while(code)
	{
		if(code->Node()->kind==akProcCall)
		{
			str += (std::wstring)util::getHashString(code->Node()->getIdent())+L"(";
			vlAstAxis * node  = code->getChild();
			bool first = true;
			while(node)
			{
				if(!first) str += L",";
				first = false;
				str += dumpCode(node,false);
				node = node->getSibling();
			}
			str += (std::wstring)L")";
		}
		else if(code->Node()->kind==akProcLiteral)
		{
			str += code->Node()->printOwner();
		}
		else if(code->Node()->kind==akProcItem)
		{
			str += code->Node()->printOwner();
			vlAstAxis * node  = code->getChild();
			while(node)
			{
				str += L"[";
				str += dumpCode(node,false);
				node = node->getChild();
				str += L"]";
			}
		}
		else if(code->Node()->kind==akProcThis)
		{
			str += (std::wstring)L"this";
		}
		else if(code->Node()->kind==akProcName)
		{
			str += code->Node()->printOwner() + L":" + dumpCode(code->getChild(),false);
		}
		else if(code->Node()->kind==akProcBaseType)
		{
			str += (std::wstring)L"base";
		}
		if(!bfirst) break;
		code = code->getSibling();
		if(code)str += (std::wstring)L"\n";
	}

	return str;
}

Handle vlApply::doAddNamespace(vlContext * context,Handle hAxis,int Id)
{
	return context->trap->addNamespace(hAxis,Id);
}

Handle vlApply::doAddType(vlContext * context,Handle hAxis,int kind, int Id,bool todef)
{
	return context->trap->addType(hAxis,(TypeKind)kind,Id,todef);
}

Handle vlApply::doMakeTypeRef(vlContext * context)
{
	Handle hMyType(skCompType,(int)context->trap->symTable.comptypes.size()+ 1) ;
	return hMyType;
}
Handle vlApply::doAddField(vlContext * context, Handle hAxis,int id,unsigned spec,Handle typeref)
{
	return context->trap->addField(hAxis, id,spec,typeref);
}

Handle vlApply::doAddMethod(vlContext * context, Handle hAxis,int id, unsigned spec,Handle hType)
{
	return context->trap->addMethod(hAxis, id,spec,hType);
}
Handle vlApply::doAddVariant(vlContext * context,HMethod hMethod, int id, unsigned spec,Handle hType)
{
	return context->trap->addVariant(hMethod,id,spec,hType);
}
Handle vlApply::doAddParam  (vlContext * context,HMethod hMethod, int id, unsigned spec,Handle hType)
{
	return context->trap->addParam(hMethod,id,spec,hType);
}

Handle vlApply::doGetTypeHandle(vlContext * context,int typeId)
{
	return  context->trap->getTypeHandle(typeId);
}

Handle vlApply::doGetFieldHandle(vlContext * context,Handle hType,int fieldId)
{
	return context->trap->getFieldHandle(hType,fieldId);
}

Handle vlApply::doGetTypeMethodHandle(vlContext * context,Handle hType,int id)
{
	return context->trap->getMethodHandle(hType,id);
}


int	vlApply::doGetTypeAllocSize(vlContext * context,Handle hType)
{
	int msize = 0;
	return context->trap->getAllocSize(hType,msize);
}

int	vlApply::doGetFieldAllocSize(vlContext * context,Handle hField)
{
	return context->trap->getFieldAllocSize(hField);
}

int vlApply::doGetOffsetOfField(vlContext * context,Handle hType,Handle hField)
{
	return context->trap->getFieldOffset(hType,hField);
}

int vlApply::doGetOffsetOfTerm(vlContext * context,Handle hMethod,Handle hTerm)
{
	if(hMethod.kind==skType||hMethod.kind==skNType) return doGetOffsetOfField(context,hMethod,hTerm);
	return context->trap->getFrameOffset(hMethod,hTerm);
}

void vlApply::doAddBase(vlContext * context,Handle hType,Handle hTypeBase)
{
	return context->trap->addTypeBase(hType,hTypeBase);
}

void vlApply::doAddCompTypeItem(vlContext * context,Handle hType)
{
	return context->trap->addCompTypeItem(hType);
}

void vlApply::doAddCompTypeRank(vlContext * context,int rank)
{
	return context->trap->addCompTypeRank(rank);
}

void vlApply::doInsertCompTypeItem(vlContext * context,Handle hCompType,Handle hType)
{
	return context->trap->insertCompTypeItem(hCompType,hType);
}

void vlApply::doInsertCompTypeRank(vlContext * context,Handle hCompType,int rank)
{
	return context->trap->insertCompTypeRank(hCompType,rank);
}

int vlApply::doHash(const wchar_t * str)
{
	return util::hashString((const wstring_t)str);
}

wchar_t * vlApply::doGetHashString(int id)
{
	return util::getHashString(id);
}

bool vlApply::Gt(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);
	if(args[0]->isNumber() && args[1]->isNumber())
	{
		if(args[0]->isDouble() || args[1]->isDouble())
		{
			return args[0]->getDouble() > args[1]->getDouble();
		}else
		{
			return args[0]->getInteger() > args[1]->getInteger();
		}
	}else if(args[0]->isString() && args[1]->isString())
	{
		if(!args[0]->getString() && !args[1]->getString()) return false;
		if(!args[0]->getString()) return false;
		return wcscmp(args[0]->getString(),args[1]->getString()) > 0;
	}
	
	assert(false);
	return false;
}

bool vlApply::Gte(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);
	if(args[0]->isNumber() && args[1]->isNumber())
	{
		if(args[0]->isDouble() || args[1]->isDouble())
		{
			return args[0]->getDouble() >= args[1]->getDouble();
		}else
		{
			return args[0]->getInteger() >= args[1]->getInteger();
		}
	}else if(args[0]->isString() && args[1]->isString())
	{
		if(!args[0]->getString() && !args[1]->getString()) return false;
		if(!args[0]->getString()) return false;
		return wcscmp(args[0]->getString(),args[1]->getString()) >= 0;
	}
	
	assert(false);
	return false;
}

bool vlApply::Lt(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);
	if(args[0]->isNumber() && args[1]->isNumber())
	{
		if(args[0]->isDouble() || args[1]->isDouble())
		{
			return args[0]->getDouble() < args[1]->getDouble();
		}else
		{
			return args[0]->getInteger() < args[1]->getInteger();
		}
	}else if(args[0]->isString() && args[1]->isString())
	{
		if(!args[0]->getString() && !args[1]->getString()) return false;
		if(!args[0]->getString()) return true;
		return wcscmp(args[0]->getString(),args[1]->getString()) < 0;
	}
	
	assert(false);
	return false;
}

bool vlApply::Lte(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);
	if(args[0]->isNumber() && args[1]->isNumber())
	{
		if(args[0]->isDouble() || args[1]->isDouble())
		{
			return args[0]->getDouble() <= args[1]->getDouble();
		}else
		{
			return args[0]->getInteger() <= args[1]->getInteger();
		}
	}else if(args[0]->isString() && args[1]->isString())
	{
		if(!args[0]->getString() && !args[1]->getString()) return false;
		if(!args[0]->getString()) return true;
		return wcscmp(args[0]->getString(),args[1]->getString()) <= 0;
	}
	
	assert(false);
	return false;
}

bool vlApply::Eq(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);
	if(!args[0]||!args[1])
	{
		std::wstring str = this->getItem(context->stackItems.back().item).lhs->printOwner();
		return false; 
	}
	if(args[0]->getIdent() || args[1]->getIdent())
	{
		if(args[0]->getIdent()==args[1]->getIdent()) return true;
		if(args[0]->getIdent() && args[0]->getIdent()==args[1]->getStringID()) return true;
		if(args[1]->getIdent() && args[1]->getIdent()==args[0]->getStringID()) return true;
	}
	else if(args[0]->isNumber() && args[1]->isNumber())
	{
		if(args[0]->isDouble() || args[1]->isDouble())
		{
			return args[0]->getDouble() == args[1]->getDouble();
		}else
		{
			return args[0]->getInteger() == args[1]->getInteger();
		}
	}else if(args[0]->isString() && args[1]->isString())
	{
		if(!args[0]->getString() && !args[1]->getString()) return true;
		if(!args[0]->getString()) return false;
		return wcscmp(args[0]->getString(),args[1]->getString()) == 0;
	}
	else if(args[0]->isBoolean() && args[1]->isBoolean())
	{
		return args[0]->getBoolean() == args[1]->getBoolean();
	}
	else if(args[0]->isOperator() && args[1]->isOperator())
	{
		return args[0]->getOperator() == args[1]->getOperator();
	}
	else if(args[0]->isOperator() && args[1]->isChar())
	{
		wchar_t buf[8];
		vlToken & token = * args[1]->getToken(); 
		::wcsncpy_s(buf,8,token.snum + 1,token.slen);
		if(wcscmp(opValue::getOperateText(args[0]->getOperator()),buf)==0) return true;
		return false;
	}
	else if(args[1]->isOperator() && args[0]->isChar())
	{
		wchar_t buf[8];
		vlToken & token = * args[0]->getToken(); 
		::wcsncpy_s(buf,8,token.snum + 1,token.slen);
		if(wcscmp(opValue::getOperateText(args[1]->getOperator()),buf)==0) return true;
		return false;
	}
	else 
	{
		assert(false);
		//类型不匹配
		return false;
	}
	//assert(false);
	return false;
}

bool vlApply::Neq(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);
	if(args[0]->isNumber() && args[1]->isNumber())
	{
		if(args[0]->isDouble() || args[1]->isDouble())
		{
			return args[0]->getDouble() != args[1]->getDouble();
		}else
		{
			return args[0]->getInteger() != args[1]->getInteger();
		}
	}else if(args[0]->isString() && args[1]->isString())
	{
		if(!args[0]->getString() && !args[1]->getString()) return true;
		if(!args[0]->getString()) return false;
		return wcscmp(args[0]->getString(),args[1]->getString()) != 0;
	}
	else if(args[0]->isBoolean() && args[1]->isBoolean())
	{
		return args[0]->getBoolean() != args[1]->getBoolean();
	}
	else if(args[0]->isOperator() && args[1]->isOperator())
	{
		return args[0]->getOperator() != args[1]->getOperator();
	}
	else if(args[0]->isOperator() && args[1]->isChar())
	{
		wchar_t buf[8];
		vlToken & token = * args[1]->getToken(); 
		::wcsncpy_s(buf,8,token.snum + 1,token.slen);
		if(wcscmp(opValue::getOperateText(args[0]->getOperator()),buf)==0) return false;
		return true;
	}
	else if(args[1]->isOperator() && args[0]->isChar())
	{
		wchar_t buf[8];
		vlToken & token = * args[0]->getToken(); 
		::wcsncpy_s(buf,8,token.snum + 1,token.slen);
		if(wcscmp(opValue::getOperateText(args[1]->getOperator()),buf)==0) return false;
		return true;
	}	
	assert(false);
	return true;
}

bool vlApply::Nil(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);
	if(args[0]->isString() && !args[0]->isString()) return true;
	if(args[0]->kind==akNilNode) return true;
	return false;
}

bool vlApply::Not(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);
	if(!args[0]) return true;
	if(args[0]->kind==akNilNode)	return true;
	if(args[0]->isString() &&  (!args[0]->getString() || wcscmp(args[0]->getString(),L"")==0))	return true;
	if(args[0]->isInteger() && !args[0]->getInteger()) return true;
	if(args[0]->isDouble()  && !args[0]->getDouble())  return true;
	if(args[0]->isBoolean() && !args[0]->getBoolean()) return true;
	
	return false;
}

bool vlApply::And(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);
	
	if(!args[0]->isBoolean())
	{
		if(args[0]->kind==akNilNode) return false;
		else if(args[0]->isString() && !args[0]->isString()) return false;
		else if(args[0]->isInteger() &&!args[0]->isInteger()) return false;
		else if(args[0]->isDouble() && !args[0]->isDouble()) return false;
		else if(args[0]->getToken() && args[0]->getToken()->inum==0) return false;
	}else 
		if(!args[0]->getBoolean()) return false;

	if(!args[1]->isBoolean())
	{
		if(args[1]->kind==akNilNode) return false;
		else if(args[1]->isString() && !args[1]->isString()) return false;
		else if(args[1]->isInteger() &&!args[1]->isInteger()) return false;
		else if(args[1]->isDouble() && !args[1]->isDouble()) return false;
		else if(args[1]->getToken() && args[1]->getToken()->inum==0) return false;
	}else 
		if(!args[1]->getBoolean()) return false;

	return true;
}

bool vlApply::Or(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);
	
	bool first = true;
	bool second = true;
	if(!args[0]->isBoolean())
	{
		if(args[0]->kind==akNilNode) first = false;
		else if(args[0]->isString() && !args[0]->isString()) first =  false;
		else if(args[0]->isInteger() &&!args[0]->isInteger()) first =  false;
		else if(args[0]->isDouble() && !args[0]->isDouble()) first =  false;
		else if(args[0]->getToken() && args[0]->getToken()->inum==0) first =  false;
	}else 
		if(!args[0]->getBoolean()) first =  false;

	if(!args[1]->isBoolean())
	{
		if(args[1]->kind==akNilNode) second = false;
		else if(args[1]->isString() && !args[1]->isString()) second = false;
		else if(args[1]->isInteger() &&!args[1]->isInteger()) second = false;
		else if(args[1]->isDouble() && !args[1]->isDouble()) second = false;
		else if(args[1]->getToken() && args[1]->getToken()->inum==0) second = false;
	}else 
		if(!args[1]->getBoolean()) second = false;

	if(!first && !second) return false;  
	return true;
}

vlAstAxis * vlApply::SqlSelect(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	if(args.size() < 2) return 0;
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);
	dbcselect((const wstring_t)args[0]->getString(),(const wstring_t)args[1]->getString());
	return 0;
}

vlAstAxis * vlApply::toString(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	wchar_t buf[80];
	std::wstring str ;
	if(args[0]->getIdent())
		str = util::getHashString(args[0]->getIdent());
	else if(args[0]->isString())
		str = args[0]->getString();
	else if(args[0]->isDouble())
	{
		swprintf_s(buf,80,L"%f",args[0]->getDouble());
		str += buf;
	}
	else if(args[0]->isInteger())
	{
		swprintf_s(buf,80,L"%d",args[0]->getInteger());
		str += buf;
	}
	else if(args[0]->isString())
	{
		str += args[0]->getString();
	}
	else if(args[0]->isBoolean()) 
	{
		if(args[0]->getBoolean())
			str += (std::wstring)L"true";
		else
			str += (std::wstring)L"false";
	}
	return this->makeStringToken(context->getAstFactory(),_wcsdup(str.c_str()));
}

vlAstAxis * vlApply::Concat(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()>=1);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	std::wstring str;
	for(int i=0;i<(int)args.size();i++)
	{
		wchar_t buf[80];
		if(args[i]->getIdent())
		{
			str += (std::wstring)util::getHashString(args[i]->getIdent());
		}
		else if(args[i]->isString())
		{
			str += (std::wstring)args[i]->getString();
		}
		else if(args[i]->isDouble())
		{
			swprintf_s(buf,80,L"%f",args[i]->getDouble());
			str += (std::wstring)buf;
		}
		else if(args[i]->isInteger())
		{
			swprintf_s(buf,80,L"%d",args[i]->getInteger());
			str += buf;
		}
		else if(args[i]->isOperator())
		{
			str += (std::wstring)opValue::getOperateText(args[i]->getOperator());
		}
		else if(args[i]->isChar())
		{
			wchar_t buf[256];
			vlToken & token = * args[1]->getToken(); 
			::wcsncpy_s(buf,256,token.snum + 1,token.slen);
			str += (std::wstring)buf;
		}
		else if(args[i]->isBoolean()) 
		{
			if(args[i]->getBoolean())
				str += (std::wstring)L"true";
			else
				str += (std::wstring)L"false";
		}
	}
	return this->makeStringToken(context->getAstFactory(),_wcsdup(str.c_str()));
}

vlAstAxis * vlApply::Add(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	int lhs = 0,rhs=1;
	if(args[1]->isString())
	{
		lhs = 1;
		rhs = 0;
	}
	if(args[lhs]->isString())
	{
		wchar_t buf[80];
		std::wstring str = args[lhs]->getString();
		if(args[lhs]->getIdent()) str = util::getHashString(args[lhs]->getIdent());
		if(args[rhs]->getIdent())
		{
			swprintf_s(buf,80,L"%s",util::getHashString(args[rhs]->getIdent()));
			if(lhs) str = buf + str; else str += buf;
		}
		else if(args[rhs]->isDouble())
		{
			swprintf_s(buf,80,L"%f",args[rhs]->getDouble());
			if(lhs) str = buf + str; else str += buf;
		}
		else if(args[rhs]->isInteger())
		{
			swprintf_s(buf,80,L"%d",args[rhs]->getInteger());
			if(lhs) str = buf + str; else str += buf;
		}
		else if(args[rhs]->isString())
		{
			if(lhs)
			{
				if(args[rhs]->getString())
					str = args[rhs]->getString() + str;
			}
			else 
				if(args[rhs]->getString()) str += args[rhs]->getString();
		}
		else if(args[rhs]->isBoolean()) 
		{
			if(args[rhs]->getBoolean())
				if(lhs) str = (std::wstring)L"true" + str; else str += (std::wstring)L"true";
			else
				if(lhs) str = (std::wstring)L"true" + str; else str += (std::wstring)L"false";
		}
		return this->makeStringToken(context->getAstFactory(),_wcsdup(str.c_str()));
	}

	if(args[0]->isDouble() || args[1]->isDouble())
		return this->makeDoubleToken(context->getAstFactory(),args[0]->getDouble() + args[1]->getDouble());
	return this->makeIntegerToken(context->getAstFactory(),args[0]->getInteger() + args[1]->getInteger());
}

vlAstAxis * vlApply::Sub(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if(args[0]->isDouble() || args[1]->isDouble())
		return this->makeDoubleToken(context->getAstFactory(),args[0]->getDouble() - args[1]->getDouble());
	return this->makeIntegerToken(context->getAstFactory(),args[0]->getInteger() - args[1]->getInteger());
}

vlAstAxis * vlApply::Mul(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if(args[0]->isDouble() || args[1]->isDouble())
		return this->makeDoubleToken(context->getAstFactory(),args[0]->getDouble() * args[1]->getDouble());
	return this->makeIntegerToken(context->getAstFactory(),args[0]->getInteger() * args[1]->getInteger());
}

vlAstAxis * vlApply::Div(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if(args[0]->isDouble() || args[1]->isDouble())
		return this->makeDoubleToken(context->getAstFactory(),args[0]->getDouble() / args[1]->getDouble());
	return this->makeIntegerToken(context->getAstFactory(),args[0]->getInteger() / args[1]->getInteger());
}

vlAstAxis * vlApply::Mod(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if((args[0]->isInteger() ||args[0]->isChar())  && (args[1]->isInteger()||
			args[1]->isChar()))
		return this->makeIntegerToken(context->getAstFactory(),args[0]->getInteger() % args[1]->getInteger());
	
	assert(false);
	return 0;
}

vlAstAxis * vlApply::Shr(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if((args[0]->isInteger() ||args[0]->isChar())  && (args[1]->isInteger()||
			args[1]->isChar()))
		return this->makeIntegerToken(context->getAstFactory(),args[0]->getInteger() >> args[1]->getInteger());
	
	assert(false);
	return 0;
}
vlAstAxis * vlApply::Shl(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if((args[0]->isInteger() ||args[0]->isChar())  && (args[1]->isInteger()||
			args[1]->isChar()))
		return this->makeIntegerToken(context->getAstFactory(),args[0]->getInteger() << args[1]->getInteger());
	
	assert(false);
	return 0;
}
vlAstAxis * vlApply::Band(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if((args[0]->isInteger() ||args[0]->isChar())  && (args[1]->isInteger()||
			args[1]->isChar()))
		return this->makeIntegerToken(context->getAstFactory(),args[0]->getInteger() & args[1]->getInteger());
	
	assert(false);
	return 0;
}
vlAstAxis * vlApply::Bor(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if((args[0]->isInteger() ||args[0]->isChar())  && (args[1]->isInteger()||
			args[1]->isChar()))
		return this->makeIntegerToken(context->getAstFactory(),args[0]->getInteger() | args[1]->getInteger());
	
	assert(false);
	return 0;
}

vlAstAxis * vlApply::Bnot(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if(args[0]->isInteger() ||args[0]->isChar())
		return this->makeIntegerToken(context->getAstFactory(),~args[0]->getInteger());
	
	assert(false);
	return 0;
}

vlAstAxis * vlApply::Xor(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==2);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if((args[0]->isInteger() ||args[0]->isChar())  && (args[1]->isInteger()||
			args[1]->isChar()))
		return this->makeIntegerToken(context->getAstFactory(),args[0]->getInteger() ^ args[1]->getInteger());
	
	assert(false);
	return 0;
}

vlAstAxis * vlApply::Inc(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if(args[0]->isInteger() ||args[0]->isChar())
		return this->makeIntegerToken(context->getAstFactory(),args[0]->getInteger() + 1);
	
	assert(false);
	return 0;
}

vlAstAxis * vlApply::Dec(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if(args[0]->isInteger() ||args[0]->isChar())
		return this->makeIntegerToken(context->getAstFactory(),args[0]->getInteger() - 1);
	
	assert(false);
	return 0;
}

vlAstAxis * vlApply::Neg(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	assert(args.size()==1);
	for(int i=0;i<(int)args.size();i++)
		if(args[i])loadItemValue(context,args[i],paramBind);

	if(args[0]->isInteger() ||args[0]->isChar())
		return this->makeIntegerToken(context->getAstFactory(),- args[0]->getInteger());
	
	assert(false);
	return 0;
}

std::wstring vlApply::dumpVariant()
{
	int size = (int)this->variants.size();
	std::wstring str;
	for(int i=0;i<size;i++)
	{
		str += (std::wstring)L"\nvar "; 
		if(variants[i].typ) str += (std::wstring)L"list";
		str += util::getHashString(variants[i].id);
		if(variants[i].typ) 
			str += (std::wstring)L" : " + util::getHashString(variants[i].typ);
		if(variants[i].fields)
		{
			str += (std::wstring)L"(";
			for(int j=0;j<(int)variants[i].fields->size();j++)
			{
				if(j!=0) str += (std::wstring)L",";
				if((*variants[i].fields)[j].typ) str += (std::wstring)L"list";
				str += util::getHashString((*variants[i].fields)[j].id);
				if((*variants[i].fields)[j].typ) 
					str += (std::wstring)L" : " + util::getHashString((*variants[i].fields)[j].typ);
			}
			str += (std::wstring)L" )";
		}
		if(variants[i].val)
			str += variants[i].val->printOwner();
	}
	return str;
}

vlAstAxis * vlApply::MakeTypeDataNode(vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	return context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstTypeData());
}
vlAstAxis * vlApply::MakeDataBufferNode(vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind)
{
	return context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstDataBuffer());
}
	
vlAstAxis * vlApply::AppendSiblingNode(vlAstAxis * lhs,vlAstAxis * rhs)
{
	return lhs->appendSibling(rhs);
}

vlAstAxis * vlApply::AppendNextNode(vlAstAxis * lhs,	vlAstAxis * rhs)
{
	return lhs->appendNext(rhs);
}
vlAstAxis * vlApply::AppendChildNode(vlAstAxis * lhs,vlAstAxis * rhs)
{
	return lhs->appendChild(rhs);
}

vlAstAxis * vlApply::getSibling(vlAstAxis * lhs)
{
	return lhs->getSibling();
}
vlAstAxis * vlApply::getNext(vlAstAxis * lhs)
{
	return lhs->getNext();
}
vlAstAxis * vlApply::getChild(vlAstAxis * lhs)
{
	return lhs->getChild();
}
vlAstNode * vlApply::getNode(vlAstAxis * lhs)
{
	return lhs->Node();
}

vlAstAxis * vlApply::getParent(vlAstAxis * lhs)
{
	return lhs->getParent();
}

vlAstAxis * vlApply::LastChild(vlAstAxis * lhs)
{
	return lhs->LastChild();
}
vlAstAxis * vlApply::ChildNext(vlAstAxis * lhs,int step)
{
	return lhs->ChildNext(step);
}
vlAstAxis * vlApply::Child(vlAstAxis * lhs,int step)
{
	return lhs->Child(step);
}
vlAstAxis * vlApply::ChildSibling(vlAstAxis * lhs,int step)
{
	return lhs->ChildSibling(step);
}

void vlApply::context_enter_scope(vlContext* context)
{
	context->getHashContext()->enter_scope();
}
void vlApply::context_leave_scope(vlContext* context)
{
	context->getHashContext()->leave_scope();
}
int vlApply::context_push_word(vlContext* context,int v,int id)
{
	return context->getHashContext()->push_word(v,id);
}
int vlApply::context_get_word(vlContext* context,int index)
{
	return context->getHashContext()->get_word(index);
}
int vlApply::context_find_token(vlContext* context,int ide)
{
	return context->getHashContext()->find_token(ide);
}
int vlApply::context_find_next_token(vlContext* context,int index, int ide)
{
	return context->getHashContext()->find_next_token(index,ide);
}


vlAstAxis * vlApply::param2Number(vlAstAxis * t,int level)
{
	/*
	int col=0;
	int item =0;
	vlAstAxis* result = 0;
	
	vector<vlAstAxis*> nodes;
	vlAstAxis* anode = t->child();
	while(anode)
	{
		nodes.push_back(anode);
		anode = anode->next();
	}
	int size = sizeex(t); //follow is next
	while(Lt(col,size))
	{
		let t = topex(t,col);
		if(IsLiteral(t))
		{
			if(child(t))
			{
				//bind(item,N(I(ident(name(t))),N(C(1),N(V(child(t))))));
				bind(item,N(I(ident(name(t))),N(C(1),V(child(t)))));
			}
			else
			{
				//bind(item, N(V(t)));
				bind(item, V(t));
			}
		}else
		{
			if(level)
				bind(item, param2Number(t,Dec(level)));
			else
				bind(item, A(x));
		}
		if(Eq(col,0))
			bind(result,item);
		else
			bind(result,N(item,result));
		bind(col,Inc(col));
	}
	return N(I(ident(t)),N(C(col),result));*/
	return 0;
}
