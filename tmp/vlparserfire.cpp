#include "vlParser.hpp"
#include "vlUtils.hpp"
#include "vlPlatform.hpp"
#include "vlAstNode.hpp"
#include "vlAstAxis.hpp"
#include "vlAstFactory.hpp"
#include "vlsynnoderef.hpp"
#include "vlapply.hpp"
#include <vector>
#include "vlwpp_runtime.hpp"

using namespace xlang;

//apply-define = "apply" apply-lhs [';' | ':='  apply-rhs   ';' |  '->'  apply-action ];
//apply-alias  = "apply" identifier '='  apply-lhs ';' ;
//apply-contextid = identifier;
//apply-context = apply-contextid ':';

//apply-element = [ apply-context ] identifier;
//apply-param =  apply-lhs|apply-match-any|  identifier | '...'| literal;
//apply-match-any = '-';
//apply-listparam = '[' { apply-param } ']';
//apply-lhs = apply-element [ '(' { apply-listparam|apply-param } ')' ] ;

int vlParser::decodeApplyDefine(vlContext * context,vlAstAxis * anode,
			vlAstAxis*	&lhs , 
			vlAstAxis*	&rhs , 
			vlAstAxis*	&action,
			int	&aliasid,
			int	&contextid,
			int &elementid)
{
	int apply_define		= util::hashString(L"apply-define");
	int apply_alias			= util::hashString(L"apply-alias");
	int apply_process_using	= util::hashString(L"apply-process-using");
	int apply_rhs			= util::hashString(L"apply-rhs");
	int apply_lhs			= util::hashString(L"apply-lhs");
	int apply_action		= util::hashString(L"apply-action");
	int apply_context		= util::hashString(L"apply-context");
	int apply_element		= util::hashString(L"apply-element");

	lhs = 0 ; 
	rhs =0 ; 
	action = 0;
	aliasid = 0;
	contextid = 0;
	elementid = 0;

	//apply-alias  = "apply" identifier '='  apply-lhs ';' ;
	if(anode->getIdent()==apply_alias && anode->ChildNext(3)/*lhs*/)
	{
		aliasid = anode->ChildNext(1)->getIdent();
		lhs = anode->ChildNext(3);
		return 1;
	}
	
	//apply-define = "apply" apply-lhs [';' | ':='  apply-rhs   ';' |  '->'  apply-action ];
	if(anode->getIdent()!=apply_define) return -1;
	if(anode->ChildNext(3) && anode->ChildNext(3)->getIdent() == apply_rhs)
		rhs = anode->ChildNext(3);
	else if(anode->ChildNext(3) && anode->ChildNext(3)->getIdent() == apply_action)
		action = anode->ChildNext(3);
	if(anode->ChildNext(1) && anode->ChildNext(1)->getIdent() == apply_lhs)
	{
		lhs = anode->ChildNext(1);
		//apply-lhs = apply-element [ '(' { apply-listparam|apply-param } ')' ] ;
		if(lhs->ChildNext(0) && lhs->ChildNext(0)->getIdent() == apply_element)
		{
			//apply-element = [ apply-context ] identifier;
			if(lhs->ChildNext(0) && !lhs->ChildNext(0)->ChildNext(1) && lhs->ChildNext(0)->LastChild()->getIdent())
			{
				elementid = lhs->ChildNext(0)->LastChild()->getIdent();
			}
			else
			{
				if(lhs->ChildNext(0) && lhs->ChildNext(0)->ChildNext(0)->getIdent() == apply_context)
				{
					if(lhs->ChildNext(0)->ChildNext(0)->Child() && lhs->ChildNext(0)->ChildNext(0)->Child()->LastChild()->getIdent())
						contextid = lhs->ChildNext(0)->ChildNext(0)->Child()->LastChild()->getIdent();
				}
				if(lhs->ChildNext(0)->ChildNext(1) && lhs->ChildNext(0)->ChildNext(1)->getIdent())
					elementid = lhs->ChildNext(0)->ChildNext(1)->getIdent();
			}
		}
	}

	return 1;
}

int vlParser::fireGener(vlContext * context,vlAstAxis * curNode)
{
	int apply_import	= util::hashString(L"apply-import");
	//os_t::print(L"%s", curNode->printOwner().c_str());
	if(curNode->getIdent()==apply_import)
	{
		const wchar_t * pUrl = curNode->ChildNext(1)->getString();
		wchar_t* pstr = context->wppcontext->fetch_file(pUrl);
		//nextToken();
		if(pstr)
		{
			//切换lexer
			vlLexer * ptr_lexer = new vlLexer;
			ptr_lexer->setSource(pstr,(wchar_t*)pUrl);
			ptr_lexer_pool->push(lexer);
			lexer = ptr_lexer;
			lexer->nextch();
			nextToken();
		}
		return 0;
	}

	if(curNode->getIdent()!=context->declare) return 0;
	//this should constructor
	vlAstAxis* anode = curNode->getChild();
	if(!anode) return 0;

	if(anode->getIdent()!=util::hashString(L"processor-define")) return 0;
	//if(anode->getIdent()!=context->gener) return 0;

	vlAstAxis*	lhs = 0; 
	vlAstAxis*	rhs = 0; 
	vlAstAxis*	action = 0;
	int	aliasid = 0;
	int	contextid = 0;
	int elementid = 0;

	int apply_process_using = util::hashString(L"apply-process-using");
	int apply_context_define = util::hashString(L"apply-context-define");
	int apply_declare_statement = util::hashString(L"apply-declare-statement");
 
	std::wstring str;
	anode = anode->getChild();

	int ide =  anode->getNext()->getChild()->getIdent();
	vlApply * apply = context->applysvr.getApply(ide);
	if(!apply)
	{
		apply = new vlApply(ide);
		context->applysvr.addApply(ide,apply);
	}

	//find list
	while(anode && anode->node->kind != akListNode)anode=anode->getNext();
	if(anode)
	{
		anode = anode->getChild();
		while(anode)
		{
			decodeApplyDefine(context,anode,lhs,rhs,action,aliasid,contextid,elementid);
			if(anode->getIdent()==apply_process_using && anode->ChildNext(2)/*identifier*/)
			{
				apply->addUsingProcessor(anode->ChildNext(2)->getIdent(),
					anode->ChildNext(4)?anode->ChildNext(4)->getIdent():0);
			}else if(anode->getIdent()== apply_declare_statement && (anode->ChildNext(1)))
			{//apply-declare-statement 		=	"var"  apply-declare-item  [ '(' L(apply-declare-item, ',' ) ')' ] ';' ;
				apply->proc_variant(context,anode,this);
			}
			else if(anode->getIdent()==apply_context_define && anode->ChildNext(3))
			{//apply-context-define = "context"  identifier '[' {apply-define} ']';
				int ContextID = anode->ChildNext(1)->getIdent();
				vlAstAxis* bnode = anode->ChildNext(3);
				while(bnode && bnode->node->kind == akListNode)bnode = bnode->getChild();
				apply->hashContext.enter_scope();
				while(bnode)
				{
					decodeApplyDefine(context,bnode,lhs,rhs,action,aliasid,contextid,elementid);
					if(anode->getIdent()== apply_declare_statement && (anode->ChildNext(1)))
					{//apply-declare-statement 		=	"var"  apply-declare-item  [ '(' L(apply-declare-item, ',' ) ')' ] ';' ;
						apply->proc_variant(context,anode,this);
					}
					else if(aliasid)
					{
						if(lhs) apply->addAlias(aliasid,lhs);
					}
					else if(elementid)
					{
						//step into lhs
						int ide = elementid;
						int varIndex = (int)apply->variants.size();
						if(action)
						{
							wstring str = lhs->printOwner().c_str();
							apply->addItem(ide,ContextID,lhs,0,apply->proc_action(context,action,this),varIndex);
						}else
						{
							apply->addItem(ide,ContextID,lhs,rhs);
						}
					}
					bnode = bnode->getSibling();
				}
				apply->hashContext.leave_scope();
			}
			else if(aliasid)
			{
				if(lhs) apply->addAlias(aliasid,lhs);
			}
			else if(elementid)
			{
				//step into lhs
				int ide = elementid;
				int varIndex = (int)apply->variants.size();
				if(action)
				{
					wstring str = lhs->printOwner().c_str();
					apply->addItem(ide,contextid,lhs,0,apply->proc_action(context,action,this),varIndex);
				}else
				{
					apply->addItem(ide,contextid,lhs,rhs);
				}
				
			}
			anode = anode->getSibling();
		}
	}
	return 1;
}

int vlParser::doProcess(vlContext * context,vlAstAxis * curNode)
{
	vlAstAxis* anode = curNode->getChild();
	anode = anode->getChild();

	//os_t::print(L"\n%s",anode->printOwner().c_str());
	//find list
	while(anode && anode->node->kind != akListNode)anode=anode->getNext();
	if(anode)
	{
		anode = anode->getChild();
		while(anode)
		{
			if(anode->getIdent()==util::hashString(L"process-statement"))
			{
				if(anode->getChild() && anode->getChild()->getToken() && getTokenIdent(*anode->getChild()->getToken())==util::hashString(L"fire"))
				{
					if(anode->ChildNext(1) && anode->ChildNext(3) && 
						anode->ChildNext(1)->getIdent() &&
						anode->ChildNext(3)->getIdent())
					{
						std::map<int,int>::iterator iter = context->firesProcess.find(anode->ChildNext(3)->getIdent());
						if(iter!=context->firesProcess.end())
						{
							context->firesProcess[anode->ChildNext(3)->getIdent()] = anode->ChildNext(1)->getIdent();
							//context->firesProcess.insert(iter,std::pair<int,int>(anode->ChildNext(3)->getIdent(),anode->ChildNext(1)->getIdent()));
						}
						else
							context->firesProcess.insert(std::pair<int,int>(anode->ChildNext(3)->getIdent(),anode->ChildNext(1)->getIdent()));
					}
				}else
				{
					vlAstAxis* lhs = anode->getChild();
					vlAstAxis* rhs = anode->ChildNext(1);
					
					if(lhs->getChild()) lhs = lhs->getChild(); //list

					//os_t::print(L"\n%s",lhs->printOwner().c_str());
					//os_t::print(L"\n%s",rhs->printOwner().c_str());

					vlAstAxis* r = (vlAstAxis*)context->modelsvr.getItem(rhs->getIdent()); 
					while(lhs)
					{
						int processor = lhs->getIdent();
						vlApply * apply = context->applysvr.getApply(processor);
						if(!apply)
						{
							format_output(L"\n未找到处理器1%s!", util::getHashString(processor));
							return -1;
						}
						if(!r)
						{
							format_output(L"\n提供给处理器%s的数据没有找到!", util::getHashString(processor));
							return -1;
						}
						r = this->unify_apply(context,apply,r);
						lhs = lhs->getSibling();
					}
				}
			}
			anode = anode->getSibling();
		}
	}

	return 1;

}

int vlParser::fireConstructor(vlContext * context,vlAstAxis * curNode)
{
	if(curNode && curNode->getIdent()!=context->declare) return 0;

	vlAstAxis* anode = curNode->getChild();
	if(!anode) return 0;

	int ide = anode->ChildNext(1)->getIdent();
	context->modelsvr.addItem(ide,curNode);

	if(anode->getIdent() == util::hashString(L"process-define"))
	{
		int ide = anode->ChildNext(1)->getIdent();
		this->doProcess(context,curNode);
	}

	return 1;

	//this should constructor
	if(anode->getIdent()!=context->constructor) return 0;

	std::wstring str;
	anode = anode->getChild();

	//find list
	while(anode && anode->node->kind != akListNode)anode=anode->getNext();

	int constructor_pattern		= util::hashString(L"constructor-pattern");
	int constructor_fire		= util::hashString(L"constructor-fire");
	int constructor_statement	= util::hashString(L"constructor-statement");
	int constructor_option		= util::hashString(L"constructor-option");
	int constructor_list_term   = util::hashString(L"constructor-list-term");

	if(anode)
	{
		anode = anode->getChild();
		while(anode)
		{
			if(anode->getIdent()==constructor_pattern)
			{
				//constructor-pattern((pattern :constructor-lhs(Method) '=' :constructor-lhs(function-define) ';'))
				// /./*[2]/.
				str += (std::wstring)L"\n pattern " + anode->ChildNext(1)->LastChild()->node->printOwner()+ L" = ";
				str += (std::wstring)L" " +  anode->ChildNext(3)->Child()->node->printOwner();
				context->patterns.insert(std::pair<int ,vlAstAxis*>(((vlAstIdentifier*)anode->ChildNext(1)->LastChildNode())->ident,
					anode->ChildNext(3)->LastChild()));
			}
			else if(anode->getIdent()==constructor_fire)
			{
				//:constructor-fire((fire :constructor-lhs(SymMethod) '=' :constructor-lhs(function-define) ';'))
				// /./*[2]/.
				str += (std::wstring)L"\n fire " + anode->ChildNext(1)->LastChildNode()->printOwner()+ L" = ";
				str += (std::wstring)L" " +  anode->ChildNext(3)->LastChildNode()->printOwner();
				context->fires.insert(std::pair<int ,vlAstAxis*>(((vlAstIdentifier*)anode->ChildNext(1)->LastChildNode())->ident,
					anode->ChildNext(3)->LastChild()));
			}else if(anode->getIdent()==constructor_statement)
			{
				//:constructor-statement((:constructor-lhs(Variant) '=' :constructor-rhs((:constructor-term(SymType) :constructor-rhs(:constructor-term(name)))) ';'))
				// /./.
				str += (std::wstring)L"\n struct " + anode->ChildNext(0)->LastChildNode()->printOwner()+ L" = ";
				int ide = ((vlAstIdentifier*)anode->ChildNext(0)->LastChildNode())->ident;
				vlAstAxis* rhs = anode->ChildNext(2)->LastChild();
				std::vector<vlAstNode*> nodes;
				std::vector<bool> blists;
				if(rhs)
				{
					if(rhs->node->kind != akListNode)
						str += (std::wstring)L" " +  rhs->node->printOwner();
					else
					{
						rhs = rhs->getChild();
						while(rhs)
						{
							vlAstAxis* r = rhs->LastChild();
							bool vector = false;
							while(true)
							{
								if(!r->getChild())
								{
									if(r->node->kind==akAxisNode)
									{
										r = (vlAstAxis*)r->node;
										r = r->LastChild();
										continue;
									}
									else
									{
										blists.push_back(vector);
										nodes.push_back(r->Node());
										if(vector)
										{
											str += (std::wstring)L" vector<" +  r->Node()->printOwner()
												+ L"> " + r->Node()->printOwner() + L"List";
										}else
										{
											str += (std::wstring)L" " +  r->Node()->printOwner();
										}
									}
									break;
								}
								if(r->getIdent()==constructor_list_term)
								{
									r = r->ChildNext(2)->LastChild();
									vector = true;
									continue;
								}
								if(r->getIdent()==constructor_option)
									r = r->ChildNext(1)->LastChild();
								else
								{
									str += (std::wstring)L" " +  r->Node()->printOwner();
									break;
								}
							}
							rhs = rhs->getSibling();
						}
					}
				}
				if(nodes.size())
				{
					vlAstAxis * head= 0;
					vlAstAxis * tail = 0;
					for(int i=0;i<(int)nodes.size();i++)
					{
						if(!head)
						{
							head = context->getAstFactory()->CreateAxis(nodes[i]);
							tail = head;
						}else
						{
							tail->setNext(context->getAstFactory()->CreateAxis(nodes[i]));
							tail = tail->getNext();
						}
						if(blists[i]) tail->param = (void *)1;
					}
					context->contructs.insert(std::pair<int ,vlAstAxis*>(ide,head));
				}
			}
			anode = anode->getSibling();
		}
	}
	if(context->bTraceConstructor)
	{
		format_output(L"\r\n-----constructor--");
		if(str!=L"") format_output(L"%s",str.c_str());
	}
	return 1;
}



int vlParser::fireNode(vlContext * context,vlAstAxis * curNode) 
{
	if(context->firesProcess.size() && curNode->child && curNode->node->realKind == akIdentifierNode)
	{
		int ide = ((vlAstIdentifier*)curNode->node)->ident;
		if(context->firesProcess.find(ide)!=context->firesProcess.end())
		{
			int processor = context->firesProcess[ide];
			vlApply * apply = context->applysvr.getApply(processor);
			if(!apply)
			{
				vlTrap* meta = context->trap;
				HType hType = meta->lookupType(meta->getGlobalNamespace(),processor);
				if(hType)
				{
					HMethod hMethod =  meta->findMethod(hType,util::hashString(L"Process"));
					if(!hMethod)
					{
						format_output(L"\n未找到方法Process!");
						return -1;
					}
					int pthis = context->wppcontext->get_runtime()->create_class(context->wppcontext,(int)hType);
					int rt = 0;
					void * faddr =  meta->getMethodAddr(hMethod);
					void * p = this;
					if(faddr)
					{
						_asm push curNode;
						_asm push p;
						_asm push context;
						_asm mov ecx,pthis;
						_asm call faddr;
						_asm mov rt,eax;
					}
					return rt;
				}else
				{
					format_output(L"\n未找到处理器%s!", util::getHashString(processor));
					return -1;
				}
			}
			this->unify_apply(context,apply,curNode);   
		}
	}
	if(context->fires.size() && curNode && curNode->node->realKind == akIdentifierNode)
	{
		int ide = ((vlAstIdentifier*)curNode->node)->ident;
		if(context->fires.find(ide)!=context->fires.end())
		{
			//fire
			if(context->bTraceFire)format_output(L"\r\n------fire %s",util::getHashString(ide));
			ide = ((vlAstIdentifier*)context->fires[ide]->node)->ident;
			if(context->contructs.find(ide)!=context->contructs.end())
			{
				vlAstAxis * rhs = context->contructs[ide];
				while(rhs)
				{
					std::wstring str;
					int id = ((vlAstIdentifier*)rhs->node)->ident;
					if(context->patterns.find(id)!=context->patterns.end()) 
					{
						vlAstAxis* node = context->patterns[id];
						int pattern = 0;
						if(node->LastChildNode()->realKind==akIdentifierNode)
						{
							pattern = ((vlAstIdentifier *)node->LastChildNode())->ident;
						}
						vlAstAxis* r = 0;//node->getChild();
						if(pattern)
						{
							r = curNode->getChild();
							while(r)
							{
								if(r->node->realKind==akIdentifierNode)
								{
									if(((vlAstIdentifier *)r->node)->ident==pattern)break;
									if(r->node->kind==akIdentifierNode && pattern==util::hashString(L"identifier")) break;
								}
								r = r->getNext();
							}
						}
						if(r)
						{
							if(rhs->param) //vector
							{
								str +=(std::wstring)L"[";
								vlAstAxis* rc = r->getChild();
								if(rc->node->kind==akListNode) rc = rc->getChild();
								while(rc)
								{
									str += (std::wstring)L" "  + util::getHashString(id) + (std::wstring)L":" + rc->LastChildNode()->printOwner();
									rc = rc->getSibling();
								}
								str +=(std::wstring)L"]";
								if(context->bTraceFire)format_output(L"%s",str.c_str());
							}else
							{
								if(context->bTraceFire)
								{
									str = (std::wstring)L" "  + util::getHashString(id) + (std::wstring)L":" + r->LastChildNode()->printOwner();
									format_output(L"%s",str.c_str());
								}
							}
						}
					}
					rhs = rhs->getNext();
				}
			}
		}
	}
	return 1;
}

int vlParser::fireTrace(vlContext * context,vlAstAxis * curNode)
{
	if(curNode->getIdent()== context->output_statement)
	{
		vlAstAxis * anode = curNode->ChildNext(1);
		int ide = anode->getIdent();

		if(ide == context->trace_ast)
		{
			context->bTraceAst = true;
			return 1;
		}else if( ide == context->trace_constructor)
		{
			context->bTraceConstructor = true;
			return 1;
		}else if( ide == context->trace_fire)
		{
			context->bTraceFire = true;
			return 1;
		}else if( ide == util::hashString(L"trace-apply"))
		{
			context->bTraceApply= true;
			return 1;
		}else if( ide == util::hashString(L"call"))
		{
			vlAstAxis * applyRhs = 0;
			std::map<int,vlAstAxis*>  paramBind;

			anode = curNode->ChildNext(2);
			if(anode )
			{
				anode = anode->Child();
				int processor = anode->getIdent();
				vlApply * apply = context->applysvr.getApply(processor);
				if(!apply)
				{
					format_output(L"\n未找到处理器%s!", util::getHashString(processor));
					return -1;
				}
				//apply->mItems
				int ide = util::hashString(L"main");
				if(anode->getSibling()) ide = anode->getSibling()->getIdent();
				if(apply->getMap().find(ide) != apply->getMap().end())
				{
					applyRhs = apply->getItem(apply->getMap().lower_bound(ide)->second).code;
				}
				if(!applyRhs)
				{
					format_output(L"\n未指定apply main()!");
					return -1;
				}
				apply->eval(applyRhs,this,context,0,paramBind);
				return 1;
			}else
			{
				format_output(L"\n未指定处理器!");
				return 0;
			}
		}
		else if( ide == util::hashString(L"eval"))
		{
			vlAstAxis * applyRhs = 0;
			std::map<int,vlAstAxis*>  paramBind;

			anode = curNode->ChildNext(2);
			if(anode )
			{
				anode = anode->Child();
				int processor = anode->getIdent();

				Handle hNamespaceApply = context->trap->lookupItem(context->trap->getGlobalNamespace(),util::hashString(L"apply-namespace"));
				if(!hNamespaceApply)
				{
					format_output(L"\n未找到处理器1%s!", util::getHashString(processor));
					return -1;
				}
				Handle hProcess = context->trap->lookupItem(hNamespaceApply,processor);
				if(!hProcess)
				{
					format_output(L"\n未找到处理器2%s!", util::getHashString(processor));
					return -1;
				}

				//apply->mItems
				int ide = util::hashString(L"main");
				if(anode->getSibling()) ide = anode->getSibling()->getIdent();
				Handle hMethod = context->trap->lookupMethod(hProcess,ide);
				if(!hMethod)
				{
					format_output(L"\n未指定apply %s!",util::getHashString(ide));
					return -1;
				}
				void * faddr = context->trap->getMethodAddr(hMethod);
				int rt = 0;
				if(faddr)
				{
					_asm mov esi ,esp;
					_asm call faddr;
					_asm mov rt,eax;
					_asm mov esp,esi;
				}
				return rt;
			}else
			{
				format_output(L"\n未指定处理器!");
				return 0;
			}
		}

		context->traceAstNodes.insert(ide);
		
		wchar_t * name = util::getHashString(ide);
		if(context->rulers.find(ide)!=context->rulers.end())
		{
			vlAstAxis * node = context->rulers[ide];
			this->format_output(L"\r\n%s = ",util::getHashString(ide));
			this->printAstAxis(node);
		}
	}
	return 1;
}

int vlParser::firePrint(vlContext * context,vlAstAxis * curNode)
{
	int ide = curNode->getIdent();
	if(!ide) return 0;

	//this->format_output(L"\n-----@%s",util::getHashString(ide));
	if(context->traceAstNodes.size() && context->traceAstNodes.find(ide)!= context->traceAstNodes.end())
	{
		this->format_output(L"\r\n-----%s:---------",util::getHashString(ide));
		this->format_output(L"\n%s = ",curNode->printOwner().c_str());
	}
	return 1;
}

int vlParser::firePrioritySort(vlContext * context,vlAstAxis * curNode)
{
#if 0
	return 1;

	int ide = curNode->getIdent();
	if(/*ide == util::hashString(L"expression") ||*/ ide == util::hashString(L"function-define") ||
		ide == util::hashString(L"declare"))
	{
		/*if(ide == util::hashString(L"expression"))
			os_t::print(L"\n--expression----\n");
		else
			os_t::print(L"\n--function-define----\n");
		*/
		//vlAstAxis * node = unify_apply(context,apply,curNode);
		//if(node)os_t::print(L"\n---apply result:\n%s\n\n",node->printOwner().c_str());
		return 1;
	}
#endif
	int ide = curNode->getIdent();
	if(!curNode->ChildNext()) return 0;
	if(curNode->ChildNext()->Node()->kind!=akListNode) return 0;
	if(ide != util::hashString(L"binary-expression")) return 0;

	std::vector<int> ops;
	std::vector<vlAstAxis *> nodes;
	std::vector<vlAstAxis *> nodeOps;
	
	nodes.push_back(curNode->Child());
	vlAstAxis * node = curNode->ChildNext()->Child();
	while(node)
	{
		int op = ((vlAstToken *)node->LastChildNode())->getToken()->inum;
		int priorty = context->getOperatorPriortyLevel(0,op);
		while(ops.size() && priorty<=context->getOperatorPriortyLevel(0,ops.back()))
		{
			nodeOps.back()->comeAwayButChild();
			nodes[nodes.size()-1]->comeAwayButChild();
			nodes[nodes.size()-2]->comeAwayButChild();

			//nodes[nodes.size()-1] = nodes[nodes.size()-1]->LastChild();
			//nodes[nodes.size()-2] = nodes[nodes.size()-2]->LastChild();

			nodes[nodes.size()-2]->setNext(nodes[nodes.size()-1]);
			nodeOps.back()->getChild()->setNext(nodes[nodes.size()-2],false);
			nodes[nodes.size()-2] = nodeOps.back();
			nodes.pop_back();
			nodeOps.pop_back();
			ops.pop_back();
		}
		ops.push_back(op);
		nodeOps.push_back(node);
		nodes.push_back(node->getNext());

		node = node->getSibling();
	}

	while(ops.size())
	{
		nodeOps.back()->comeAwayButChild();
		nodes[nodes.size()-1]->comeAwayButChild();
		nodes[nodes.size()-2]->comeAwayButChild();

		//nodes[nodes.size()-1] = nodes[nodes.size()-1]->LastChild();
		//nodes[nodes.size()-2] = nodes[nodes.size()-2]->LastChild();

		nodes[nodes.size()-2]->setNext(nodes[nodes.size()-1]);
		nodeOps.back()->getChild()->setNext(nodes[nodes.size()-2],false);
		nodes[nodes.size()-2] = nodeOps.back();
		nodes.pop_back();
		nodeOps.pop_back();
		ops.pop_back();
	}
	//os_t::print(L"\n%s",nodes[0]->printOwner().c_str());
	nodes[0]->setParent(curNode,false);
	
	return 1;
}

vlAstAxis * vlParser::unify_apply(vlContext * context,vlApply*  apply, vlAstAxis * curNode)
{
	std::map<int,vlAstAxis*>  paramBind;
	if(curNode->node->kind == akListNode)
	{
		vlAstAxis* node = curNode->getChild();
		vlAstAxis* rnode = 0;
		vlAstAxis* anode = 0;
		while(node)
		{
			if(node->Node()->kind == akTokenNode || node->Node()->kind == akIdentifierNode)
				rnode = unify_apply_call(context,apply,curNode,node,paramBind,false);
			else
				rnode = unify_apply(context,apply,node);
			node = node->getSibling();
			//if(!rnode) break;
			if(rnode)
			{
				if(!anode)
					anode = rnode;
				//else
				//	anode->appendSibling(rnode);
			}
		}
		return anode;
	}

	std::multimap<int,int /*index*/ >::iterator uIter,lIter;
	int ide = curNode->getIdent();
	bool unify = false;
	for(int i=0;i<=(int)apply->mProcessores.size();i++) 
	{
		if(i > 0)
		{
			//break;
			apply  = context->applysvr.getApply(apply->mProcessores[i - 1].id);
			if(!apply) break;
		}
		
		if(apply && apply->getMap().find(ide) != apply->getMap().end())
		{
			lIter = apply->getMap().lower_bound(ide);
			uIter = apply->getMap().upper_bound(ide);

			if(context->contexts.size())
			{
				for(lIter--;lIter!=--uIter;)
				{
					paramBind.clear();
					vlAstAxis * lhs = apply->getItem(uIter->second).lhs;
					if(apply->getItem(uIter->second).id != context->contexts.back())continue;
					unify = unify_apply_match(context,apply,curNode,lhs,paramBind);
					if(unify)break;
				}
			}

			if(!unify)
			{
				lIter = apply->getMap().lower_bound(ide);
				uIter = apply->getMap().upper_bound(ide);
				for(lIter--;lIter!=--uIter;)
				{
					paramBind.clear();
					vlAstAxis * lhs = apply->getItem(uIter->second).lhs;
					
					if(apply->getItem(uIter->second).id) continue;
					unify = unify_apply_match(context,apply,curNode,lhs,paramBind);
					if(unify)break;
				}
			}
		}
		if(unify)break; 
	}

	//find match
	if(!unify)
	{
		this->format_output(L"\nwarn:未找到合适的%s apply.\n",util::getHashString(ide));
		//this->format_output((wstring_t)curNode->printOwner().c_str());
		//this->format_output(L"\n");
		return 0;  
	}

	const wchar_t * str = util::getHashString(ide);
	if(apply->getItem(uIter->second).code)
	{
		//os_t::print(L"\n%s",util::getHashString(uIter->first));

		context->enterUnifyApply(util::getHashString(ide),uIter->second,apply->getItem(uIter->second).varCount);
		vlAstAxis * result = unify_apply_action(context,apply,curNode,apply->getItem(uIter->second).code,paramBind);
		apply->loadItemValue(context,result,paramBind);
		context->leaveUnifyApply();
		return result;
	}
	else if(apply->getItem(uIter->second).rhs)
		return unify_apply_call(context,apply,curNode,apply->getItem(uIter->second).rhs,paramBind);
	return 0; 
}

vlAstAxis * vlParser::unify_apply_action(vlContext * context,vlApply* apply,vlAstAxis * curNode,vlAstAxis * applyRhs,std::map<int,vlAstAxis*> & paramBind,bool child)
{
	//os_t::print(L"\n%s",applyRhs->printOwner().c_str());
	//apply->doAction(applyRhs);
	vlAstAxis * result = apply->eval(applyRhs,this,context,curNode,paramBind,child);
	return result;


	vlAstAxis * node = applyRhs; //unify_apply(context,apply,applyRhs);
//	if(node) os_t::print(L"\n%s",node->printOwner().c_str());
	vlAstAxis * rnode = node;
	vlAstAxis * knode = 0;
	while(rnode)
	{
		int ide = rnode->Node()->getIdent();
		if(ide)
		{
			if(paramBind.find(ide)!=paramBind.end())
			{
				knode = unify_apply(context,apply,paramBind[ide]);
			}
		}
		rnode = rnode->getSibling();
	}
	return node;
}

vlAstAxis * vlParser::unify_apply_call(vlContext * context,vlApply* apply,vlAstAxis * curNode,vlAstAxis * applyRhs,std::map<int,vlAstAxis*> & paramBind,bool child)
{
	int apply_rhs = util::hashString(L"apply-rhs");
	int apply_param = util::hashString(L"apply-param");
	int dt_call_param = util::hashString(L"dt-call-param");
	int dt_call = util::hashString(L"dt-call");
	int dt_name = util::hashString(L"dt-name");
	int variant_name = util::hashString(L"variant-name");
	
	vlAstAxis * node = child ? applyRhs->getChild() : applyRhs;

	struct NodeList
	{
		vlContext * context;
		vlAstAxis * head;
		vlAstAxis * tail;
		NodeList(vlContext * context):head(0),tail(0),context(context){}

		void push(vlAstAxis * node,bool strip = false)
		{
			if(!node) return;
			if(strip)
				node = context->getAstFactory()->CreateAxis(node->Node());
			if(tail)
			{
				tail->setNext(node,false);
				tail = node;
			}else
			{
				head = node;
				tail = node;
			}
			while(tail->getNext())tail = tail->getNext(); 
		}
	}nodelist(context);

	while(node)
	{
		if(node->Node()->kind == akTokenNode || 
			(node->Child() && node->Child()->Node()->kind == akTokenNode))
		{
			wchar_t buf[255];
			std::wstring str;
			vlToken& token = node->Node()->kind == akTokenNode?
				*node->Node()->getToken():
				*node->Child()->Node()->getToken();
			if(token.ty == tokenType::tString)
			{
				if(token.snum[0]=='\n' && token.snum[1]=='\0')
					str += token.snum;
				else
				{
					//str = str + L"\"" + token.snum + L"\"";
					str = str + token.snum ;
				}
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
			else if(token.ty == tokenType::tChar)
			{
				//swprintf_s(buf,255,L"'%c'",token.inum);
				swprintf_s(buf,255,L"%c",token.inum);
				str = buf ;
			}
			else if(token.ty == tokenType::tBool)
			{
				//swprintf_s(buf,255,L"'%c'",token.inum);
				if(token.inum)
					swprintf_s(buf,255,L"true");
				else
					swprintf_s(buf,255,L"false");
				str = buf ;
			}
			else if(token.ty == tokenType::tIdent)
			{
				str=  token.snum;
			}
			else if(token.ty == tokenType::tOp)
			{
				str =  opValue::getOperateText(token.inum);
			}else
				assert(false);
			if(context->bTraceApply)format_output(L"%s",str.c_str());

			if(node->Node()->kind == akTokenNode)
				nodelist.push(node,true);
			else
				nodelist.push(node->Child(),true);

			node = node->getNext();
			continue;
		}
		if(node->Node()->kind == akListNode)
		{
			vlAstAxis * snode = node->getChild();
			vlAstAxis * rnode = 0;
			vlAstAxis * anode = 0;
			while(snode)
			{
				//os_t::print(L"\n%s",snode->printOwner().c_str());
				rnode = unify_apply_call(context,apply,curNode,snode,paramBind,false);
				if(rnode)
				{
					if(!anode)
						anode = rnode;
					else
						anode->appendSibling(rnode);
				}
				snode = snode->getSibling();
			}
			if(anode) nodelist.push(anode);
			node = node->getNext();
			continue;
		}
		int ide = node->getIdent();
		if(ide==apply_rhs)
		{
			nodelist.push(unify_apply_call(context,apply,curNode,node,paramBind));
		}
		else if(ide==apply_param)
		{
			ide = node->getChild()/*LastChild()*/->getIdent();
			while(ide)
			{
				if(paramBind.find(ide) != paramBind.end())
				{
					if(paramBind[ide]->getIdent() && !paramBind[ide]->getChild())
					{
						nodelist.push(paramBind[ide],true);
						if(context->bTraceApply)format_output(L" %s ",util::getHashString(paramBind[ide]->getIdent()));
					}
					else if(paramBind[ide]->Node()->getIdent()==variant_name)
					{
						nodelist.push(paramBind[ide]->getChild(),true);
						if(context->bTraceApply)format_output(L" %s",util::getHashString(paramBind[ide]->LastChild()->getIdent()));
					}
					else if(paramBind[ide]->Node()->kind == akTokenNode)
					{
						wchar_t buf[255];
						std::wstring str;
						vlToken& token = *paramBind[ide]->Node()->getToken();
						if(token.ty == tokenType::tString)
							str = str + L"\"" + token.snum + L"\"";
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
						else if(token.ty == tokenType::tChar)
						{
							swprintf_s(buf,255,L"'%c'",token.inum);
							str = buf ;
						}
						else if(token.ty == tokenType::tBool)
						{
							if(token.inum)
								swprintf_s(buf,255,L"true");
							else
								swprintf_s(buf,255,L"false");
							str = buf ;
						}
						else if(token.ty == tokenType::tIdent)
						{
							str=  token.snum;
						}
						else if(token.ty == tokenType::tOp)
						{
							str =  opValue::getOperateText(token.inum);
						}else
							assert(false);
						nodelist.push(paramBind[ide],true);
						if(context->bTraceApply)format_output(L" %s",str.c_str());
					}else
					{
						nodelist.push(unify_apply(context,apply,paramBind[ide]));
					}
					break;
				}else
				{
					if(node->getChild() && !node->getChild()->getNext())
					{
						node = node->getChild();
						if(node->getChild())
							ide = node->getChild()/*LastChild()*/->getIdent();
						else
							break;
					}else
						break;
					//os_t::print(L" %s ",util::getHashString(ide));
				}
			}
		}
		else if(ide==dt_call)
		{
			nodelist.push(unify_apply_call(context,apply,curNode,node,paramBind));
		}
		else if(ide==dt_name) 
		{
			//unproceess ok
			node->LastChildNode()->printOwner();
		}else if(ide==dt_call_param)
		{
			nodelist.push(unify_apply_call(context,apply,curNode,node,paramBind));
		}else
		{
			if(context->bTraceApply)format_output(L"#%s",util::getHashString(ide));
			//assert(false);
			return nodelist.head;
		}
		node = node->getNext();
	}

	return nodelist.head;
}

bool vlParser::unify_apply_match_token(vlContext * context,vlApply* apply,vlAstAxis * curNode,vlAstAxis * applyLhs,std::map<int,vlAstAxis*> & paramBind)
{
	//apply-lhs = apply-element '(' { apply-listparam|apply-param } ')' ;
	//apply-element = identifier
	
	//判断当前节点的id与模板的id是否一致。
	if(curNode->getIdent()!=applyLhs->Child()/*apply-element*/->LastChild()/*identifier*/->getIdent()) return false;

	//判断子项是否一致
	vlAstAxis * node = curNode->getChild();	//获得子项
	vlAstAxis * tnode = applyLhs->ChildNext(2); //获得参数项

	return false;
}

bool vlParser::unify_apply_matchSpecialItem(vlContext * context,vlAstAxis * curNode,int ident,std::map<int,vlAstAxis*> & paramBind)
{
	bool unify = false;
	vlAstNode * node = curNode->Node(); 
	//任何类型的节点都匹配any节点。
	if(ident==util::hashString(L"any"))
		unify = true;
	else if(ident==context->identifier)
	{
		if(node->kind==akTokenNode)
		{
			vlToken& tk = *node->getToken();
			if(tk.ty == tokenType::tBool || tk.ty == tokenType::tDouble ||
				tk.ty == tokenType::tInt || tokenType::tString || tk.ty==tokenType::tChar)
				unify = true;
		}
	}
	else if(ident==context->double_literal)
	{
		if(node->kind==akTokenNode)
		{
			vlToken& tk = *node->getToken();
			if(tk.ty == tokenType::tDouble)
				unify = true;
		}
	}
	else if(ident==context->integer_literal)
	{
		if(node->kind==akTokenNode)
		{
			vlToken& tk = *node->getToken();
			if(tk.ty == tokenType::tInt)
				unify = true;
		}
	}
	else if(ident==context->string_literal)
	{
		if(node->kind==akTokenNode)
		{
			vlToken& tk =  *node->getToken();
			if(tk.ty == tokenType::tString)
				unify = true;
		}
	}
	else if(ident==context->char_literal)
	{
		if(node->kind==akTokenNode)
		{
			vlToken& tk =  *node->getToken();
			if(tk.ty == tokenType::tChar)
				unify = true;
		}
	}
	else if(ident==context->bool_literal)
	{
		if(node->kind==akTokenNode)
		{
			vlToken& tk =  *node->getToken();
			if(tk.ty == tokenType::tBool)
				unify = true;
		}
	}
	else if(ident==context->identifier)
	{
		if(node->kind==akTokenNode)
		{
			vlToken& tk =  *node->getToken();
			if(tk.ty == tokenType::tIdent)
				unify = true;
		}
	}
	else if(ident==util::hashString(L"operate"))
	{
		if(node->kind==akTokenNode)
		{
			vlToken& tk =  *node->getToken();
			if(tk.ty == tokenType::tOp)
				unify = true;
		}
	}
	else if(ident==util::hashString(L"keyword"))
	{
		if(node->kind==akTokenNode)
		{
			vlToken& tk =  *node->getToken();
			if(tk.ty == tokenType::tIdent && context->isScopeKeyword(getTokenIdent()))
				unify = true;
		}
	}else if(ident==util::hashString(L"number"))
	{
		//os_t::print(L"%s", node->printOwner().c_str());
		if(node->kind==akTokenNode)
		{
			vlToken& tk =  *node->getToken();
			if(tk.ty == tokenType::tDouble || tk.ty == tokenType::tInt)
				unify = true;
		}
	}
	return unify;
}

bool vlParser::unify_apply_matchTokenItem(vlContext * context,vlAstAxis * curNode,vlToken &token,std::map<int,vlAstAxis*> & paramBind)
{
	bool unify = false;
	vlAstAxis * node = curNode; 
	if(token.ty == tokenType::tString)
	{
		if(node->Node()->getIdent()==token.inum)
		{
			unify = true;
		}
		else if(node->Node()->kind==akTokenNode)
		{
			vlToken& tk = *node->Node()->getToken();
			if(tk.ty == tokenType::tIdent && tk.inum ==token.inum)
				unify = true;
			else if(tk.ty == tokenType::tString && tk.inum ==token.inum)
				unify = true;
		}
	}
	else if(token.ty == tokenType::tInt)
	{
		if(node->Node()->kind==akTokenNode)
		{
			vlToken& tk = *node->Node()->getToken();
			if(tk.ty == tokenType::tInt && tk.inum ==token.inum)
				unify = true;
		}
	}
	else if(token.ty == tokenType::tDouble)
	{
		if(node->Node()->kind==akTokenNode)
		{
			vlToken& tk = *node->Node()->getToken();
			if(tk.ty == tokenType::tDouble && tk.dnum ==token.dnum)
				unify = true;
		}
	}
	else if(token.ty == tokenType::tBool)
	{
		if(node->Node()->kind==akTokenNode)
		{
			vlToken& tk = *node->Node()->getToken();
			if(tk.ty == tokenType::tBool && tk.dnum ==token.dnum)
				unify = true;
		}
	}
	else if(token.ty == tokenType::tChar)
	{
		wchar_t buf[8];
		::wcsncpy_s(buf,8,token.snum + 1,token.slen);
		if(node->Node()->kind == akTokenNode)
		{
			vlToken& tk = *node->Node()->getToken();
			if(tk.ty == tokenType::tOp)
			{
				if(wcscmp(opValue::getOperateText(tk.inum),buf)==0)
					unify = true;
			}else if(tk.ty == tokenType::tChar)
			{
				if(token.slen==1 && tk.slen==1 && tk.inum==token.inum) 
					unify = true;
				else 
				{
					wchar_t buf1[8];
					::wcsncpy_s(buf1,8,tk.snum + 1,tk.slen);
					if(!wcscmp(buf1,buf))unify = true;
				}
			}
		}
	}
	else if(token.ty == tokenType::tIdent)
	{
	}	
	return unify ;
}

bool vlParser::unify_apply_ensureMatchApplyParam(vlContext * context,vlApply* apply,vlAstAxis * curNode,vlAstAxis * tNode,std::map<int,vlAstAxis*> & paramBind)
{
	vlAstAxis * tnode = tNode->getChild();
	if(tnode->Node()->kind==akTokenNode)
	{
		vlToken & token = *tnode->getToken();
		return unify_apply_matchTokenItem(context,curNode,token,paramBind);
	}

	int ide = tnode->getIdent();
	//处理别名的情况
	if(ide && apply->mAlias.find(ide)!=apply->mAlias.end())
	{
		//tnode->child = apply->mAlias[ide];
		//ide = tnode->getChild()->getIdent(); 
		tnode = apply->mAlias[ide];  
		ide = tnode->getIdent(); 
	}

	else if(ide== context->apply_param)
		return unify_apply_ensureMatchApplyParam(context,apply,curNode,tnode,paramBind);
	else if(ide== context->apply_lhs)
	{
		if(tnode->Child()->LastChild()->getIdent()==curNode->getIdent())
			return true;
	}
	else if(ide==context->apply_compparam)
	{
		int index = 2;
		bool blist = false;
		if( tnode->ChildNext(2)->getInteger()==util::hashString(L"list"))
		{
			index = 3;
			blist = true;
		}
		if( tnode->ChildNext(index)->getIdent() && !tnode->ChildNext(index)->Child())
		{
			if(curNode->getIdent()==tnode->ChildNext(index)->getIdent())
			{
				return true;
			}else
			{
				if(blist && curNode->node->kind == akListNode)
				{
					const wchar_t * str = util::getHashString(tnode->ChildNext(index)->getIdent());
					if(curNode->getChild() && 
						curNode->getChild()->getIdent()==tnode->ChildNext(index)->getIdent())
					{
						return true;
					}
				}
				return false;
			}
		}
		return false;
	}else if(ide==curNode->getIdent())
		return true;
	else
		return false;
	return false;
}

bool vlParser::unify_apply_matchApplyParam(vlContext * context,vlApply* apply,vlAstAxis * curNode,vlAstAxis * tNode,MatchState & state, std::map<int,vlAstAxis*> & paramBind)
{
	//apply-param =  apply-compparam|apply-optparam|apply-lhs|apply-match-any|  identifier | '...'| literal;
	//apply-optparam = '[' { apply-param } ']';
	vlAstAxis * tnode = tNode->getChild();
	if(!curNode)
	{
		if(tnode->Node()->getIdent()==context->apply_optparam) return true;
		return false;
	}
	
	//常数项literal或skipItem
	if(tnode->Node()->kind==akTokenNode)
	{
		vlToken & token = *tnode->getToken();
		return unify_apply_matchTokenItem(context,curNode,token,paramBind);
	}
	
	int ide = tnode->getIdent();
	//处理别名的情况
	if(ide && apply->mAlias.find(ide)!=apply->mAlias.end())
	{
		//tnode->child = apply->mAlias[ide];
		//ide = tnode->getChild()->getIdent(); 
		tnode = apply->mAlias[ide];  
		ide = tnode->getIdent(); 
	}

	//参数项
	if(ide== context->apply_ellipsisparam)
	{
		state.skipItem = true;
		return true;
	}
	else if(ide== context->apply_param)
		return unify_apply_matchApplyParam(context,apply,curNode,tnode,state,paramBind);
	else if(ide==context->apply_match_any) return true;
	else if(ide==context->apply_lhs) return unify_apply_match(context,apply,curNode,tnode,paramBind);
	else if(ide==context->apply_compparam)
	{
		int index = 2;
		bool blist = false;
		if( tnode->ChildNext(2)->getInteger()==util::hashString(L"list"))
		{
			index = 3;
			blist = true;
		}
		if( tnode->ChildNext(index)->getIdent() && !tnode->ChildNext(index)->Child())
		{
			if(curNode->getIdent()==tnode->ChildNext(index)->getIdent())
			{
				paramBind.insert(std::pair<int,vlAstAxis*>(tnode->Child()->getIdent(),curNode));
				return true;
			}else
			{
				if(blist && curNode->node->kind == akListNode)
				{
					const wchar_t * str = util::getHashString(tnode->ChildNext(index)->getIdent());
					if(curNode->getChild() && 
						curNode->getChild()->getIdent()==tnode->ChildNext(index)->getIdent())
					{
						paramBind.insert(std::pair<int,vlAstAxis*>(tnode->Child()->getIdent(),curNode));
						return true;
					}
				}
				
				return false;
			}
		}else
		{
			//bug
		}
		return false;
	}
	else if(ide==context->apply_optparam)
	{
		MatchState fstate;
		fstate.reset();
		bool unify = false;
		state.optionItem = true;
		tnode = tnode->ChildNext();
		vlAstAxis * node = curNode;
		std::map<int,vlAstAxis*> params;
		if(tnode->Node()->kind!=akListNode)
		{
			std::map<int,vlAstAxis*> params;
			unify = unify_apply_matchApplyParam(context,apply,node,tnode,state,params);
			state.optionUnify = unify;
		}else
		{
			tnode = tnode->getChild();
			bool startSkip = false;
			while(tnode)
			{
				fstate.reset();
				if(!node)
				{
					if(tnode->getChild()->getIdent()!=context->apply_ellipsisparam)
					{
						unify = false;
						break;
					}
					fstate.skipItem = true;
				}
				else 
					unify = unify_apply_matchApplyParam(context,apply,node,tnode,fstate,paramBind);
				if(fstate.skipItem)
				{
					//设置skip标记
					startSkip = true;
				}
				else
				{
					//如匹配成功，则结束skip
					if(unify && startSkip) startSkip = false; 
				}
				
				//如果非skip并且一致化不成立，则退出。
				if(!startSkip && !unify && !fstate.optionItem) break;

				if(!(fstate.optionItem && !unify))
				{
					if(fstate.nextNodeAfterProcess)
						node = fstate.nextNodeAfterProcess;
					else
						if(!fstate.skipItem)node = node->getNext();
				}
				if(unify || (!unify && fstate.optionItem) )
					tnode = tnode->getSibling();
			}
			if(unify)
			{
				state.optionUnify = true;
				state.nextNodeAfterProcess = node;
			}
		}
		if(unify)
		{
			std::map<int,vlAstAxis*>::iterator iter= params.begin();
			while(iter!=params.end())
			{
				paramBind.insert(*iter);
				iter++;
			}
		}

		return unify;
	}
	else //变量项
	{
		paramBind.insert(std::pair<int,vlAstAxis*>(ide,curNode));
		return true;
	}
	return false;
}

bool vlParser::unify_apply_match(vlContext * context,vlApply* apply,vlAstAxis * curNode,vlAstAxis * applyLhs,std::map<int,vlAstAxis*> & paramBind,bool option)
{
	//apply-element [ '(' { apply-param|apply-optparam } ')' ] ;
	//apply-element = [ apply-context ] identifier;
	//apply-contextid = identifier;
	//apply-context = apply-contextid ':';

	bool unify = false;
	
	//以下部分处理apply-element
	int ident = applyLhs->Child()->LastChild()->getIdent();
	//处理apply-element包含上下文(apply-context)的情况
	if(applyLhs->Child()->ChildNext(1) && applyLhs->Child()->ChildNext(1)->getIdent())
		ident = applyLhs->Child()->ChildNext(1)->getIdent();

	//当前待处理的节点非元素型节点的情况
	//判断是否与特殊符号项模板相匹配。
	if(curNode->getIdent()!=ident)
	{
		int ide = applyLhs->Child()->LastChild()->getIdent();
		unify = unify_apply_matchSpecialItem(context,curNode,ident,paramBind);
		if(!unify) return false ;
	}else if(ident != curNode->getIdent()) 
		return false;

	//以下部分处理参数
	//数据node
	vlAstAxis * node = curNode->getChild();
	//模板tnode
	vlAstAxis * tnode = applyLhs->ChildNext(2); //skip element and '('
	if(!tnode) return node ? false : true;

	MatchState fstate;
	//单项apply_element
	if(tnode->Node()->kind!=akListNode)
	{
		if(!tnode->getChild()) return true;//no param
		return unify_apply_matchApplyParam(context,apply,node,tnode,fstate,paramBind);
	}
	
	//模板是重复项
	assert(tnode->node->kind==akListNode);
	tnode = tnode->getChild();
	bool startSkip = false;
	while(tnode)
	{
		fstate.reset();
		if(!node)
		{
			if(tnode->getChild()->getIdent()!=context->apply_ellipsisparam)
			{
				unify = false; 
				break;
			}
			fstate.skipItem = true;
		}
		else
		{
			if(tnode->getChild()->getIdent()==context->apply_optparam)
			{
				vlAstAxis * anode = tnode;
				unify = false;
				while(anode)
				{
					unify = unify_apply_ensureMatchApplyParam(context,apply,node,anode,paramBind);
					if(unify)break;
					if(anode->getChild()->getIdent()!=context->apply_optparam)break;
					anode = anode->getSibling(); 
				}
				if(unify)tnode = anode;
				unify = unify_apply_matchApplyParam(context,apply,node,tnode,fstate,paramBind);
			}else
				unify = unify_apply_matchApplyParam(context,apply,node,tnode,fstate,paramBind);
		}
		if(fstate.skipItem)
		{
			//设置skip标记
			startSkip = true;
		}
		else
		{
			//如匹配成功，则结束skip
			if(unify && startSkip) startSkip = false; 
		}
		
		//如果非skip并且一致化不成立，则退出。
		if(!startSkip && !unify && !fstate.optionItem) break;

		if(!(fstate.optionItem && !unify))
		{
			if(fstate.nextNodeAfterProcess)
				node = fstate.nextNodeAfterProcess;
			else
				if(!fstate.skipItem)node = node->getNext();
		}
		if(unify || (!unify && fstate.optionItem))
			tnode = tnode->getSibling();
		if(!node)break;
	}
	if(!node)
	{
		if(!tnode) return startSkip? true: unify;
		
		while(tnode->Child() && (context->apply_optparam==tnode->Child()->getIdent() ||
			context->apply_ellipsisparam==tnode->Child()->getIdent()))
			tnode = tnode->getSibling();
		if(!tnode)
			return startSkip? true: unify;
		else
		{
			wstring str = tnode->printOwner();
			return false;
		}
	}else
	{
		while(tnode->Child() && (context->apply_optparam==tnode->Child()->getIdent() ||
			context->apply_ellipsisparam==tnode->Child()->getIdent()))
			tnode = tnode->getSibling();
		if(!tnode) return startSkip? true: false;
	}
	return unify;
}

void vlParser::ReleaseParser(vlParser*  parser)
{
	parser->getLexer()->source = 0;
	delete parser;
}

vlParser * vlParser::makeParser(vlContext * context)
{
	vlParser * parser = new vlParser;
	parser->set_err_ptr(((xlang::wpp_context*)context->wppcontext)->get_error());
	return parser;	
}

vlAstAxis * vlParser::ParserUrl(vlContext * context,vlParser* parser,wchar_t* ruler,wchar_t * source)
{
	const wchar_t * pUrl = source;
	wchar_t* pstr =  context->wppcontext->get_runtime()->GetSource((wchar_t*)pUrl);
	//context->wppcontext->fetch_file(pUrl);
	//nextToken();
	if(pstr)
	{
		parser->lexer->setSource(pstr,0);
		parser->lexer->nextch();
		parser->nextToken();
	}else
		return 0;

	while(parser->matchKeyword(parser->getToken(),L"use")) 
	{
		parser->nextToken();
		if(parser->matchString(parser->getToken()))
		{
			const wchar_t * pUrl = parser->getTokenString(parser->getToken());
			wchar_t* pstr = context->wppcontext->fetch_file(pUrl);
			//nextToken();
			if(pstr)
			{
				//切换lexer
				vlLexer * ptr_lexer = new vlLexer;
				ptr_lexer->setSource(pstr,(wchar_t*)pUrl);
				parser->ptr_lexer_pool->push(parser->lexer);
				parser->lexer = ptr_lexer;
				parser->lexer->nextch();
				parser->nextToken();
			}else
				parser->nextToken();
		}else
		{
			parser->error(error::err_needstring,0,parser->getToken());
			//skipLine();
			return 0;
		}
		//parser->nextToken();
	}
	
	vlAstAxis * node = 0;
	if(!ruler || !ruler[0])
		parser->unify_term(context,context->primaryRuler);
	else
	{
		if(context->rulers.find(util::hashString(ruler))!=context->rulers.end())
			node = parser->unify_term(context,context->rulers[util::hashString(ruler)]);
		else
			parser->format_output(L"\n未找到规则%s.",ruler);
	}
	return node; 
}

VLANG_API  intptr_t vlParser::MakeClass(vlContext * context,const wchar_t* urlClass)
{

	int cf = context->wppcontext->get_runtime()->load_class(context->wppcontext,urlClass);
	return context->wppcontext->get_runtime()->create_class(context->wppcontext,cf);
}

VLANG_API void vlParser::Output(vlContext * context,const wchar_t* str)
{
	context->wppcontext->get_error()->output(0,str);
}

VLANG_API void vlParser::Print(vlContext * context,const wchar_t* format,const wchar_t* str)
{
	context->wppcontext->get_error()->print((wchar_t*)format,str);
}

vlAstAxis * vlParser::Parser(vlContext * context,vlParser* parser, wchar_t* ruler, wchar_t * source)
{

	parser->lexer->setSource(source,0);
	parser->lexer->nextch();
	parser->nextToken();


	vlAstAxis * node = 0;
	if(!ruler || !ruler[0])
		parser->unify_term(context,context->primaryRuler);
	else
	{
		if(context->rulers.find(util::hashString(ruler))!=context->rulers.end())
			node = parser->unify_term(context,context->rulers[util::hashString(ruler)]);
		else
			parser->format_output(L"\n未找到规则%s.",ruler);
	}
	return node;  
}

vlAstAxis * vlParser::Process(vlContext * context, vlParser* parser, wchar_t* processor, vlAstAxis * data)
{
	if(!data) return 0;

	vlApply * apply = context->applysvr.getApply(util::hashString(processor));
	if(!apply)
	{
		parser->format_output(L"\n未找到处理器%s!", processor); 
		return 0;
	}
	return parser->unify_apply(context,apply,data);
}

wchar_t * vlParser::NodeToString(vlAstAxis * axis)
{
	if(!axis) return 0;
	return wcsdup(axis->printOwner().c_str());  
}
