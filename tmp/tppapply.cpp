#include "vlpreparse.hpp"
#include "machine_x86.hpp"
#include "vlcoder.hpp"
#include "vlole.hpp"

using namespace XM;
using namespace xlang;

namespace trap {
namespace complier {

extern  std::vector<xlang::HNamespace> namespaces;
extern  std::vector<xlang::HType> classes;
extern  std::vector<xlang::HMethod> methodes;
extern  vlErrorSvr errorMgr;

enum TermKind : unsigned char 
{
	TermCount = 1,
	TermAny = 2,
	TermAnyType = 3,
	TermType = 4,
	TermDef = 5,
	TermIdent = 6,
	TermValue = 7,
	TermCond = 8,
	TermEllipsis = 9,
	TermMethod = 10
};

struct TermPos
{
	int index;
	TermPos * parent;
	TermPos(TermPos * parent,int index) :index(index), parent(parent){}
	bool isSame(TermPos * rhs)
	{
		return index == rhs->index && parent == rhs->parent;
	}
};

struct TermItem
{
	TermKind kind;
	TermPos* pos;
	union
	{
		int ident;
		OpValue* val;
	};
	
	TermItem(TermKind kind, TermPos* pos,OpValue* val):kind(kind),pos(pos),val(val){}
	TermItem(TermKind kind, TermPos* pos,int ident):kind(kind),pos(pos),ident(ident){}
	bool isSame(TermItem * rhs)
	{
		return kind == rhs->kind && pos == rhs->pos && val == rhs->val;
	}
};

struct Term
{
	TermItem* termItem;
	unsigned usescount;
	unsigned int uses;
};

class Checker
{
	public:
	std::vector<TermPos*> poses;
	std::vector<Term> terms;
	int applycount;
	int ident;

	public:
		Checker():applycount(0){}
	public:
	TermPos * getTermPos(TermPos * parent, int index)
	{
		for(int i=0;i<(int)poses.size();i++)
		{
			if(poses[i]->parent == parent && poses[i]->index==index)
				return poses[i];
		}
		return 0;
	}
	
	TermPos * sureTermPos(TermPos * parent, int index)
	{
		for(int i=0;i<(int)poses.size();i++)
		{
			if(poses[i]->parent == parent && poses[i]->index==index)
				return poses[i];
		}
		poses.push_back(new TermPos(parent,index));
		return poses.back();
	}
	
	int addCheck( int aIndex, TermItem* tItem)
	{
		//if(tItem->kind ==TermIdent) return -1;
		if(aIndex >= applycount) applycount = aIndex+1;
		for(int i=0;i<(int)terms.size();i++)
		{
			if(terms[i].termItem->isSame(tItem))
			{
				terms[i].usescount++;
				terms[i].uses |= 1<<aIndex;
				return i;
			}
		}
		Term tm;
		tm.termItem = tItem;
		tm.usescount = 1;
		tm.uses = 1<<aIndex;
		terms.push_back(tm);
		return (int)terms.size() - 1;
	}
	
	int getTerm(int startIndex, int applyIndex, TermPos * pos)
	{
		assert(applyIndex >=0);
		for(int i=startIndex; i<(int)terms.size();	i++)
		{
			if(terms[i].termItem->pos ==pos && terms[i].uses & (1<<applyIndex)) return i;
		}
		return -1;
	}
	
	int getTerm(int applyIndex, TermPos * pos ,TermKind termKind)
	{
		return   getTerm(0,applyIndex, pos,termKind);
	}
	
	int getTerm(int startIndex, int applyIndex, TermPos * pos ,TermKind termKind)
	{
		assert(applyIndex >=0);
		for(int i=startIndex; i<(int)terms.size();	i++)
		{
			if(terms[i].termItem->pos ==pos && terms[i].termItem->kind == termKind && terms[i].uses & (1<<applyIndex)) return i;
		}
		return -1;
	}
	
	int getIdent(const wchar_t* ident)
	{
		return util::hashString((const xlang::wstring_t)ident);
	}

	const wchar_t * getIdentString(int ident)
	{
		return util::getHashString(ident);
	}
	
	std::wstring dump(TContext &context,int applyIndex, TermPos* pos,int &vIndex)
	{
		wchar_t buf[64];
		std::wstring str =L"(";
		
		if(getTerm(applyIndex,pos,TermCount)>=0)
		{
			int acount = terms[getTerm(applyIndex,pos,TermCount)].termItem->val->val_i32;
			for(int i=0; i<acount;i++)
			{
				if(i) str+=L", ";
				if(getTerm(applyIndex,sureTermPos(pos,i),TermAny)>=0)
				{
					str +=L" - ";
				}
				else if(getTerm(applyIndex,sureTermPos(pos,i),TermDef)>=0)
				{
					str =  str + getIdentString(terms[getTerm(applyIndex,sureTermPos(pos,i),TermDef)].termItem->ident)+L"()";
				}
				else if(getTerm(applyIndex,sureTermPos(pos,i),TermValue)>=0)
				{
					swprintf_s(buf,64,L"%d",terms[getTerm(applyIndex,sureTermPos(pos,i),TermValue)].termItem->val->val_i32);
					str+= buf ;
				}
				else
				{
					 if(getTerm(applyIndex,sureTermPos(pos,i),TermIdent)>=0)
					{
						str +=  getIdentString(terms[getTerm(applyIndex,sureTermPos(pos,i),TermIdent)].termItem->ident);
					}else
					{
						swprintf_s(buf,64,L"t%d",++vIndex);
						str += buf;
					}
					
					int tIndex = getTerm(applyIndex,sureTermPos(pos,i),TermType);
					if(tIndex >= 0)
					{
						str += (std::wstring)L":" + getIdentString(context.trap->getType(terms[tIndex].termItem->val->val_i32)->name);
						str +=  dump(context,applyIndex,sureTermPos(pos,i),vIndex);
					}
				}
			}
		}
		str +=L")";
		return str;
	}

	std::wstring dump(TContext &context)
	{
		std::wstring dstr;
		for(int i=0; i < applycount;i++)
		{
			int vIndex = 0;
			dstr += getIdentString(ident) + dump(context,i,NULL,vIndex) + L";\n";
		}
		return dstr;
	}
	
	std::wstring dumpCond(TContext &context,TermPos * pos, unsigned int uses, std::wstring space)
	{
		std::wstring dstr;
		std::wstring str;
		wchar_t buf[128];
		for(int i=0; i <(int)terms.size();i++)
		{
			//normal
			str =L"";
			if((terms[i].termItem->kind ==TermType || terms[i].termItem->kind ==TermValue ||
					terms[i].termItem->kind ==TermDef ) && terms[i].usescount>=2 && terms[i].termItem->pos == pos 
					&& terms[i].uses & uses )
			{
				std::wstring bodyStr;
				if(getTermPos(pos->parent,pos->index+1) && terms[i].uses & uses)
				{
					bodyStr = space + L"{\n" + dumpCond(context,getTermPos(pos->parent,pos->index+1),terms[i].uses & uses,space+L"   ") + space + L"}\n";
				}else
					bodyStr = space + L"{\n"+ space +L"}\n";
				
				if(terms[i].termItem->kind==TermType)
				{
					swprintf_s(buf,128,L"typeof(t.Term(%d))", terms[i].termItem->pos->index);
					str = (std::wstring)L"if(" + buf +L" instance of " + getIdentString(
						context.trap->getType((HType)terms[i].termItem->val->val_i32)->name)+L")\n"+bodyStr;
				}
				else if(terms[i].termItem->kind==TermValue)
				{
					swprintf_s(buf,128,L"t.Term(%d).Value == %d", terms[i].termItem->pos->index,
						terms[i].termItem->val->val_i32);
					str = (std::wstring)L"if(" + buf +L")\n"+bodyStr;
				}
				/*else if(terms[i].termItem->kind==TermCount)
				{
					swprintf_s(buf,128,L"t.CountParam() == %d", terms[i].termItem->val);
					str = (std::wstring)+L"if(" + buf +L")\n"+bodyStr;
				}*/
				else if(terms[i].termItem->kind==TermDef)
				{
					swprintf_s(buf,128,L"t.Term(%d) == ", terms[i].termItem->pos->index);
					str = (std::wstring)L"if(" + buf + getIdentString(terms[i].termItem->ident)+L")\n"+bodyStr;
				}
				if(dstr!=L"") 
					dstr +=space+ L"else " + str;
				else
					dstr += space + str;
			}
		}
		return dstr;
	}
		
	std::wstring dumpCond(TContext &context)
	{
		std::wstring dstr;
		std::wstring paramCountStr;
		std::vector<int> simpleTerms;
		wchar_t buf[128];
		std::wstring space=L"   ";
		
		for(int i=0; i <(int)terms.size();i++)
		{
			//param count
			if(terms[i].termItem->kind==TermCount && terms[i].termItem->pos == NULL)
			{
				if(paramCountStr!=L"") paramCountStr+=L" && ";
				swprintf_s(buf,128,L"t.CountParam()==%d", terms[i].termItem->val->val_i32);
				paramCountStr += buf;
			}
			
			//simple term
			if((terms[i].termItem->kind==TermType || terms[i].termItem->kind==TermValue ||
					terms[i].termItem->kind==TermDef)
					&& terms[i].usescount==1 && terms[i].termItem->pos && !terms[i].termItem->pos->parent )
			{
				simpleTerms.push_back(i);
			}
		}
		
		//比较容易判断的先做，特化的先做，后补的规则优先前期的规则
		//先判断常数
		//判断常量
		//判断类型
		//生成决策树
		//生成代码
		//更据参数，实例化模板
		
		//param
		dstr += space + L"if(!(" + paramCountStr +L") return -1;\n";
		//simple
		for(int i=0;i<(int)simpleTerms.size();i++)
		{
			int apply = -1;
			int uses = terms[simpleTerms[i]].uses;
			while(uses){
				uses >>= 1;
				apply++;
			}
			
			std::wstring str;
			if(terms[simpleTerms[i]].termItem->kind==TermType)
			{
				swprintf_s(buf,128,L"typeof(t.Term(%d))", terms[simpleTerms[i]].termItem->pos->index);
				str = space + L"if(" + buf +L" instance of " + getIdentString(context.trap->getType(
					(HType)terms[simpleTerms[i]].termItem->val->val_i32)->name)+L")\n" + space +L"{\n";
			}else if(terms[simpleTerms[i]].termItem->kind==TermValue)
			{
				swprintf_s(buf,128,L"t.Term(%d).Value == %d", terms[simpleTerms[i]].termItem->pos->index,
					terms[simpleTerms[i]].termItem->val->val_i32);
				str = space + L"if(" + buf +L")\n"+ space +L"{\n";
			}else if(terms[simpleTerms[i]].termItem->kind==TermDef)
			{
					swprintf_s(buf,128,L"t.Term(%d) == ", terms[simpleTerms[i]].termItem->pos->index);
					str =space + L"if(" + buf + getIdentString(terms[simpleTerms[i]].termItem->ident)+L")\n"+ space +L"{\n";
			}
			swprintf_s(buf,128,L"%d", apply);
			str += space + space + L"   if(t.MatchApply(" + buf+L"))" + L" return "+ buf +L"; else return -1;\n";
			str += space + L"}\n";
			
			dstr += str;
		}
		//normal
		dstr += dumpCond(context,sureTermPos(0,0),0xffffffff,space);
		
		return dstr;
	};
};

void AddTermsCheck(Checker &checker,int apply,TermPos * pos, OpList * terms)
{
	int i = apply;
	if(terms->kind==kList)
	{
		int arg =-1;
		while(terms)
		{
			OpParam* term = (OpParam*)terms->node;
			arg++;
			if(term->ident)
				checker.addCheck(i,new TermItem(TermIdent,checker.sureTermPos(pos,arg),term->ident));
			if(term->typ)
				checker.addCheck(i,new TermItem(TermType,checker.sureTermPos(pos,arg),opValue((int)term->typ->hType)));
			if(term->expr && term->expr->kind==kValue)
				checker.addCheck(i,new TermItem(TermValue,checker.sureTermPos(pos,arg),(OpValue*)term->expr));
			if(term->terms)
				AddTermsCheck(checker,i,checker.sureTermPos(pos,arg),(OpList*)term->terms);
			terms = terms->next;
		}
		if(arg>=0)checker.addCheck(i,new TermItem(TermCount,pos,opValue(arg+1))); 
	}
}

struct TermNode
{
	TermNode * child;
	TermNode * next;
	TermNode * parent;

	TermItem * term;
	TermNode(TermItem * term) : child(0),next(0),parent(0),term(term){}
};

TermNode * AddFunc(TContext &context,TermNode * root, HMethod hMethod,std::vector<TermNode*> &itemsFunc)
{
	if(!root) root = new TermNode(new TermItem(TermAny,0,0));
	
	//sure same param node
	int paramCount = context.trap->getMethod(hMethod)->paramCount;
	if(paramCount < 0)  return 0;
	TermNode * node = root->child;
	TermNode * prenode = 0;
	while(node)
	{
		if(node->term && node->term->kind==TermCount && node->term->val->val_i32 == paramCount)break;
		prenode = node;
		node = node->next;
	}
	if(!node)
	{
		node = new TermNode(new TermItem(TermCount,0,opValue(paramCount)));
		node->parent = root;
		if(!prenode) 
			root->child = node;
		else
			prenode->next = node;
	}
	itemsFunc.push_back(node);
	
	//comp function param and soure param node
	TermNode * snode = node->child;
	prenode = 0;
	HParam hParam = context.trap->getMethod(hMethod)->hParam;
	hParam.index--;
	for(int i=0;i<paramCount;i++)
	{
		hParam.index++;

		//sure snode
		if(!snode)
		{
			snode = new TermNode(new TermItem(TermAny,0,0));
			snode->parent = node;
			if(prenode)
			{
				prenode->next = snode;
			}else
			{
				node->child = snode;
			}
		}

		HType hType = context.trap->getParam(hParam)->hType;
		//'-'
		if(context.trap->getParam(hParam)->any)
		{
			itemsFunc.push_back(snode);
			continue;
		}
		//'undef type'	
		TermNode * curnode = snode->child;
		if(!curnode)
		{
			curnode = new TermNode(new TermItem(TermAnyType,0,0));
			snode->child = curnode;
		}
		if((int)hType==(int)context.trap->GetTypeUnknown())
		{
			itemsFunc.push_back(curnode);
			continue;
		}
		
		//type
		if(!curnode->child)
		{
			curnode->child = new TermNode(new TermItem(TermType,0,opValue((int)hType)));
			curnode->child->parent = curnode;
			curnode = curnode->child;
		}else
		{//sure type match node
			curnode = curnode->child;
			TermNode* lnode = 0; //left node
			while(true)
			{
				HType hTypeNode = curnode->term->val->val_i32;
				if((int)hType == (int)hTypeNode)break;
				if(context.trap->isSuperClass(hTypeNode,hType))
				{
					if(!curnode->child)
					{
						curnode->child = new TermNode(new TermItem(TermType,0,opValue((int)hType)));
						curnode->child->parent = curnode;
						curnode=curnode->child;
						break;
					}else
					{
						//continue search
						curnode=curnode->child;
						continue;
					}
				}
				else if(context.trap->isSuperClass(hType,hTypeNode))
				{//insert type and merge some child type
					TermNode * xnode = new TermNode(new TermItem(TermType,0,opValue((int)hType)));
					xnode->parent =curnode->parent;
					if(lnode)
						lnode->next = xnode;
					else
						curnode->parent->child = xnode;
					xnode->next = curnode->next;
					curnode->next = 0;
					xnode->child = curnode;
					curnode->parent = xnode;

					TermNode * wnode = xnode;
					TermNode * unode = wnode->next;
					TermNode * vnode = curnode;
					while(unode)
					{
						if(context.trap->isSuperClass(hType,(HType)unode->term->val->val_i32))
						{
							wnode->next = unode->next;
							unode->next = 0;
							unode->parent = xnode;
							vnode->next = unode;
							vnode = vnode->next;
							unode = wnode->next;
						}else
						{
							wnode = wnode->next;
							unode = wnode->next;
						}
					}
					curnode = xnode;
					break;
				}
				//search right
				lnode = curnode;
				curnode = curnode->next;
				if(!curnode)
				{
					curnode = new TermNode(new TermItem(TermType,0,opValue((int)hType)));
					lnode->next = curnode;
					break;
				}
			}
		}
		//skip param terms

		//const check
		if(context.trap->getParam(hParam)->value)
		{
			int val = ((OpValue*)context.trap->getParam(hParam)->value)->val_i32;
			if(!curnode->child)
			{
				curnode->child = new TermNode(new TermItem(TermValue,0,(OpValue*)context.trap->getParam(hParam)->value));
				curnode->child->parent = curnode;
				curnode = curnode->child;
			}else
			{
				curnode = curnode->child;
				TermNode* lnode = 0; //left node
				while(curnode)
				{
					if(curnode->term->kind==TermValue && 
						((OpValue*)context.trap->getParam(hParam)->value)->val_i32 ==
						curnode->term->val->val_i32) break;
					lnode = curnode;
					curnode = lnode->next;
				}
				if(!curnode)
				{
					curnode = new TermNode(new TermItem(TermValue,0,(OpValue*)context.trap->getParam(hParam)->value));
					curnode->parent = lnode->parent;
					lnode->next = curnode;
				}
			}
		}
		itemsFunc.push_back(curnode);
		prenode = snode;
		snode = snode->next;
	}
	return root;
}

std::wstring dumpFuncDecl(TContext &context,int id,std::vector<TermNode*> nodes)
{
	std::wstring str;
	wchar_t buf[80];
	if(!nodes.size()) return str;
	
	std::size_t index = 0;
	while(index < nodes.size())
	{
		int paramCount = nodes[index]->term->val->val_i32;
		str += (std::wstring)L"\n\tapply " + util::getHashString(id) +L"(";
		for(int i=0;i<paramCount;i++)
		{
			if(i) str +=L",";
			TermNode* lnode = nodes[index+i+1];
			if(lnode->term->kind==TermType)
			{
				str += (std::wstring)L" :" +util::getHashString(context.trap->getType(lnode->term->val->val_i32)->name);
			}
			else if(lnode->term->kind==TermValue)
			{
				if(lnode->term->val->vt==OpValue::STR)
					str += (std::wstring)L" \""+lnode->term->val->val_wstr+L"\"";
				else if(lnode->term->val->vt==OpValue::BOOL)
					str += lnode->term->val->val_bool?L" true ":L" false ";
				else
				{
					swprintf_s(buf,80,L" %d",lnode->term->val->val_i32);
					str += buf;
				}
			}			
		}
		str +=L")";
		index += paramCount+1;
	}

	return str;
}

std::wstring dumpFuncNode(TContext &context,TermNode * node)
{
	std::wstring str;
	wchar_t buf[80];
	if(!node) return str;
	
	TermNode * lnode = node;
	if(lnode->next)str += L"(";
	while(lnode)
	{
		if(lnode->term->kind==TermType)
		{
			str += (std::wstring)L" " +util::getHashString(context.trap->getType(lnode->term->val->val_i32)->name);
		}
		else if(lnode->term->kind==TermValue)
		{
			if(lnode->term->val->vt==OpValue::STR)
				str += (std::wstring)L" \""+lnode->term->val->val_wstr+L"\"";
			else if(lnode->term->val->vt==OpValue::BOOL)
				str += lnode->term->val->val_bool?L" true ":L" false ";
			else
			{
				swprintf_s(buf,80,L" %d",lnode->term->val->val_i32);
				str += buf;
			}
		}
		if(lnode->child)
			str+=(std::wstring)L"->" + dumpFuncNode(context,lnode->child);
		lnode = lnode->next;
	}
	if(node->next)str += L")";
	return str;
}

/*
	return：
		   -2  不能比较
	       -1  小于
		   0   相等
		   1   大于
*/
int CompFuncParam(TContext &context, HParam hParam1, HParam hParam2)
{
	if((int)hParam1==(int)hParam2) return 0; //eq
	SymParam * param1 = context.trap->getParam(hParam1);
	SymParam * param2 = context.trap->getParam(hParam2);
	if(param1->any && param2->any) return 0;
	if(param1->any) return -1; //lt
	if(param2->any) return 1;  //gt;

	int result = -2;
	if((int)param1->hType == (int)param2->hType) result = 0;
	if((int)param1->hType==(int)context.trap->GetTypeUnknown()) return result==0?0:-1;
	if((int)param2->hType==(int)context.trap->GetTypeUnknown()) return result==0?0:1;
	
	if(result !=0 )
	{
		if(context.trap->isSuperClass(param1->hType,param2->hType)) return -1;
		else if(context.trap->isSuperClass(param2->hType,param1->hType)) return 1;
		else return -2; 
	}
	
	if(param1->value && param2->value)
	{
		if(((OpValue*)param1->value)->val_i32 == ((OpValue*)param2->value)->val_i32) return 0;
		return -2;
	}
	if(param1->value) return 1;
	if(param2->value) return -1;

	return 0;
}

/*
	return：
		   -2  不能比较
	       -1  小于
		   0   相等
		   1   大于
*/
int CompFunc(TContext &context, HMethod hMethod1, HMethod hMethod2)
{
	if((int)hMethod1==(int)hMethod2) return 0; //eq
	SymMethod * method1 = context.trap->getMethod(hMethod1);
	SymMethod * method2 = context.trap->getMethod(hMethod2);
	int paramCount1 = method1->paramCount;
	int paramCount2 = method2->paramCount;
	if(paramCount1 != paramCount2) return -2; //not company
	HParam hParam1 = method1->hParam;
	HParam hParam2 = method2->hParam;

	int result = 0;
	
	for(int i=0;i<paramCount1;i++)
	{
		int current = CompFuncParam(context,hParam1,hParam2);
		if(current==-2) return -2;
		if(i==0)
			result = current;
		else if(result==0)
			result = current;
		else if(result != current && current!=0)
			return -2;
		
		hParam1.index++;
		hParam2.index++;
	}

	return result;
}

TermNode * InsertFunc(TContext &context,TermNode * prenode,TermNode * curnode, TermNode * tnode,HMethod hMethod)
{
	//prenode should parent
	if(!curnode)
	{
		prenode->child = tnode;
		tnode->parent = prenode;
		return curnode;
	}

	bool parent = prenode == curnode->parent? true:false;
	int result = CompFunc(context,(HMethod)curnode->term->val->val_i32,hMethod);
	if(result==0) //eq
	{
		tnode->parent = curnode->parent;
		tnode->next = curnode->next;
		curnode->next = tnode;
		return tnode;
	}else if(result==-1) //lt
	{
		return InsertFunc(context,curnode,curnode->child,tnode,hMethod);
	}else if(result==1) //gt
	{
		tnode->parent = curnode->parent;
		curnode->parent = tnode;
		tnode->child = curnode;
		tnode->next = curnode->next;
		curnode->next = 0;
		if(prenode->child ==curnode)
			prenode->child = tnode;
		else
			prenode->next = tnode;

		TermNode * wnode = tnode;
		TermNode * unode = wnode->next;
		TermNode * vnode = curnode;
		while(unode)
		{
			if(CompFunc(context,(HMethod)unode->term->val->val_i32,hMethod)==1)
			{
				wnode->next = unode->next;
				unode->next = 0;
				unode->parent = tnode;
				vnode->next = unode;
				vnode = vnode->next;
				unode = wnode->next;
			}else
			{
				wnode = wnode->next;
				unode = wnode->next;
			}
		}
		curnode = tnode;
		return tnode;
	}else //if(result==-2) //not comp
	{
		if(!curnode->next)
		{
			curnode->next = tnode;
			tnode->parent = curnode->parent;
			return tnode;
		}else
		{
			return InsertFunc(context,curnode,curnode->next,tnode,hMethod);
		}
	}
}

TermNode * InsertFunc(TContext &context,TermNode * root, HMethod hMethod)
{
	//sure same param node
	int paramCount = context.trap->getMethod(hMethod)->paramCount;
	if(paramCount < 0)  return 0;
	TermNode * node = root->child;
	TermNode * prenode = 0;
	while(node)
	{
		if(node->term && node->term->kind==TermCount && node->term->val->val_i32 == paramCount)break;
		prenode = node;
		node = node->next;
	}
	if(!node)
	{
		node = new TermNode(new TermItem(TermCount,0,opValue(paramCount)));
		node->parent = root;
		if(!prenode) 
			root->child = node;
		else
			prenode->next = node;
	}
	
	TermNode * curnode = node->child;
	TermNode * tnode = new TermNode(new TermItem(TermMethod,0,opValue((int)hMethod)));
	return InsertFunc(context,node,curnode,tnode,hMethod);
}

std::wstring dumpFuncDecl(TContext &context,HMethod hMethod,std::wstring space)
{
	std::wstring str;
	SymMethod* method = context.trap->getMethod(hMethod);
	int paramCount = method->paramCount;
	HParam hParam = method->hParam;
	str += space + (std::wstring)L"apply " + util::getHashString(method->name)+L"(";
	wchar_t buf[80];
	for(int i=0;i<paramCount;i++)
	{
		if(i) str+=L", ";
		SymParam * param = context.trap->getParam(hParam);
		if(param->name)
		{
			std::wstring name = util::getHashString(param->name);
			if(!wcsstr(name.c_str(),L"__param"))str += name;
		}
		if(!param->any)
		{
			if(param->value)
			{
				OpValue *val = (OpValue*)param->value;
				if(val->vt==OpValue::STR)
				str += (std::wstring)L" \"" + val->val_wstr+L"\"";
				else if(val->vt==OpValue::BOOL)
					str += val->val_bool?L" true ":L" false ";
				else
				{
					swprintf_s(buf,80,L" %d",val->val_i32);
					str += buf;
				}
			}
			else if((int)param->hType !=(int)context.trap->GetTypeUnknown())
				str += (std::wstring)L" :"+ context.trap->getTermName(param->hType);
		}else
			str +=L"-";

		hParam.index++;
	}
	str +=L");\n";
	return str;
}

std::wstring dumpFuncTreeNode(TContext &context,TermNode * node,std::wstring space)
{
	std::wstring str;
	if(!node) return str;
	
	TermNode * lnode = node;
	while(lnode)
	{
		str += space +dumpFuncDecl(context,(HMethod)lnode->term->val->val_i32,space);
		if(lnode->child)
			str+= dumpFuncTreeNode(context,lnode->child,space+L"   ");
		lnode = lnode->next;
	}
	return str;
}

std::wstring dumpFuncTree(TContext &context,TermNode * root)
{
	std::wstring str;
	std::wstring space;
	if(!root) return str;
	TermNode* node = root->child;
	if(!node) return str;
	while(node)
	{
		int paramCount = node->term->val->val_i32;
		wchar_t buf[80];
		swprintf_s(buf,80,L"\n%d param:\n",paramCount);
		space = L"   ";
		str += buf;
		str += dumpFuncTreeNode(context,node->child,space);
		node = node->next;
	}
	return str;
}

std::wstring dumpFunc(TContext &context,TermNode * root, HMethod hMethod)
{
	std::wstring str;
	std::wstring space;
	if(!root) return str;
	TermNode* node = root->child;
	if(!node) return str;
	while(node)
	{
		int paramCount = node->term->val->val_i32;
		wchar_t buf[80];
		swprintf_s(buf,80,L"\n%d param:\n",paramCount);
		space = L"\t";
		str += buf;
		TermNode * snode = node->child;
		for(int i=0;i<paramCount;i++)
		{
			TermNode* tnode = snode->child->child;
			swprintf_s(buf,80,L" param %d:",i);
			str += space + buf;
			str += dumpFuncNode(context,tnode);
			str += L"\n";
			snode = snode->next;
		}
		node = node->next;
	}
	return str;
}

TNode * makeDicisionTree(TContext &context,HMethod hMethod);
TNode * ProcessApply(TContext &context,HType hType)
{
	HMethod mt = context.trap->getType(hType)->hMethod;
	int count = context.trap->getType(hType)->methodCount;
	std::set<int> ids;
	for(int i=0;i<count;i++)
	{
		if(context.trap->getMethod(mt)->kind==mkApply)
		{
			int id = context.trap->getMethod(mt)->name;
			if(ids.find(id)==ids.end())
			{
				ids.insert(id);
				makeDicisionTree(context,mt);
			}
		}
		mt.index++;
	}
	return 0;
}

TNode * makeDicisionTree(TContext &context,HMethod hMethod)
{
 HType hType = context.trap->getParent(hMethod);
 int name = context.trap->getMethod(hMethod)->name;
 HMethod mt = context.trap->getType(hType)->hMethod;
 int count = context.trap->getType(hType)->methodCount;
 std::vector<HMethod> methods;
 for(int i=0;i<count;i++)
 {
	 if(context.trap->getMethod(mt)->name==name && context.trap->getMethod(mt)->kind==mkApply)
		 methods.push_back(mt);
	 mt.index++;
 }
 if(methods.size())
 {
	 TermNode * root = new TermNode(new TermItem(TermAny,0,0));
	 //std::vector<TermNode*>fcts;
	 for(int i=0;i<(int)methods.size();i++)
	 {
		 //root = AddFunc(context,root,methods[i],fcts);
		InsertFunc(context,root,methods[i]);
	 }
	 printf("%S",dumpFuncTree(context,root).c_str());
	 //printf("%S",dumpFunc(context,root,hMethod).c_str());
	 //printf("%S",dumpFuncDecl(context,name,fcts).c_str());
 }

#if 0
 if(methods.size())
 {
	 Checker checker;
	 checker.ident = name;
	 for(int i=0;i<(int)methods.size();i++)
	 {
		mt = methods[i];
 		int paramCount = context.trap->getMethod(mt)->paramCount;
 		checker.addCheck(i,new TermItem(TermCount,NULL,opValue(paramCount)));
		HParam hParam = context.trap->getMethod(mt)->hParam;
		for(int j=0;j<paramCount;j++)
		{
			//param
			HType hType = context.trap->getParam(hParam)->hType;
			//value
			void * val = context.trap->getParam(hParam)->value;
			if(val && ((SNode*)val)->kind==kValue)
			{
				checker.addCheck(i,new TermItem(TermValue,checker.sureTermPos(NULL,j),(OpValue*)val));
			}else
			//type
			if(hType) 
				checker.addCheck(i,new TermItem(TermType,checker.sureTermPos(NULL,j),opValue((int)hType)));

			checker.addCheck(i,new TermItem(TermIdent,checker.sureTermPos(NULL,j),context.trap->getParam(hParam)->name));
			OpList* terms = (OpList*)context.trap->getParam(hParam)->terms;
			if(terms) AddTermsCheck(checker,i,checker.sureTermPos(NULL,j),terms);
			hParam.index++;
		}
	 }
	 printf("%S",checker.dump(context).c_str());
	 printf("%S",checker.dumpCond(context).c_str());
 }
#endif

 return 0;
}


}//namespace complier
}//namespace trap
