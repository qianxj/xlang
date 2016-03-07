#include "vlpreparse.hpp"
#include "machine_x86.hpp"
#include "vlcoder.hpp"
#include <set>
using namespace XM;
using namespace xlang;

namespace trap {
namespace complier {

/*static*/ std::vector<xlang::HNamespace> namespaces;
/*static*/ std::vector<xlang::HType> classes;
/*static*/ std::vector<xlang::HMethod> methodes;
/*static*/ vlErrorSvr errorMgr;

#define TRACE 1
#undef TRACE
int GetNodeIdent(TNode * id)
{
	return util::hashString((const wstring_t)((OpToken*)id)->tk.snum,((OpToken*)id)->tk.slen);
}

 const wchar_t* GetNodeIdentString(TNode * id)
{
	return ((OpToken*)id)->tk.snum;
}

TypeKind GetClassKind(const wchar_t* kind)
 {
	 if(wcscmp(kind,L"class")==0) return tkClass;
	 if(wcscmp(kind,L"struct")==0) return tkStruct;
	 if(wcscmp(kind,L"interface")==0) return tkInterface;
	 return tkUnknown;
 }

TNode* addOleNamespace(TContext &context, TNode* id, TNode* olestr)
{
	xlang::Handle hOleTerm = context.trap->addNSOleTerm(GetNodeIdent(olestr),0,0);
	HNamespace hspace = namespaces.size()<1 ? 
		context.trap->addNamespace(context.trap->getGlobalNamespace(),GetNodeIdent(id))
		: context.trap->addNamespace(namespaces.back(),GetNodeIdent(id));
	context.trap->setNSOleTerm(hspace, hOleTerm);
	context.getHashContext()->push_word(hspace,GetNodeIdent(id));
	return 0;
}

 TNode* enterNamespace(TContext &context,TNode* id)
{
	#ifdef TRACE
		printf("\n namespace %S",GetNodeIdentString(id));
	#endif
	HNamespace hspace = namespaces.size()<1 ? 
		context.trap->addNamespace(context.trap->getGlobalNamespace(),GetNodeIdent(id))
		: context.trap->addNamespace(namespaces.back(),GetNodeIdent(id));
	namespaces.push_back(hspace);
	context.getHashContext()->push_word(hspace,GetNodeIdent(id));
	context.getHashContext()->enter_scope();
	return 0;
}

 TNode* leaveNamespace(TContext &context)
{
	context.getHashContext()->leave_scope();
	namespaces.pop_back();
	return 0;
}

 TNode* forwardClass(TContext &context,TNode* class_key,TNode* id,TNode* base)
 {
	const wchar_t * key = GetNodeIdentString(class_key);
	const wchar_t * cid = GetNodeIdentString(id);
	#ifdef TRACE
		printf("\n class %S",GetNodeIdentString(id));
	#endif
	HNamespace hspace = namespaces.size()<1 ? context.trap->getGlobalNamespace():
		namespaces.back();
	HType hType = context.trap->addType(hspace,GetClassKind(key),GetNodeIdent(id),false);
	context.getHashContext()->push_word(hType,GetNodeIdent(id));
	return 0;
 }

 TNode* makeClass(TContext &context,TNode* class_key,TNode* id,TNode* base)
{
	const wchar_t * key = GetNodeIdentString(class_key);
	const wchar_t * cid = GetNodeIdentString(id);
	#ifdef TRACE
		printf("\n class %S",GetNodeIdentString(id));
	#endif
	HNamespace hspace = namespaces.size()<1 ? context.trap->getGlobalNamespace():
		namespaces.back();
	HType hType = context.trap->addType(hspace,GetClassKind(key),GetNodeIdent(id));

	if(base)
	{
		if(base->kind==kList)
		{
			TNode* term = ((OpList*)base)->node;
			int errIndex = (int)errorMgr.getErrors().size();
			term = checkTypeId(context,(OpItem*)term,&errorMgr);
			errorMgr.printError(errIndex);

			context.trap->addTypeBase(hType,((OpList*)base)->node->hType);
		}
	}
	classes.push_back(hType);
	context.getHashContext()->push_word(hType,GetNodeIdent(id));
	context.getHashContext()->enter_scope();
	return 0;
}

 TNode* finishedClass(TContext &context)
 {
	classes.back();
	SymType * cls = context.trap->getType(classes.back());
	HMethod hMethod = cls->hMethod;
	int count = cls->methodCount;
	int errors = errorMgr.getErorCount();
	for(int i=0;i<count;i++)
	{
		TNode * node =(TNode*)context.trap->getMethod(hMethod)->vcode;
		if(node)
		{
			methodes.push_back(hMethod);
			HParam hParam = context.trap->getMethod(hMethod)->hParam;
			int paramCount = context.trap->getMethod(hMethod)->paramCount;
			context.getHashContext()->enter_scope();
			for(int i=0;i<paramCount;i++)
			{
				context.getHashContext()->push_word(hParam,
					context.trap->getParam(hParam)->name);
				hParam.index++;
			}
			/*
			int variantCount = context.trap->getMethod(hMethod)->variantCount;
			HVariant hVariant = context.trap->getMethod(hMethod)->hVariant;
			for(int i=0;i<variantCount;i++)
			{
				context.getHashContext()->push_word(hVariant,
					context.trap->getVariant(hVariant)->name);
				hVariant.index++;
			}
			*/

			checkNode(context,node);
			context.getHashContext()->leave_scope();
			methodes.pop_back();
		}
		hMethod.index++;
	}

	ProcessApply(context,classes.back());

	if( errors == errorMgr.getErorCount())
	{
		HMethod hMethod = cls->hMethod;
		for(int i=0;i<count;i++)
		{
			TNode * node =(TNode*)context.trap->getMethod(hMethod)->vcode;
			if(node)
			{
				methodes.push_back(hMethod);
				PCode::Block* block = (PCode::Block*)genStmtEx(context,node);
				genCode(context,block);
				methodes.pop_back();
			}
			hMethod.index++;
		}
	}
	context.getHashContext()->leave_scope();
	classes.pop_back();
	 return 0;
 }

 TNode* finishedMethod(TContext &context)
 {
 	context.getHashContext()->leave_scope();
	methodes.pop_back();
	return 0;
}

 TNode* makeApply(TContext &context,TNode* id,TNode* typ,TNode* paramList)
 {
	 int ident = GetNodeIdent(id);
#ifdef TRACE
	printf("\n apply %S",GetNodeIdentString(id));
#endif
	int haxis = classes.size()>0? classes.back() : namespaces.size()>0? 
		 namespaces.back(): context.trap->getGlobalNamespace();
	
	OpItem * item = (OpItem*)typ;
	if(item)checkTypeId(context,item,&errorMgr);
	
	HMethod hMethod = context.trap->addApply(0,haxis,ident,0,0);
	context.getHashContext()->push_word(hMethod,GetNodeIdent(id));
	context.getHashContext()->enter_scope();
	if(item)context.trap->getMethod(hMethod)->hType = item->hType;

	int errIndex = (int)errorMgr.getErrors().size();
	
	int nameid = 0;
	if(paramList)
	{
		OpList * stmts = 0;
		OpList * params = (OpList *)paramList;
		std::set<int> vars;
		while(params)
		{
			if(params->node)
			{
				bool any = false;
				bool ellipsis = false;
				OpParam* param = (OpParam*)params->node;
				if(param->typ)checkTypeId(context,(OpItem*)param->typ,&errorMgr);
				HType exprType  = context.trap->GetTypeUnknown();
				if(param->expr)
				{
					if(param->expr->kind==kToken)
					{
						vlToken& tk = ((OpToken*)param->expr)->tk;
						//match '-'
						if(tk.ty == tokenType::tOp && tk.inum==opValue::sub)
						{
							any = true;
						}
						//match '...'
						else if(tk.ty == tokenType::tOp && tk.inum==opValue::dot3)
						{
							ellipsis = false;
						}
					}else
					{
						checkExpression(context,param->expr,&errorMgr);
						exprType = param->expr->hType;

					}
				}
				if(!param->ident)
				{
					wchar_t buf[80];
					swprintf_s(buf,80,L"__param_%d__",nameid++);
					param->ident = util::hashString(buf);
				}
				HParam hParam = param->typ?
					context.trap->addParam(hMethod,param->ident,((OpItem*)param->typ)->spec,param->typ->hType):
					context.trap->addParam(hMethod,param->ident,0,exprType);
				
				context.trap->getParam(hParam)->value = 0;
				if(param->expr)
				{
					context.trap->setTermInitValue(hParam,0);
					if(!any && !ellipsis) context.trap->getParam(hParam)->value = param->expr;
				}
				context.trap->getParam(hParam)->any = any;
				context.trap->getParam(hParam)->ellipsis = ellipsis;
				context.trap->getParam(hParam)->terms = param->terms;

				if(param->terms)
				{
					HType hType = context.trap->getParam(hParam)->hType;
					int arg  = -1;
					OpList * terms = (OpList *)param->terms;
					while(terms)
					{
						if(terms->node)
						{
							arg++;
							OpParam* term = (OpParam*)terms->node;
							if(term->ident && vars.find(term->ident)==vars.end())
							{
								vars.insert(term->ident);
								HField hField = context.trap->getType(hType)->hField;
								hField.index += arg;
								HType hTy;
								if(term->typ)
								{
									checkTypeId(context,(OpItem*)term->typ,&errorMgr);
									hTy = term->typ->hType;
								}else
								{
									hTy = context.trap->getField(hField)->hType;
								}

								OpItem * item = opItem(hParam);
								item->hType = hType;
								OpField* expr = opField(0,item,0);
								expr->hType = hTy;
								expr->hField = hField;

								OpVar* var = opVar(term->ident,opItem(hTy),expr);
								var->typ->hType = hTy;
								if(stmts)
									opList(stmts,var);
								else
									stmts = opList(var);

								//HVariant hVariant = context.trap->addVariant(hMethod,term->ident,0,hTy);
								//context.getHashContext()->push_word(hVariant,term->ident);
							}
						}
						terms = terms->next;
					}
				}

				context.getHashContext()->push_word(hParam,param->ident);
			}
			params = params->next;
		}
		if(stmts) context.trap->getMethod(hMethod)->precode = stmts;
	}
	
	methodes.push_back(hMethod);
	errorMgr.printError(errIndex);

	 return 0;
 }

 

 TNode* makeFunction(TContext &context,TNode* id,TNode* Type,TNode* paramList)
 {
	if(!Type) return 0;
	int ident = GetNodeIdent(id);
#ifdef TRACE
	printf("\n function %S",GetNodeIdentString(id));
#endif
	int haxis = classes.size()>0? classes.back() : namespaces.size()>0? 
		 namespaces.back(): context.trap->getGlobalNamespace();
	OpItem * item = (OpItem*)Type;
	checkTypeId(context,item,&errorMgr);
	HMethod hMethod = context.trap->addMethod(haxis,ident,item->spec,item->hType);
	context.getHashContext()->push_word(hMethod,GetNodeIdent(id));
	context.getHashContext()->enter_scope();

	int errIndex = (int)errorMgr.getErrors().size();
	if(paramList)
	{
		OpList * params = (OpList *)paramList;
		while(params)
		{
			if(params->node)
			{
				OpParam* param = (OpParam*)params->node;
				checkTypeId(context,(OpItem*)param->typ,&errorMgr);
				if(param->expr)checkExpression(context,param->expr,&errorMgr);
				if(!param->ident) param->ident = util::hashString(L"__paramid__");
				HParam hParam = context.trap->addParam(hMethod,param->ident,((OpItem*)param->typ)->spec,param->typ->hType);
				if(param->expr)context.trap->setTermInitValue(hParam,0);
				context.getHashContext()->push_word(hParam,param->ident);
			}
			params = params->next;
		}
	}
	methodes.push_back(hMethod);
	errorMgr.printError(errIndex);

	 return 0;
 }

 TNode* SetNativeFrom(TContext &context, TNode* val)
 {
	 if(methodes.size())
	 {
		 context.trap->setMethodNativeFrom(methodes.back(),GetNodeIdent(val));
	 }else
		 context.trap->setTypeNativeFrom(classes.back(),GetNodeIdent(val));
	 return val;
 }
 TNode* SetNativeAlias(TContext &context, TNode* val)
 {
	 if(methodes.size())
	 {
		 context.trap->setMethodNativeAlias(methodes.back(),GetNodeIdent(val));
	 }else
		 context.trap->setTypeNativeAlias(classes.back(),GetNodeIdent(val));
	 return val;
 }

 TNode* makeTypeField(TContext &context,TNode* Type,TNode* id,TNode * init)
 {
	 if(!Type) return 0;

	 int ident = GetNodeIdent(id);
	 HField hfield = context.trap->addField(classes.back(),ident,0,
		 checkTypeId(context,(OpItem*)Type,&errorMgr)->hType);
	 context.getHashContext()->push_word(hfield,ident);

	 return 0;
 }

 TNode* makeClassBase(TContext &context,TNode* class_key,
										TNode* typ)
{
	return typ;
}

 TNode* makeThis(TContext &context)
{
	return opThis();
}

TNode* makeQualifiedTerm(TContext &context,TNode * nest,TNode* ident, TNode * param)
{
	OpItem * term = opItem((OpItem*)nest,GetNodeIdent(ident),param);
	term->srcRow = ident->srcRow;
	term->srcCol= ident->srcCol;
	return term;
}

 TNode* makeQualifiedId(TContext &context,TNode* spec,TNode * terms,TNode * rank)
{
	TNodeList * specs = (TNodeList*)spec;
	unsigned int specValue = 0;
	static std::map<int /*ident*/,unsigned int /*val*/> mapSpec;
	if(!mapSpec.size())
	{
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"extern") 		, 0x1));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"static")		, 0x2));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"mutable")		, 0x4));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"readonly")		, 0x8));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"thread_local")	, 0x10));	
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"volatile")		, 0x20));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"native")		, 0x10));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"safe")			, 0x20));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"manage")		, 0x20));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"const")			, 0x40));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"in")			, 0x80));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"out")			, 0x100));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"byref")			, 0x200));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"ref")			, 0x200));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"byval")			, 0x400));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"inline")		, 0x100));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"explicit")		, 0x400));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"overridev")		, 0x800));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"stdcall")		, 0x1000));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"friend")		, 0x2000));
		mapSpec.insert(std::pair<int,int>(util::hashString((wstring_t)L"virtual")  		, 0x4000));
	}
	while(specs && specs->node)
	{
		int ident = GetNodeIdent(specs->node);
		std::map<int,unsigned int>::const_iterator iter = mapSpec.find(ident);
		if(iter!= mapSpec.end())
			specValue |= mapSpec[ident];
		specs = specs->next;
	}
	OpItem* item = (OpItem*)terms;
	item->spec = specValue;
	item->rank = rank;
	return terms;
}

 TNode* makeSuper(TContext &context)
{
	return opSuper();
}

 TNode* makeItem(TContext &context,TNode* id)
{
	assert(false);
	return 0;
}

 TNode* makeLiteral(TContext &context,TNode* r)
{

	return opValue(0,r);
}

 TNode* makeField(TContext &context,TNode* exp,TNode* id)
{
	return opField(0,exp,id);
}

 TNode* makeElement(TContext &context,TNode* exp,TNode* rank)
{
	return opElement(0,exp,rank);
}

 TNode* makeInc(TContext &context,TNode* exp)
{
	return opInc(0,exp);
}
 TNode* makeDec(TContext &context,TNode* exp)
{
	return opDec(0,exp);
}

 TNode* makeUnryExpr(TContext &context,TNode* unary,TNode* exp)
{
	if(!unary) return exp;
	vlToken& tk = ((OpToken *)unary)->tk;
	TNode * result = 0;
	if(tk.ty==tokenType::tIdent)
	{
		if(tk.slen==3 && wcsncmp(tk.snum,L"new",3)==0)
		{
			if(exp->kind==kItem)
			{
				OpItem* term = (OpItem*)exp;
				if(term->rank && ((OpList*)term->rank)->node)
				{
					TNode* rank = ((OpList*)term->rank)->node;
					term->rank = 0;
					return setNodePosition(opNew(exp,rank),unary);
				}
				return setNodePosition(opNew(exp,0),unary);
			}
			else if(exp->kind==kElement)
			{
				return setNodePosition(opNew(((OpElement*)exp)->exp,((OpElement*)exp)->rank),unary);
			}else
				return setNodePosition(opNew(exp,0),unary);
		}
		if(tk.slen==6 && wcsncmp(tk.snum,L"delete",6)==0) 
			return setNodePosition(opDelete(exp),unary);
		if(tk.slen==6 && wcsncmp(tk.snum,L"sizeof",6)==0) 
			return setNodePosition(opSizeof(exp),unary);
	}
	if(tk.ty==tokenType::tOp)
	{
		if(tk.inum==opValue::inc) return setNodePosition(opUInc(0,exp),unary);
		if(tk.inum==opValue::dec) return setNodePosition(opUDec(0,exp),unary);
		if(tk.inum==opValue::sub) return setNodePosition(opNeg(0,exp),unary);
		if(tk.inum==opValue::not) return setNodePosition(opNot(0,exp),unary);
		if(tk.inum==opValue::bnot) return setNodePosition(opBNot(0,exp),unary);
	}
	return 0;
}

 TNode* makeBinaryExpr(TContext &context,TNode* op,TNode* lhs,TNode* rhs  )
{
	vlToken & tk = ((OpToken*)op)->tk;
	return setNodePosition(opBinary(0,(OpBinary::Op)tk.inum,lhs,rhs),op);
}

 TNode* makeTypeID(TContext &context,TNode* pid,TNodeList* id,TNodeList* params )
{
	if(pid)
		return setNodePosition(opType(context.trap->getTypeHandle(GetNodeIdent(pid))),pid);

	if(id)
	{
		while(id->next && !id->node)id=id->next;
		if(id->node)
		{
			SNode* node = id->node;
			Handle ht;
			int t = context.getHashContext()->find_token(GetNodeIdent(id->node));
			if(t==-1)
			{
				printf("\n未定义类型: '%S'",GetNodeIdentString(id->node));
				return 0;
			}else
				ht = context.getHashContext()->get_word(t);
			id = id->next;
			while(id && id->node)
			{
				ht = context.trap->lookupItem(ht,GetNodeIdent(id->node));
				if(!ht.kind)
				{
					printf("\n未定义类型: '%S'",GetNodeIdentString(id->node));
					return 0;
				}
				if(id->next) id = id->next;
			}
			return setNodePosition(opType(ht),id);
		}else return 0;
	}
	return 0;
}

TNode* makeContinue(TContext &context)
{
	return opContinue();
}

TNode* makeBreak(TContext &context)
{
	return opBreak();
}

TNode* makeFor(TContext &context,TNode* init,TNode* cond,TNode* inc,TNode* body)
{
	return opFor(init,cond,inc,body);
}

TNode* makeWhile(TContext &context,TNode* cond,TNode* body)
{
	return opWhile(cond,body);
}

TNode* makeUntil(TContext &context,TNode* cond,TNode* body)
{
	return opUntil(cond,body);
}

TNode* makeCompStmt(TContext &context,TNode* body)
{
	return opCompStmt(body);
}

TNode* makeIf(TContext &context,TNode* cond,TNode* tbody,TNode* fbody)
{
	return opIf(cond,tbody,fbody);
}

TNode* makeTypeID(TContext &context,TNode* axis,TNode* spec,TNode* id,TNode* param,TNode* rank)
{
	int ident = ((OpToken*)id)->tk.inum;
	return opType((OpList*)spec,ident,(OpList*)param,(OpList*)rank,(OpType*)axis);
}

TNode* makeReturn(TContext &context,TNode* expr)
{
	return setNodePosition(opReturn(expr),expr);
}

TNode* makeCall(TContext &context,TNode* fn,TNode* args)
{
	return setNodePosition(opCall(0,fn,args),fn);
}

TNode* makeVar(TContext &context,TNode* id,TNode* typ,TNode* init)
{
	int ident = id?((OpToken*)id)->tk.inum:0;
	return setNodePosition(opVar(ident,typ,init),id);
}

TNode* makeParam(TContext &context,TNode* id,TNode* typ,TNode* init,TNode* terms)
{
	int ident = id?((OpToken*)id)->tk.inum:0;
	return setNodePosition(opParam(ident,typ,init,terms),id);
}

TNode* makeParam(TContext &context,TNode* id,TNode* typ,TNode* init)
{
	int ident = id?((OpToken*)id)->tk.inum:0;
	return setNodePosition(opParam(ident,typ,init),id);
}

TNode* makeCast(TContext &context,TNode* expr,TNode* toType)
{
	return opCast(toType,expr);
}

TNode* makeExpression(TContext &context, TNode* expr)
{
	if(!expr) return expr;
	if(expr->kind==kBinary)((OpBinary*)expr)->expand = false;
	return expr;
}

TNode* makeExprStmt(TContext &context,TNode* expr)
{
	return opExprStmt(expr);
}

std::wstring TraceNode(TContext &context,TNode* stmt,std::wstring  space)
{
	if(!stmt) return L"";
	std::wstring str;
	//if(stmt->isArg)	str +=L"push ";
	switch(stmt->kind)
	{
	case kIf:
		{
			OpIf * term = (OpIf*)stmt;
			str += L"if(" + TraceNode(context,term->cond) + L")";
			str += (std::wstring)(term->tbody->kind==kCompStmt? L"" : space+L"\t") 
						+  TraceNode(context,term->tbody,space);
			if(term->fbody)
			{
				str += space + L"else";
				str += (std::wstring)(term->fbody->kind==kCompStmt? L"" : space+L"\t") 
						+  TraceNode(context,term->fbody,space);
			}
		}
		break;
	case kFor:
		{
			OpFor * term = (OpFor*)stmt;
			str += L"for(" + (term->init?TraceNode(context,term->init):L"")+ 
				L" "  + (term->cond?TraceNode(context,term->cond):L"") +
				L"; " + (term->inc?TraceNode(context,term->inc):L"")  + L")";
			str += (term->body->kind==kCompStmt? L"" : space+L"\t") 
						+  TraceNode(context,term->body,space);
		}
		break;
	case kWhile:
		{
			OpWhile * term = (OpWhile*)stmt;
			str += L"while(" + TraceNode(context,term->cond) + L")";
			str += (term->body->kind==kCompStmt? L"" : space+L"\t") 
						+  TraceNode(context,term->body,space);
		}
		break;
	case kUntil:
		{
			OpUntil * term = (OpUntil*)stmt;
			str += L"do " + TraceNode(context,term->body,space);
			str += L"while(" + TraceNode(context,term->cond) + L")";
		}
		break;
	case kReturn:
		{
			OpReturn * term = (OpReturn*)stmt;
			if(term->expr)
			{
				str +=  L"return " + TraceNode(context,term->expr) + L";";
			}
			else
				str += L"return;";
		}
		break;
	case kContinue:
		str += L"continue;";
		break;
	case kBreak:
		str += L"break;";
		break;
	case kCompStmt:
		{
			str += space + L"{";
			OpCompStmt * term = (OpCompStmt*)stmt;
			if(term->stmts->kind==kList)
			{
				OpList * terms = (OpList *)term->stmts;
				while(true)
				{
					if(terms->node) 
						str+= space +L"\t" + TraceNode(context,terms->node,space+L"\t");
					if(!terms->next)break;
					terms = terms->next;
				}
			}else
				str += space +L"\t" + TraceNode(context,term->stmts,space+L"\t");
			
			str += space + L"}";
		}
		break;
	case kExprStmt:
		{
			OpExprStmt* term = (OpExprStmt*)stmt;
			str += TraceNode(context,term->expr)+L";";
			break;
		}
		break;
	case kItem:
		{
			OpItem * term = (OpItem *)stmt;
			if(term->hTerm)
			{
				if(term->hTerm.kind==skVariant)
					str += util::getHashString(context.trap->getVariant(term->hTerm)->name);
				else if(term->hTerm.kind==skParam)
					str += util::getHashString(context.trap->getParam(term->hTerm)->name);
				else if(term->hTerm.kind==skField)
					str += util::getHashString(context.trap->getField(term->hTerm)->name);
				else if(term->hTerm.kind==skMethod)
					str += util::getHashString(context.trap->getMethod(term->hTerm)->name);
				else
					str += L"#v";
			}
			else if(term->tempId)
			{
				wchar_t buf[3];
				if(term->tempId>9)
				{
					buf[2] = 0;
					buf[1] = '0' + (term->tempId - 1) % 10;
					buf[0] = '0' + (term->tempId  - 1)/10;
				}else
				{
					buf[1] = 0;
					buf[0] = '0' + (term->tempId -1)% 10;
				}
				str += (std::wstring)L"t" + buf;
			}
			else if(term->nest)
			{
				str += TraceNode(context,term->nest);
				str+=(std::wstring)L"::" +  util::getHashString(term->ident);
			}else
				str +=  util::getHashString(term->ident);
			
			if(term->rank)
			{
				TNodeList * rank = (TNodeList *)term->rank;
				while(rank)
				{
					if(rank->node &&rank->node->kind==kEmpty)
					{
						str += L"[]";
					}else
						str += (std::wstring)L"[" + TraceNode(context,rank->node,space)+L"]";
					rank = rank->next;
				}
			}
		}
		break;
	case kThis:
		str +=  L"this";
		break;
	case kSuper:
		str +=  L"super";
		break;
	case kValue:
		{
			OpValue * term = (OpValue*)stmt;
			wchar_t buf[80];
			switch(term->vt)
			{
			case OpValue::I32:
			case OpValue::UI32:
				_ltow_s(term->val_i32,buf,80,10);
				str += buf;
				break;
			case OpValue::D32:
			case OpValue::D64:
				{
					swprintf_s(buf,80,L"%f",term->val_d64);
					str += buf;
				}
				break;
			case OpValue::BOOL:
				str += term->val_bool?L"true":L"false";
				break;
			case OpValue::STR:
				str += (std::wstring)L"\"" + term->val_wstr + L"\"";
				break;
			default:
				assert(false);
				break;
			}
		}
		break;
	case kNill:
		str += L" nill ";
		break;
	case kField:
		{
			OpField * term = (OpField *)stmt;
			vlToken & tk = ((OpToken *)term->hitem)->tk;
			str += TraceNode(context,term->base) + L"." + util::getHashString(tk.inum);
		}
		break;
	case kElement:
		{
			OpElement * term = (OpElement*)stmt;
			str += TraceNode(context,term->exp)+ L"["+( term->rank?TraceNode(context,term->rank):L"") + L"]";
		}
		break;
	case kMethod:
		assert(false);
		break;
	case kCall:
		{
			OpCall * term = (OpCall *) stmt;
			str += TraceNode(context,term->mt)+L"(";
			if(term->args)
			{
				if(term->args->kind==kList)
				{
					OpList * terms = (OpList*)term->args;
					while(true)
					{
						if(terms->node)
						{
							str += TraceNode(context,terms->node);
							if(terms->next) str += L",";
						}
						if(!terms->next)break;
						terms = terms->next;
					}
				}else
				{
					str += TraceNode(context,term->args);
				}
			}
			str += L")";
		}
		break;
	case kArgList:
		assert(false);
		break;
	case kType:
		{
			OpType * term = (OpType*)stmt;
			if(term->spec)
			{
				if(term->spec->kind==kList)
				{
					OpList * terms = (OpList*)term->spec;
					while(true)
					{
						if(terms->node) str+= TraceNode(context,terms->node)+L" ";
						if(!terms->next) break;
						terms = terms->next;
					}
				}else
				{
					str += TraceNode(context,term->spec)+L" ";
				}
			}
			if(term->axis) str += TraceNode(context,term->axis)+L"::";
			str += util::getHashString(term->ident);
			if(term->param)
			{
				if(term->param->kind==kList)
				{
					OpList * terms = (OpList*)term->param;
					while(true)
					{
						if(terms->node) str+= TraceNode(context,terms->node);
						if(terms->next) str += L", ";
						if(!terms->next) break;
						terms = terms->next;
					}
				}else
				{
					str +=(std::wstring)L"<" + TraceNode(context,term->spec) +L">";
				}
			}
			if(term->rank)
			{
				if(term->rank->kind==kList)
				{
					OpList * terms = (OpList*)term->rank;
					while(true)
					{
						if(terms->node) str+= (std::wstring)L"[" + TraceNode(context,terms->node)+L"]";
						if(!terms->next) break;
						terms = terms->next;
					}
				}else
				{
					str +=(std::wstring)L"[" + TraceNode(context,term->rank) +L"]";
				}
			}
		}
		break;
	case kInc:
		{
			OpInc * term = (OpInc *)stmt;
			str +=  TraceNode(context,term->expr)+L"++";
		}
		break;
	case kDec:
		{
			OpDec * term = (OpDec *)stmt;
			str +=  TraceNode(context,term->expr)+L"--";
		}
		break;
	case kUInc:
		{
			OpUInc * term = (OpUInc *)stmt;
			str += (std::wstring) + L"++" + TraceNode(context,term->expr);
		}
		break;
	case kUDec:
		{
			OpUDec * term = (OpUDec *)stmt;
			str += (std::wstring)L"--" + TraceNode(context,term->expr);
		}
		break;
	case kNew:
		{
			OpNew * term = (OpNew *)stmt;
			str += (std::wstring)L"new " + TraceNode(context,term->typ);
			if(term->rank) str += (std::wstring)L"["+TraceNode(context,term->rank)+L"]";
		}
		break;
	case kDelete:
		{
			OpDelete * term = (OpDelete *)stmt;
			str += (std::wstring)L"delete " + TraceNode(context,term->exp);
		}
		break;
	case kNot:
		{
			OpNot * term = (OpNot *)stmt;
			str += (std::wstring)L"!(" + TraceNode(context,term->expr)+L")";
		}
		break;
	case kBNot:
		{
			OpBNot * term = (OpBNot *)stmt;
			str += (std::wstring)L"~" +TraceNode(context,term->expr);
		}
		break;
	case kNeg:
		{
			OpNeg * term = (OpNeg *)stmt;
			str += (std::wstring)L" - " +TraceNode(context,term->expr)+L"";
		}
		break;
	case kCast:
		{
			OpCast * term = (OpCast *)stmt;
			str += (std::wstring)L"case(" + TraceNode(context,term->expr)+L" as "+ 
				TraceNode(context,term->typ) +L")";
		}
		break;
	case kBinary:
		{
			OpBinary * term = (OpBinary*)stmt;
			str += TraceNode(context,term->lhs);
			str += (std::wstring)L" "+ opValue::getOperateText(term->op)+L" ";
			str += TraceNode(context,term->rhs);
			if(term->ops.size())
			{
				for(std::size_t i=0;i<term->ops.size();i++)
				{
					str += (std::wstring)L" "+ opValue::getOperateText(term->ops[i])+L" ";
					if(term->terms[i]->kind==kBinary)
						str += (std::wstring)L"(" + TraceNode(context,term->terms[i]) +L")";
					else
						str += TraceNode(context,term->terms[i]);
				}
			}
		}
		break;
	case kList:
		{
			OpList * terms = (OpList *)stmt;
			bool start = true;
			while(true)
			{
				if(terms->node)
				{
					if(start)
					{
						str += TraceNode(context,terms->node,space);
						start = false;
					}
					else
						str += space + TraceNode(context,terms->node,space);
				}
				if(!terms->next)break;
				terms = terms->next;
			}
		}
		break;
	case kParen:
		assert(false);
		break;
	case kVar:
		{
			OpVar * term = (OpVar *) stmt;
			str += TraceNode(context,term->typ);
			str += (std::wstring)L" " + util::getHashString(term->ident);
			if(term->expr) str += (std::wstring)L" = " + TraceNode(context,term->expr);
			str += L";";
		}
		break;
	case kParam:
		assert(false);
		break;
	case kToken:
		{
			OpToken * term = (OpToken*)stmt;
			if(term->tk.ty==tokenType::tIdent)
				str+=util::getHashString(term->tk.inum);
			else 
				assert(false);
		}
		break;
	}
	return str;
}

TNode*  dumpNode(TContext &context,TNode* node)
{
	printf("%S",TraceNode(context,node,L"\n").c_str());
	return node;
}

void pushError(TNode* node,int errCode,vlErrorSvr * errMgr)
{
	errMgr->adderror(0,node->srcRow,node->srcCol,errCode);
}
void pushError(TNode* node,int errCode,const wchar_t * errText,vlErrorSvr * errMgr)
{
	errMgr->adderror(0,node->srcRow,node->srcCol,errCode,errText);
}
void pushError(TNode* node,int errCode,int ident,vlErrorSvr * errMgr)
{
	errMgr->adderror(0,node->srcRow,node->srcCol,errCode,util::getHashString(ident));
}
void pushWarn(TNode* node,int errCode,vlErrorSvr * errMgr)
{
	errMgr->addwarn(0,node->srcRow,node->srcCol,errCode);
}
void pushWarn(TNode* node,int errCode,const wchar_t * errText,vlErrorSvr * errMgr)
{
	errMgr->addwarn(0,node->srcRow,node->srcCol,errCode,errText);
}
void pushWarn(TNode* node,int errCode,int ident,vlErrorSvr * errMgr)
{
	errMgr->addwarn(0,node->srcRow,node->srcCol,errCode,util::getHashString(ident));
}


TNode* checkBinary(TContext & context,OpBinary* expr,vlErrorSvr * errMgr)
{
	OpBinary::Op op = expr->op; 
	if(op==OpBinary::asn) expr->lhs->isLeftVaule = true;
	TNode* lhs = checkExpression(context,expr->lhs,errMgr);
	TNode* rhs = checkExpression(context,expr->rhs,errMgr);
	expr->lhs = lhs;
	expr->rhs = rhs;
	/*for(int i=0;i<(int)expr->terms.size();i++)
	{
		checkExpression(context,expr->terms[i],errMgr);
	}*/

	switch(op & 0x7f)
	{
	case OpBinary::add:	
	case OpBinary::sub:
	case OpBinary::mul:
	case OpBinary::div:
	case OpBinary::mod:
		if((int)lhs->hType!=(int)context.trap->GetTypeUnknown() && (int)rhs->hType!=
			(int)context.trap->GetTypeUnknown())
		{
			if((int)lhs->hType == (int)context.trap->GetTypeString()||
				(int)rhs->hType == (int)context.trap->GetTypeString())
			{
				if((int)lhs->hType!=(int)rhs->hType)
				{
					pushError(lhs,error::err_text,L"字符串相加需要字符串类型.",errMgr);
				}
				expr->hType = lhs->hType;
				return expr;
			}
			else if(!context.trap->isNumberType(lhs->hType) && 
						!context.trap->isNumberType(rhs->hType))
			{
				pushError(lhs,error::err_neednumbertype,errMgr);
			}
		}
		expr->hType = lhs->hType;
		return expr;
	case OpBinary::shl:
	case OpBinary::shr:
	case OpBinary::band:
	case OpBinary::bor:
	case OpBinary::xor:
		//判断操作数是否是整数类型
		if((int)lhs->hType!=(int)context.trap->GetTypeUnknown() && (int)rhs->hType!=
			(int)context.trap->GetTypeUnknown())
		{
			if( !context.trap->isIntNumberType(lhs->hType) && 
						!context.trap->isIntNumberType(rhs->hType))
			{
				pushError(lhs,error::err_neednumbertype,errMgr);
			}
			expr->hType = lhs->hType;
		}
		return expr;
	case OpBinary::or:
	case OpBinary::and:
		expr->hType = context.trap->GetTypeBool();
		return expr;
	case OpBinary::eq:
	case OpBinary::neq:
	case OpBinary::gt:
	case OpBinary::gte:
	case OpBinary::lt:
	case OpBinary::lte:
		if((int)lhs->hType!=(int)context.trap->GetTypeUnknown() && (int)rhs->hType!=
			(int)context.trap->GetTypeUnknown())
		{
			if(lhs->hType != rhs->hType)
			{
				//pushError(lhs,error::err_text,L"关系操作需要相同的类型.",errMgr);
				pushError(lhs->srcRow?lhs:rhs,error::err_text,::wcsdup(((std::wstring)L"关系操作需要相同的类型1. "+
					context.trap->getTermName(lhs->hType)+opValue::getOperateText(op)+
					context.trap->getTermName(rhs->hType)).c_str()),errMgr);
			}
		}
		expr->hType = context.trap->GetTypeBool();
		return expr;
	case OpBinary::asn:
		if((int)lhs->hType!=(int)context.trap->GetTypeUnknown() && (int)rhs->hType!=
			(int)context.trap->GetTypeUnknown())
		{
			if(lhs->hType && rhs->hType && !(lhs->hType.kind==skCompType && rhs->hType.kind==skCompType) &&
				!(lhs->hType.kind==skMethod && rhs->hType.kind==skMethod) && lhs->hType != rhs->hType && !context.trap->isSuperClass(lhs->hType,rhs->hType))
			{
				HType hControl = context.trap->getTypeHandle(util::hashString(L"xcontrol"));
				HType hVariant = context.trap->getTypeHandle(util::hashString(L"variant"));
				if((int)lhs->hType != (int)hVariant && (int)rhs->hType != (int)hVariant)
				{
					if(!( hControl && (int) hControl==(int)rhs->hType &&
						context.trap->isSuperClass( hControl,lhs->hType)))
					{
						if(!(context.trap->isOleItem(lhs->hType) && context.trap->isOleItem(rhs->hType)))
						{
							if(!(lhs->hType.kind==skCompType && context.trap->isIntType(rhs->hType) &&
								rhs->kind==kValue && ((OpValue*)rhs)->vt==OpValue::I32 &&
								((OpValue*)rhs)->val_i32==0))
							{
								if(!context.trap->isSuperClass(lhs->hType,rhs->hType))
								{
									pushError(lhs->srcRow?lhs:rhs,error::err_text,wcsdup(((std::wstring)L"赋值操作需要相同的类型1. "+
										context.trap->getTermName(rhs->hType)+L"->"+
										context.trap->getTermName(lhs->hType)).c_str()), errMgr);
								}
							}
						}
					}
				}
			}
		}
		expr->hType = lhs->hType;
		return expr;
	default:
		assert(false);
		return expr;
	}
}

TNode* checkLiteral(TContext &context,OpValue* expr,vlErrorSvr * errMgr)
{
	OpValue * term = expr;
	HType hType = 0;
	switch(term->vt)
	{
	case OpValue::I32:
		hType = context.trap->GetTypeInt();
		expr->hType = hType;
		return expr;
	case OpValue::UI32:
		hType = context.trap->GetTypeUInt();
		expr->hType = hType;
		return expr;
	case OpValue::D32:
		hType = context.trap->GetTypeFloat();
		expr->hType = hType;
		return expr;
	case OpValue::D64:
		hType = context.trap->GetTypeDouble();
		expr->hType = hType;
		return expr;
	case OpValue::BOOL:
		hType = context.trap->GetTypeBool();
		expr->hType = hType;
		return expr;
	case OpValue::STR:
		hType = context.trap->GetTypeString();
		expr->hType = hType;
		return expr;
	default:
		assert(false);
		return expr;
	}
}

TNode* checkItem(TContext &context,OpItem* expr,vlErrorSvr * errMgr)
{
	return checkQualifiedId(context,expr,errMgr);
}

TNode* checkTypeId(TContext &context,OpItem* node,vlErrorSvr * errMgr)
{
	return checkQualifiedId(context,node,errMgr);
}

TNode* checkQualifiedId(TContext & context,OpItem* expr,vlErrorSvr * errMgr)
{
	//type
	OpItem * ty = expr;
	HType hType = 0;
	Handle ht = 0;

	if(expr->hTerm) return expr;

	if(!ty->nest)
	{
		int ident = ty->ident; 
		const wstring_t id = util::getHashString(ident);
		int index = context.getHashContext()->find_token(ident);
		if(index<1 && !ht)
		{
			ht = context.trap->getTypeHandle(ident);
			if(ht) hType = ht;
			if(!ht && classes.size())
			{
				ht = context.trap->lookupItem(classes.back(),ident);
				hType = context.trap->GetItemType(ht);
			}
		}
		if(!ht)
		{
			if(index < 1)
			{
				pushError(ty,error::err_undefinedtype,ident,errMgr);
				ht = 0;
				hType = context.trap->GetTypeUnknown();
			}else
			{
				ht = context.getHashContext()->get_word(index);
				hType = context.trap->GetItemType(ht);
			}
		}
		expr->hType = hType;
		expr->hTerm = ht;
	}else
	{

		std::vector<OpItem* > nodes;
		nodes.push_back(ty);
		while(ty->nest)
		{
			ty= ty->nest;
			nodes.push_back(ty);
		}
		for(int i= (int)nodes.size() - 1;i>=0; i--)
		{
			int ident = nodes[i]->ident; 
			if(!ht)
			{
				int it = context.getHashContext()->find_token(ident);
				if(it < 0)
				{
					pushError(nodes[i],error::err_undefinedtype,ident,errMgr);
					hType = context.trap->GetTypeUnknown();
					ht = 0;
				}else
				{
					ht = context.getHashContext()->get_word(it);
					hType = context.trap->GetItemType(ht);
				}
			}else
			{
				ht = context.trap->lookupItem(ht,nodes[i]->ident);
				if(!ht)
				{
					pushError(nodes[i],error::err_askclassornamespace,ident,errMgr);
					hType = context.trap->GetTypeUnknown();
					ht = 0;
				}else
				{
					hType = context.trap->GetItemType(ht);
				}
			}
		}
		expr->hType = hType;
		expr->hTerm = ht;
	}
	if(ty->rank)
	{
		
		if(expr->hTerm.kind==skType || expr->hTerm.kind==skCompType)
		{//type
			Handle hMyType(skCompType,(int)context.trap->symTable.comptypes.size()+ 1);
			context.trap->addCompTypeItem(expr->hTerm);
			OpList * rank = (OpList *)ty->rank;
			while(rank)
			{
				SNode* rnode = checkExpression(context,rank->node,errMgr);
				if(rnode->kind==kEmpty)
					context.trap->insertCompTypeRank(hMyType,0);
				else
				{
					if(rnode->kind!=kValue || ((OpValue*)rnode)->vt!=OpValue::I32)
					{
						pushError(ty,error::err_needconstintrank,errMgr);
						context.trap->insertCompTypeRank(hMyType,0);
					}
					else
						context.trap->insertCompTypeRank(hMyType,
							((OpValue*)rnode)->val_i32);
				}
				context.trap->insertCompTypeItem(hMyType,context.trap->getTypeHandle(util::hashString(L"array")));
				rank = rank->next;
			}
			expr->hTerm = hMyType;
			expr->hType = hMyType;
		}else
		{//item
			HType hType = expr->hType;
			OpList * rank = (OpList *)ty->rank;
			while(rank)
			{	
				SNode* rnode = checkExpression(context,rank->node,errMgr);
				if(hType.kind!=skCompType)
				{
					hType = context.trap->GetTypeUnknown();
					pushError(ty,error::err_needarraytype,errMgr);
				}
				else
					hType = (HType)context.trap->stepCompType(hType);
				expr = (OpItem *)opElement(0,expr,rnode);
				rank = rank->next;
			}
			expr->hType = hType;
		}
	}
	return expr;
}

TNode* checkExpression(TContext & context,TNode* node,vlErrorSvr * errMgr)
{
	//unknown type
	HType hUnknown = context.trap->GetTypeUnknown();

	HType	hType;
	TNode * expr = node;

	switch(expr->kind)
	{
	//primary
	case kThis:
	case kSuper:
		//check class scope
		if(!classes.size())
		{
			pushError(expr,error::err_usedforclassmethod,expr->kind==kThis?L"this":L"super",errMgr);
			hType = hUnknown;
		}else
			hType = expr->kind==kThis?classes.back():context.trap->getParent(classes.back());
		expr->hType = hType;
		return expr;
	case kValue:
		return checkLiteral(context,(OpValue*)expr,errMgr);
	case kItem:
		return checkItem(context,(OpItem *)expr,errMgr);
	case kNill:
		hType = context.trap->GetTypeNill();
		expr->hType = hType;
		return expr;
	//postfix
	case kInc:
	case kDec:
		((OpDec*)expr)->expr->isLeftVaule = true;
		((OpDec*)expr)->expr = checkExpression(context,((OpDec*)expr)->expr,errMgr);
		expr->hType = ((OpDec*)expr)->expr->hType;
		if(!context.trap->isNumberType(expr->hType))
		{
			pushError(expr,error::err_neednumbertype,errMgr);
		}
		return expr;
	case kField:
		{
			((OpField*)expr)->base = checkExpression(context,((OpField*)expr)->base,errMgr);
			OpField* term = (OpField*)expr;
			//HField hField =  context.trap->lookupIn(term->base->hType,id);

			HType hType = term->base->hType;
			if(context.trap->isIntType(hType)) hType = context.trap->getTypeHandle(util::hashString(L"xint"));
			else if(context.trap->isDoubleType(hType)) hType = context.trap->getTypeHandle(util::hashString(L"xdouble"));
			else if(context.trap->isStringType(hType)) hType = context.trap->getTypeHandle(util::hashString(L"xstring"));

			HField hField = term ->hField;
			if(!term->hField)
			{
				int id = GetNodeIdent(term->hitem);
				hField =  context.trap->lookupItem(hType,id);
				if(!hField || (hField.kind != skField && hField.kind != skMethod &&
					hField.kind != skOleField && hField.kind != skOleMethod))
				{
					pushError(expr,error::err_needfield,id,errMgr);
				}
				term ->hField = hField;
			}
			expr->hType = context.trap->GetItemType(hField);
			if(!expr->hType)expr->hType = context.trap->getTermType(hField);
		}
		return expr;
	case kElement:
		((OpElement*)expr)->exp  = checkExpression(context,((OpElement*)expr)->exp,errMgr);
		((OpElement*)expr)->rank = checkExpression(context,((OpElement*)expr)->rank,errMgr);
		if(((OpElement*)expr)->exp->hType.kind!=skCompType)
		{
			pushError(((OpElement*)expr)->exp,error::err_needarraytype,errMgr);
		}
		if((int)context.trap->getCompTypeItem(((OpElement*)expr)->exp->hType)
			!=(int)context.trap->getTypeHandle(util::hashString(L"array")))
		{
			pushError(((OpElement*)expr)->exp,error::err_needarraytype,errMgr);
		}
		if(!context.trap->isIntNumberType(((OpElement*)expr)->rank->hType))
		{
			pushError(((OpElement*)expr)->rank,error::err_neednumbertype,errMgr);
		}
		expr->hType = context.trap->stepCompType(((OpElement*)expr)->exp->hType);
		return expr;
	case kCall:
		{
			OpCall* term = (OpCall*)node;
			term->mt =checkExpression(context,term->mt,errMgr);
			hType = term->mt->hType;
			HMethod hMethod;
			if(hType.kind != skMethod && hType.kind!= skOleMethod)
			{
				pushError(term->mt,error::err_needmethod,errMgr);
			}else
				hMethod = (HMethod)hType;
			
			OpList* args = (OpList*)term->args;
			std::vector<HType> typs;

			if(context.trap->getParent(hMethod))
			{
				HType hParent = (HType)context.trap->getParent(hMethod);
				if(hParent.index == context.trap->getTypeHandle(util::hashString(L"xstring")).index)
					typs.push_back(context.trap->getTypeHandle(util::hashString(L"string")));
				else if(hParent.index == context.trap->getTypeHandle(util::hashString(L"xint")).index)
					typs.push_back(context.trap->getTypeHandle(util::hashString(L"int")));
				else if(hParent.index == context.trap->getTypeHandle(util::hashString(L"xdouble")).index)
					typs.push_back(context.trap->getTypeHandle(util::hashString(L"double")));
			}
			while(args)
			{
				if(args->node && args->node->kind!=kEmpty)
				{
					args->node->isArg = true;
					args->node = checkExpression(context,args->node,errMgr);
					typs.push_back(args->node->hType);
				}
				args = args->next;
			}
			if(hMethod)
			{
				int ident = context.trap->getMethod(hType)->name;
				hMethod = context.trap->findMatchedMethod(context.trap->getParent(hMethod),methodes.back(),ident,(int)typs.size(),typs);
				if((int)hMethod==0x10)
					pushError(term,error::err_unknownmethod,ident,errMgr);
				else if((int)hMethod==0x20)
					pushError(term,error::err_method_arg,ident,errMgr);
				else
				{
					term->mt->hType = (HType)hMethod;
					term->hType = context.trap->getMethod(hMethod)->hType;
				}
			}
			return term;
		}
	//unary
	case kUInc:
	case kUDec:
		((OpUInc*)expr)->expr->isLeftVaule = true;
	case kNeg:
		((OpUInc*)expr)->expr = checkExpression(context,((OpUInc*)expr)->expr,errMgr);
		expr->hType = ((OpUInc*)expr)->expr->hType;
		if(!context.trap->isNumberType(expr->hType))
		{
			pushError(expr,error::err_neednumbertype,errMgr);
		}
		return expr;
	case kSizeof:
		{
			int msize = 0;
			((OpSizeof*)expr)->expr = checkExpression(context,((OpSizeof*)expr)->expr,errMgr);
			expr->hType = ((OpSizeof*)expr)->expr->hType;
			int nlen = context.trap->getAllocSize(expr->hType,msize);
			return opValue(nlen);
		}
	case kNew:
		{
			((OpNew*)expr)->typ = checkExpression(context,((OpNew*)expr)->typ,errMgr);
			if(((OpNew*)expr)->rank) ((OpNew*)expr)->rank = checkExpression(context,((OpNew*)expr)->rank,errMgr);
			return expr;
		}
	case kDelete:
		{
			((OpDelete*)expr)->exp = checkExpression(context,((OpDelete*)expr)->exp,errMgr);
			return expr;
		}
	case kNot:
		{
			((OpNot*)expr)->expr =checkExpression(context,((OpNot*)expr)->expr,errMgr);
			expr->hType = context.trap->GetTypeBool();
			return expr;
		}
	case kBNot:
		{
			((OpBNot*)expr)->expr =checkExpression(context,((OpBNot*)expr)->expr,errMgr);
			expr->hType = ((OpBNot*)expr)->expr->hType;
			if(!context.trap->isIntNumberType(expr->hType))
			{
				pushError(expr,error::err_neednumbertype,errMgr);
			}
			return expr;
		}
	//cast
	case kCast:
		{
			((OpCast*)expr)->expr =  checkExpression(context,((OpCast*)expr)->expr,errMgr);
			((OpCast*)expr)->typ =  checkExpression(context,((OpCast*)expr)->typ,errMgr);
			expr->hType = ((OpCast*)expr)->typ->hType;
		}
		return expr;
	//binary
	case kBinary:
		{
			node = normalizeExpr(node);
			if(node->kind==kBinary)
				return checkBinary(context,(OpBinary*)node,errMgr);
			else
				return checkExpression(context,node,errMgr);
		}
	case kEmpty:
		return node;
	default:
		assert(false);
		return 0;
	}
	return 0;
}

#if 0
TNode* checkExpression(TContext & context, TNode* node)
{
	struct Type
	{
		static HType getPrimaryType(TContext & context, const wchar_t * typeName)
		{
			return context.trap->getTypeHandle(util::hashString((const wstring_t)typeName));
		}
		static HType getType(TContext &context, Handle ht)
		{
			if(!ht) return 0;
			switch(ht.kind)
			{
			case skType:
				return ht;
			case skField:
				return context.trap->getField(ht)->hType;
			case skNamespace:
				return ht;
			case skMethod:
				return context.trap->getMethod(ht)->hType;
			case skVariant:
				return context.trap->getVariant(ht)->hType;
			case skParam:
				return context.trap->getParam(ht)->hType;
			default:
				assert(false);
				return 0;
			}
		}
	} ;

	struct State
	{
		enum Kind
		{
			sField,
			sEleExpr, 
			sEleRank, 
			sCall,
			sNot,
			sNeg,
			sBNot,
			sInc,
			sDec,
			sUInc,
			sUDec,
			sBinary,
			sBinaryrhs,
		} kind;
		TNode* node;
		int param;
		State(Kind kind):kind(kind),node(0),param(0){}
		State(Kind kind, TNode* node):kind(kind),node(node),param(0){}
		State(Kind kind, TNode* node, int param):kind(kind),node(node),param(param){}
	};

	struct Error
	{
		unsigned int errorno;
		wchar_t* errText;
		int row;
		Error(wchar_t* errText):errorno(0),errText(errText),row(0){}
	};

	//error info
	std::vector<Error> errors;
	std::vector<Error> warns;
	
	//state info
	std::vector<State> states;

	//unknown type
	HType hUnknown = Type::getPrimaryType(context,L"unknown");

	HType	hType;
	bool bExit = false;
	TNode * expr = node;
	while(true)
	{
		if(bExit)
		{
			bExit = false;
			if(!states.size()) break;
			State state = states.back();
			states.pop_back();
			switch(state.kind)
			{
			case State::sField:
				{
					OpField* term = (OpField*)state.node;
					int id = GetNodeIdent(term->hitem);
					HField hField = context.trap->lookupIn(hType,id);
					if(!hField || hField.kind != skField)
					{
						printf("\n无效的字段名'%S'",util::getHashString(id));
					}
					hType = Type::getType(context,hField);
				}
				bExit=true;
				continue;
			case State::sEleExpr: 
				{
					OpElement* term = (OpElement*)state.node;
					//判断是否是数组类型
					expr = term->rank;
					states.push_back(State(State::sEleRank,state.node,hType));
				}
				continue;
			case State::sEleRank:
				{
					//判断rank是否为整形类型 
					//取元素类型
					bExit = true;
				}
				continue;
			case State::sCall:
			case State::sNot:
				{
					hType = Type::getPrimaryType(context,L"bool");
				}
				bExit = true;
				continue;
			case State::sBNot:
				{
					if( (int)hType!=(int)Type::getPrimaryType(context,L"int") && 
						(int)hType!=(int)Type::getPrimaryType(context,L"uint"))
					{
						errors.push_back(Error(L"'~'操作数要求是整数类型."));
					}
				}
				bExit = true;
				continue;
			case State::sNeg:
			case State::sInc:
			case State::sDec:
			case State::sUInc:
			case State::sUDec:
				{
					if( (int)hType!=(int)Type::getPrimaryType(context,L"int") && 
						(int)hType!=(int)Type::getPrimaryType(context,L"uint") &&
						(int)hType!=(int)Type::getPrimaryType(context,L"float") && 
						(int)hType!=(int)Type::getPrimaryType(context,L"double"))
					{
						errors.push_back(Error(L"'-'操作数要求是数字类型."));
					}
				}
				bExit = true;
				continue;
			case State::sBinary:
				expr = ((OpBinary*)state.node)->rhs;
				states.push_back(State(State::sBinaryrhs,state.node,hType));
				continue;
			case State::sBinaryrhs:
				{
					OpBinary::Op op = ((OpBinary*)state.node)->op;
					switch(op & 0x7f)
					{
					case OpBinary::add:	
					case OpBinary::sub:
					case OpBinary::mul:
					case OpBinary::div:
					case OpBinary::mod:
						if((int)hType==(int)Type::getPrimaryType(context,L"string") ||
							(int)state.param==(int)Type::getPrimaryType(context,L"string"))
						{
							if((int)hType!=(int)state.param)
							{
								errors.push_back(Error(L"字符串相加需要字符串类型."));
							}
							continue;
						}
						else if((int)hType!=(int)Type::getPrimaryType(context,L"int") && 
							(int)hType!=(int)Type::getPrimaryType(context,L"uint") &&
							(int)hType!=(int)Type::getPrimaryType(context,L"float") && 
							(int)hType!=(int)Type::getPrimaryType(context,L"double"))
						{
							errors.push_back(Error(L"'+-*/'操作数要求是数字类型."));
						}
						bExit = true;
						continue;
					case OpBinary::shl:
					case OpBinary::shr:
					case OpBinary::band:
					case OpBinary::bor:
					case OpBinary::xor:
						//判断操作数是否是整数类型
						if( (int)hType!=(int)Type::getPrimaryType(context,L"int") && 
							(int)hType!=(int)Type::getPrimaryType(context,L"uint"))
						{
							errors.push_back(Error(L"'>>'或'<<'操作数要求是整数类型."));
						}
						bExit = true;
						continue;

					case OpBinary::or:
					case OpBinary::and:
					case OpBinary::eq:
					case OpBinary::neq:
					case OpBinary::gt:
					case OpBinary::gte:
					case OpBinary::lt:
					case OpBinary::lte:
						if((int)hType!=(int)state.param)
						{
							errors.push_back(Error(L"关系操作需要相同的类型."));
						}
						hType = Type::getPrimaryType(context,L"bool");
						bExit = true;
						continue;

					case OpBinary::asn:
						if((int)hType!=(int)state.param)
						{
							errors.push_back(Error(L"赋值操作需要相同的类型."));
						}
						bExit = true;
						continue;
					default:
						assert(false);
						return 0;

					//case ref
					//case pointer
					//case mref
					//case mpointer
					//case varrow
					//case arrow
					}
				}
				continue;
			default:
				assert(false);
				return 0;
			}
		}
		bExit = false;
		switch(expr->kind)
		{
		//primary
		case kThis:
			if(classes.size())
			{
				errors.push_back(Error(L"this需要用在类函数上"));
				hType = hUnknown;
			}else
				hType = classes.back();
			bExit = true;
			continue;
		case kSuper:
			if(classes.size())
			{
				errors.push_back(Error(L"this需要用在类函数上"));
				hType = hUnknown;
			}else
				hType = context.trap->getParent(classes.back());
			bExit = true;
			return 0;
		case kValue:
			{
				OpValue * term = (OpValue*)expr;
				switch(term->vt)
				{
				case OpValue::I32:
					hType = Type::getPrimaryType(context,L"int");
					bExit = true;
					continue;
				case OpValue::UI32:
					hType = Type::getPrimaryType(context,L"uint");
					bExit = true;
					continue;
				case OpValue::D32:
					hType = Type::getPrimaryType(context,L"float");
					bExit = true;
					continue;
				case OpValue::D64:
					hType = Type::getPrimaryType(context,L"double");
					bExit = true;
					continue;
				case OpValue::BOOL:
					hType = Type::getPrimaryType(context,L"bool");
					bExit = true;
					continue;
				case OpValue::STR:
					hType = Type::getPrimaryType(context,L"string");
					bExit = true;
					continue;
				default:
					assert(false);
					return 0;
				}
			}
			break;
		case kItem:
			{
				OpItem * term = (OpItem *)expr;
				Handle ht = 0;
				bool hasError = false;
				if(term->qualifiedId->kind==kList)
				{
					OpList * terms = (OpList*)term->qualifiedId;
					while(true)
					{
						if(terms->node)
						{
							OpToken * term = (OpToken*)terms->node;
							if(!ht)
							{
								int it = context.getHashContext()->find_token(term->tk.inum);
								if(it < 0)
								{
									printf("\n未定义变量: '%S'", util::getHashString(term->tk.inum));
									hType = Type::getPrimaryType(context,L"unknown");
								}else
								{
									hType = Type::getType(context,context.getHashContext()->get_word(it));
								}
							}else
							{
								ht = context.trap->lookupItem(ht,term->tk.inum);
								if(!ht)
								{
									printf("\n未定义变量: '%S'", util::getHashString(term->tk.inum));
									hType = Type::getPrimaryType(context,L"unknown");
								}else
									hType = Type::getType(context,ht);
							}
						}
						if(!terms->next)break;
						terms = terms->next;
					}
				}else
				{
					//type
					OpType * ty = (OpType*)term->qualifiedId;
					if(!ty->axis)
					{
						int t = context.getHashContext()->find_token(ty->ident);
						if(t==-1)
						{
							printf("\n未定义类型: '%S'",util::getHashString(ty->ident));
							hType = Type::getPrimaryType(context,L"unknown");
						}else
						{
							hType = Type::getType(context,context.getHashContext()->get_word(t));
						}
					}else
					{
						std::vector<OpType* > nodes;
						nodes.push_back(ty);
						while(ty->axis)
						{
							ty= ty->axis;
							nodes.push_back(ty);
						}
						for(std::size_t i= nodes.size() - 1;i>=0; i--)
						{
							if(!ht)
							{
								int it = context.getHashContext()->find_token(nodes[i]->ident);
								if(it < 0)
								{
									printf("\n未定义变量: '%S'", util::getHashString(nodes[i]->ident));
									hType = Type::getPrimaryType(context,L"unknown");
								}else
								{
									ht = context.getHashContext()->get_word(it);
								}
							}else
							{
								ht = context.trap->lookupItem(hType,nodes[i]->ident);
								if(!ht)
								{
									printf("\n未定义变量: '%S'", util::getHashString(nodes[i]->ident));
									hType = Type::getPrimaryType(context,L"unknown");
								}else
									hType = Type::getType(context,ht);
							}
						}
					}
				}
				term->hTerm = ht;
			}
			bExit = true;
			continue;
		case kNill:
			hType = Type::getPrimaryType(context,L"nilltyp");
			bExit = true;
			continue;
		//postfix
		case kInc:
			states.push_back(State(State::sInc));
			expr = ((OpInc*)expr)->expr;
			continue;
		case kDec:
			states.push_back(State(State::sDec));
			expr = ((OpDec*)expr)->expr;
			continue;
		case kField:
			states.push_back(State(State::sField,expr));
			expr = ((OpField*)expr)->base;
			continue;
		case kElement:
			states.push_back(State(State::sEleExpr,expr));
			expr = ((OpElement*)expr)->exp;
			continue;
		case kCall:
			assert(false);
			return 0;
			//expr = ((OpCall*)node)->mt;
			//continue:
		
		//unary
		case kUInc:
			states.push_back(State(State::sUInc));
			expr = ((OpUInc*)expr)->expr;
			continue;
		case kUDec:
			states.push_back(State::sUDec);
			expr = ((OpUDec*)expr)->expr;
			continue;
		case kNeg:
			states.push_back(State::sNeg);
			expr = ((OpUDec*)expr)->expr;
			continue;
		case kNew:
			{
				expr =((OpNew*)expr)->typ;
				bExit = true;
				continue;
			}
		case kDelete:
			{
				expr =((OpDelete*)expr)->exp;
				bExit = true;
				continue;
			}
		case kNot:
			{
				states.push_back(State::sNot);
				expr =((OpNot*)expr)->expr;
				continue;
			}
		case kBNot:
			{
				states.push_back(State::sBNot);
				expr =((OpNot*)expr)->expr;
				continue;
			}
		//cast
		case kCast:
			assert(false);
			return 0;
		//binary
		case kBinary:
			{
				states.push_back(State(State::sBinary,expr));
				OpBinary * term = (OpBinary*)expr;
				expr = term->lhs;
				continue;
			}
		default:
			assert(false);
			return 0;
		}
	}
	for(std::size_t  i=0;i<errors.size();i++)
	{
		printf("\n%S", errors[i].errText);
	}
	return 0;
}
#endif

TNode* setFunctionBody(TContext & context, TNode* node)
{
	if(context.trap->getMethod(methodes.back())->precode)
	{
		OpCompStmt * stmt = (OpCompStmt *)node;
		OpList * pre = (OpList *)context.trap->getMethod(methodes.back())->precode;
		if(stmt->stmts->kind==kList)
			pre->last()->next = (OpList*)stmt->stmts;
		else
			pre = opList(pre,stmt->stmts);
		stmt->stmts = pre;
		context.trap->getMethod(methodes.back())->precode = 0;
	}
	context.trap->getMethod(methodes.back())->vcode = node;
	return node;
}

TNode* checkNode(TContext & context, TNode* node)
{
	int errIndex = (int)errorMgr.getErrors().size();
	TNode * result = checkNode(context,node,&errorMgr);
	errorMgr.printError(errIndex);
	return result;
}

TNode* checkNode(TContext & context, TNode* node,vlErrorSvr * errMgr)
{
	if(!node) return 0;
	switch(node->kind)
	{
	case kIf:
		{
			OpIf * term = (OpIf*)node;
			term->cond = checkExpression(context,term->cond,errMgr);
			term->tbody = checkNode(context,term->tbody,errMgr);
			if(term->fbody) term->fbody = checkNode(context,term->fbody,errMgr);
		}
		break;
	case kFor:
		{
			OpFor * term = (OpFor*)node;
			context.getHashContext()->enter_scope();
			if(term->init)term->init = checkNode(context,term->init,errMgr); 
			if(term->cond)term->cond = checkExpression(context,term->cond,errMgr);
			if(term->inc)term->inc = checkExpression(context,term->inc,errMgr); 
			term->body = checkNode(context,term->body,errMgr);
			context.getHashContext()->leave_scope();
		}
		break;
	case kWhile:
		{
			OpWhile * term = (OpWhile*)node;
			if(term->cond)term->cond = checkExpression(context,term->cond,errMgr);
			term->body = checkNode(context,term->body,errMgr);
		}
		break;
	case kUntil:
		{
			OpUntil * term = (OpUntil*)node;
			term->body = checkNode(context,term->body,errMgr);
			if(term->cond)term->cond = checkExpression(context,term->cond,errMgr);
		}
		break;
	case kReturn:
		{
			OpReturn * term = (OpReturn*)node;
			if(term->expr)
				term->expr = checkExpression(context,term->expr,errMgr);
		}
		break;
	case kContinue:
		break;
	case kBreak:
		break;
	case kCompStmt:
		{
			OpCompStmt * term = (OpCompStmt*)node;
			context.getHashContext()->enter_scope();
			if(term->stmts->kind==kList)
			{
				OpList *  terms = (OpList *)term->stmts;
				while(true)
				{
					if(terms->node) 
						terms->node = checkNode(context,terms->node,errMgr);
					if(!terms->next)break;
					terms = terms->next;
				}
			}else
				term->stmts = checkNode(context,term->stmts,errMgr);
			context.getHashContext()->leave_scope();
		}
		break;
	case kExprStmt:
		{
			OpExprStmt* term = (OpExprStmt*)node;
			term->expr = checkExpression(context,term->expr,errMgr);
		}
		break;
	case kVar:
		{
			OpVar*	term = (OpVar *) node;
			checkTypeId(context,(OpItem*)term->typ,errMgr);
			HType hType = term->typ->hType;
			if(context.getHashContext()->lookup_currentscope(term->ident))
			{
				pushError(term,error::err_existvar,term->ident,errMgr);
			}
			HMethod hMethod = methodes.back();
			HVariant hVariant = context.trap->addVariant(hMethod,term->ident,0,hType);
			term->hVariant = hVariant;
			context.getHashContext()->push_word(hVariant,term->ident);
			if(term->expr) term->expr=checkExpression(context, term->expr,errMgr);
		}
		break;
	case kList:
		{
			OpList * terms = (OpList *)node;
			while(true)
			{
				if(terms->node) 
					terms->node = checkNode(context,terms->node,errMgr);
				if(!terms->next)break;
				terms = terms->next;
			}
		}
		break;
	case kItem:
	case kThis:
	case kSuper:
	case kValue:
	case kNill:
	case kField:
	case kElement:
	case kMethod:
	case kCall:
	case kArgList:
	case kInc:
	case kDec:
	case kUInc:
	case kUDec:
	case kNew:
	case kDelete:
	case kNot:
	case kBNot:
	case kNeg:
	case kCast:
	case kBinary:
	case kParen:
		assert(false);
		break;
	case kType:
		assert(false);
		break;
	case kParam:
		assert(false);
		break;
	case kToken:
		assert(false);
		break;
	}
	return node;
}

TNode* normalizeConstBinary(OpBinary::Op op,OpValue* lhs, OpValue* rhs)
{
#define ConstOP(op)	\
	if(lhs->vt==OpValue::I32 && (rhs->vt==OpValue::I32 || rhs->vt==OpValue::I16 || rhs->vt==OpValue::I8))	\
		return opValue(lhs->val_i32 op rhs->val_i32);	\
	if(lhs->vt==OpValue::I16 && (rhs->vt==OpValue::I16 || rhs->vt==OpValue::I8))	\
		return opValue(lhs->val_i16 op rhs->val_i16);	\
	if(lhs->vt==OpValue::I8 && rhs->vt==OpValue::I8)	\
		return opValue(lhs->val_i8 op rhs->val_i8);	\
	if(lhs->vt==OpValue::UI32 && (rhs->vt==OpValue::UI32 || rhs->vt==OpValue::UI16 || rhs->vt==OpValue::UI8))	\
		return opValue(lhs->val_ui32 op rhs->val_ui32);	\
	if(lhs->vt==OpValue::UI16 && (rhs->vt==OpValue::UI16 || rhs->vt==OpValue::UI8))	\
		return opValue(lhs->val_ui16 op rhs->val_ui16);	\
	if(lhs->vt==OpValue::UI8 && rhs->vt==OpValue::UI8)	\
		return opValue(lhs->val_ui8 op rhs->val_ui8);

#define ConstDblOP(op)	\
	if(lhs->vt==OpValue::D64 && rhs->vt==OpValue::D64)	\
		return opValue(lhs->val_d64 op rhs->val_d64);	\
	if(lhs->vt==OpValue::D64 && rhs->vt==OpValue::I32)	\
		return opValue(lhs->val_d64 op rhs->val_i32);	\
	if(lhs->vt==OpValue::I32 && rhs->vt==OpValue::D64)	\
		return opValue(lhs->val_i32 op rhs->val_d64);	

	switch(op)
	{
	case OpBinary::add:
		ConstOP(+)
		ConstDblOP(+)
		break;
	case OpBinary::sub:
		ConstOP(-)
		ConstDblOP(-)
		break;
	case OpBinary::mul:
		ConstOP(*)
		ConstDblOP(*)
		break;
	case OpBinary::div:
		ConstOP(/)
		ConstDblOP(/)
		break;
	case OpBinary::shl:
		ConstOP(<<)
		break;
	case OpBinary::shr:
		ConstOP(>>)
		break;
	case OpBinary::xor:
		ConstOP(^)
		break;
	case OpBinary::band:
		ConstOP(&)
		break;
	case OpBinary::bor:
		ConstOP(|)
		break;
	case OpBinary::and:
		ConstOP(&&)
		break;
	case OpBinary::or:
		ConstOP(||)
		break;
	case OpBinary::mod:
		ConstOP(%)
		break;
	case OpBinary::eq:
		ConstOP(=)
		break;
	case OpBinary::neq:
		ConstOP(!=)
		break;
	case OpBinary::gt:
		ConstOP(>)
		break;
	case OpBinary::gte:
		ConstOP(>=)
		break;
	case OpBinary::lt:
		ConstOP(<)
		break;
	case OpBinary::lte:
		ConstOP(<=)
		break;
	default:
	assert(false);
	return 0;
	}
	assert(false);
	return 0;
}

TNode* normalizeExpr(TNode* expr)
{
	switch(expr->kind)
	{
	//primary
	case kThis:
	case kSuper:
	case kValue:
	case kNill:
	case kItem:
		return expr;
	//postfix
	case kInc:
		((OpInc*)expr)->expr = normalizeExpr(((OpInc*)expr)->expr);
		return expr;
	case kDec:
		((OpInc*)expr)->expr = normalizeExpr(((OpDec*)expr)->expr);
		return expr;
	case kField:
		((OpField*)expr)->base = normalizeExpr(((OpField*)expr)->base);
		return expr;
	case kElement:
		((OpElement*)expr)->exp = normalizeExpr(((OpElement*)expr)->exp);
		return expr;
	case kCall:
		{
			TNode* mt = ((OpCall*)expr)->mt;
			std::vector<TNode*> terms;
			if(((OpCall*)expr)->args)
			{
				if(((OpCall*)expr)->args->kind!=kList)
				{
					terms.push_back(((OpCall*)expr)->args);
				}else
				{
					TNodeList* args = (TNodeList*)((OpCall*)expr)->args;
					while(true)
					{
						terms.push_back(args->node);
						if(!args->next) break;
						args = args->next;
					}
				}
			}
			for(int i= (int)terms.size() - 1;i>=0;i--)
			{
				terms[i] = normalizeExpr(terms[i]);
			}
		}
		return expr;
	//unary
	case kUInc:
		((OpUInc*)expr)->expr = normalizeExpr(((OpUInc*)expr)->expr);
		return expr;
	case kUDec:
		((OpUDec*)expr)->expr = normalizeExpr(((OpUDec*)expr)->expr);
		return expr;
	case kNeg:
		((OpNeg*)expr)->expr = normalizeExpr(((OpNeg*)expr)->expr);
		return expr;
	case kNew:
		((OpNeg*)expr)->typ = normalizeExpr(((OpNeg*)expr)->typ);
		if(((OpNew*)expr)->rank)((OpNew*)expr)->rank = normalizeExpr(((OpNew*)expr)->rank);
		return expr;
	case kDelete:
		((OpDelete*)expr)->exp = normalizeExpr(((OpDelete*)expr)->exp);
		return expr;
	case kNot:
		((OpNot*)expr)->expr = normalizeExpr(((OpNot*)expr)->expr);
		return expr;
	case kBNot:
		((OpBNot*)expr)->expr = normalizeExpr(((OpBNot*)expr)->expr);
		return expr;
	case kSizeof:
		((OpSizeof*)expr)->expr = normalizeExpr(((OpSizeof*)expr)->expr);
		return expr;
	//cast
	case kCast:
		((OpCast*)expr)->expr = normalizeExpr(((OpCast*)expr)->expr);
		return expr;
	//binary
	case kBinary:
		{
			OpBinary * term = (OpBinary*)expr;
			std::vector<SNode*> terms;
			std::vector<OpBinary::Op> ops;
			terms.push_back(normalizeExpr(term->lhs));
			terms.push_back(normalizeExpr(term->rhs));
			ops.push_back(term->op);

			OpBinary* node = 0;
			for(int i=0;i<(int)term->ops.size() || ops.size();)
			{
				if(i<(int)term->ops.size())
				{
					if(!ops.size() || vlContext::getOperatorPriortyLevel(0,ops.back()) <
						vlContext::getOperatorPriortyLevel(0,term->ops[i]))
					{
						terms.push_back(normalizeExpr(term->terms[i]));
						ops.push_back(term->ops[i]);
						i++;
						continue;
					}
				}

				TNode* rhs = terms.back();
				terms.pop_back();
				TNode* lhs = terms.back();
				terms.pop_back();
				
				OpBinary::Op op = ops.back();
				ops.pop_back();

				if(lhs->kind==kValue && rhs->kind==kValue)
				{
					terms.push_back(normalizeConstBinary(op,(OpValue*)lhs,(OpValue*)rhs));
				}else
					terms.push_back(opBinary(0,op,lhs,rhs,true));
			}
			terms[0]->isLeftVaule = expr->isLeftVaule;
			terms[0]->isArg = expr->isArg;
			return terms[0];
		}
	case kEmpty:
		return expr;
	default:
		assert(false);
		return 0;
	}
}


}//namespace complier
}//namespace trap