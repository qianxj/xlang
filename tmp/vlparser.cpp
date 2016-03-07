#include "vlParser.hpp"
#include "vlUtils.hpp"
#include "vlPlatform.hpp"
#include "vlAstNode.hpp"
#include "vlAstAxis.hpp"
#include "vlAstFactory.hpp"
#include "vlsynnoderef.hpp"
#include <vector>

//using namespace vord;
using namespace xlang;

vlAstAxis * vlParser::parse_bnf_expression(vlContext * context)
{
	vector<vlAstAxis*> items; 
	vector<int> indexs;//for oneof
	while(true)
	{
		if(matchOperate(opValue::lparen))
		{
			nextToken();
			if(!matchOperate(opValue::rparen))
			{
				items.push_back(context->getAstFactory()->CreateAxis(parse_bnf_expression(context)));
				if(!ensurseOperator(opValue::rparen,error::err_needrightparen)) return 0;
			}
			nextToken();
		}
		else if(matchOperate(opValue::lbrackets))
		{
			nextToken();
			if(!matchOperate(opValue::rbrackets))
			{
				items.push_back(context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstNode(akBnfOption)));
				items.back()->setChild(parse_bnf_expression(context));
				if(!ensurseOperator(opValue::rbrackets,error::err_needrightbracket)) return 0;
			}
			nextToken();
		}
		else if(matchOperate(opValue::lbrace))
		{
			nextToken();
			if(!matchOperate(opValue::rbrace))
			{
				items.push_back(context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstNode(akBnfRepeat)));
				items.back()->setChild(parse_bnf_expression(context));
				if(!ensurseOperator(opValue::rbrace,error::err_needrightbrace)) return 0;
			}
			nextToken();
		}			
		else if(getTokenIdent()==util::hashString(L"L"))
		{
			nextToken();
			if(matchOperate(opValue::lparen))
			{
				nextToken();
				items.push_back(context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstNode(akBnfList)));
				items.back()->setChild(context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAxis(parse_bnf_expression(context))));
				if(!ensurseOperator(opValue::comma,error::err_needcomma)) return 0;
				nextToken();
				if(!matchChar())
				{
					error(error::err_needoperator,L"",getToken());
					return 0;
				}
				items.back()->getChild()->setNext(context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstToken(akBnfTerminal,getToken())));
				nextToken();
				if(!ensurseOperator(opValue::rparen,error::err_needrightparen)) return 0;
			}
			else
				items.push_back(context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstIdentifier(akBnfIdentifier,getTokenIdent())));
			nextToken();
		}
		else 
		{
			if(matchIdent()) 
			{
				
				items.push_back(context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstIdentifier(akBnfIdentifier,getTokenIdent())));
			}
			else if(matchString()) //keyword or some operate
			{
				context->addKeyword(getTokenIdent());
				items.push_back(context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstToken(akBnfTerminal,getToken())));
			}
			else if(matchChar()) //operate
			{
				items.push_back(context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstToken(akBnfTerminal,getToken())));
			}else
			{
				if(matchOperate(opValue::semicolon))
					nextToken();
				break;
			}
			nextToken();
		}
		if(matchOperate(opValue::bor))
		{
			nextToken();
			indexs.push_back((int)items.size());
		}
	}

	if(items.size() == 0) return 0;
	if(indexs.size()== 0)
	{
		for(int i=0;i<(int)items.size() - 1;i++)
		{
			items[i]->setNext(items[i+1]);
		}
		return items[0];
	}

	vlAstAxis * node = context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstNode(akBnfOneof));
	indexs.push_back((int)items.size());
	for(int index=0;index<=(int)indexs.size() - 1;index++)
	{
		int start = index==0 ? 0 : indexs[index - 1];
		int last  = indexs[index] - 1;
		for(int it = start;it <=last - 1;it++)
		{
			items[it]->setNext(items[it+1]);
		}
		if(index<(int)indexs.size() - 1)
			items[start]->setSibling(items[last + 1]);
	}
	node->setChild(items[0]); 

	return node;
}

int vlParser::parse_bnf(vlContext * context,map<int ,vlAstAxis*> & rulers)
{
	while(true)
	{
		if(!matchIdent())break;
		int ide = getTokenIdent();
		getLexer()->save();
		nextToken();
		if(!matchOperate(opValue::asn))
		{
			getLexer()->restore();
			break;
		}
		getLexer()->pop();
		nextToken();

		vlAstAxis* node = parse_bnf_expression(context);
		if(node)
		{
			if(!context->primaryRuler) context->primaryRuler = node;

			if(rulers.find(ide)==rulers.end())
				rulers[ide] = node;
			else
			{
				//并入ruler
				if(rulers[ide]->kind==akBnfOneof && !rulers[ide]->getNext())
				{
					vlAstAxis * item = rulers[ide]->getChild();
					while(item->getSibling())item=item->getSibling();
					item->setSibling(node);
				}else
				{
					//插入nodeoneof
					vlAstAxis * oneof = context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstNode(akBnfOneof));
					oneof->setChild(rulers[ide]);
					rulers[ide]->setSibling(node);
					rulers[ide] = oneof;
				}
			}
		}
		while(matchOperate(opValue::semicolon))
					nextToken();
	}
	return 1;
}

int vlParser::parse_xlang_translation_unit(vlContext * context)
{
	while(true)
	{
		int ret = 0;
		/*if(matchKeyword(L"model"))
		{
			ret = parse_model_define(parser);
		}
		else*/ if(matchKeyword(L"lang"))
		{
			ret = parse_lang_define(context);
		}

		else if(matchKeyword(getToken(),L"use")) 
		{
			nextToken();
			if(matchString(getToken()))
			{
				const wchar_t * pUrl = getTokenString(getToken());
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
				}else
					nextToken();
				ret = 1;
			}else
			{
				error(error::err_needstring,0,getToken());
				//skipLine();
				return -1;
			}
		}
		else
		{
			int cc = this->lexer->token.cc;
			ret = parser_unify_term(context);
			if(cc == this->lexer->token.cc)
			{
				this->error(error::err_unknownstmt,L"",getToken());
				this->skipLine();
			}
		}
		if(matchEof())break;

		if(ret!=1)
		{
			error(error::err_unknownstmt,L"",getToken());
			skipLine();
		}
	}
	return 1;
}

int vlParser::parse_lang_define(vlContext * context)
{
	if(!matchKeyword(L"lang")) return 0;
	nextToken();

	if(matchIdent()) nextToken();
	if(matchOperate(opValue::lbrackets))
	{
		nextToken();

		while(true)
		{
			if(matchKeyword(L"syntax"))
			{
				nextToken();
				if(matchIdent())
				{
					context->langterm = getTokenIdent();
					nextToken();
				}
				if(!ensurseOperator(opValue::lbrackets,error::err_needleftbracket))return -1;
				nextToken();
				parse_bnf(context,context->rulers);
				if(!ensurseOperator(opValue::rbrackets,error::err_needrightbracket))return -1;
				nextToken();
				context->langterm = 0;
			}
			else if(matchKeyword(L"decision"))
			{
				nextToken();
				if(matchIdent()) nextToken();
				if(!ensurseOperator(opValue::lbrackets,error::err_needleftbracket))return -1;
				nextToken();
				parse_bnf(context,context->surerulers);
				if(!ensurseOperator(opValue::rbrackets,error::err_needrightbracket))return -1;
				nextToken();
			}
			else if(matchKeyword(L"ensure"))
			{
				nextToken();
				if(matchIdent()) nextToken();
				if(!ensurseOperator(opValue::lbrackets,error::err_needleftbracket))return -1;
				nextToken();
				parse_bnf(context,context->surerulers);
				if(!ensurseOperator(opValue::rbrackets,error::err_needrightbracket))return -1;
				nextToken();
			}else
				break;
		}
	}
	if(!ensurseOperator(opValue::rbrackets,error::err_needrightbracket))return -1;
	nextToken();

	return 1;
}

bool vlParser::ensure_unify_term(vlContext * context, vlAstAxis * anode,bool option)
{
	bool match = false;
	while(anode)
	{
		match = false;
		if(matchEof())break;
		//keyword、val、operator
		else if(anode->node->kind==akBnfTerminal)
		{
			vlAstToken * terminal = (vlAstToken*)anode->node;
			//match keyword
			if(matchString(*terminal->getToken()) )
			{
				if(matchIdent() && terminal->getToken()->inum == getTokenIdent())
					match = true;
			}
			//match operator
			else if(matchChar(*terminal->getToken()))
			{
				if(matchOperate(getToken()))
				{
					wchar_t buf[8];
					if(terminal->getToken()->slen > 1)
					{
						::wcsncpy_s(buf,8,terminal->getToken()->snum + 1,terminal->getToken()->slen);
						if(matchOperate(opValue::getOperateByString(buf))) match = true;
					}else
					{
						if(matchOperate(opValue::getOperateByToken(terminal->getToken()->inum)))
							match = true;
					}
				}
			}
			if(match) nextToken();
			anode = anode->getNext();
		}
		else if(anode->node->kind==akBnfIdentifier)
		{
			//match string-literal
			//match integer-literal
			//match double-literal
			//match char-literal
			//match bool-literal
			vlAstIdentifier* identifier = (vlAstIdentifier*)anode->node;
			if( identifier->ident == context->literal &&
				( matchString()||
				  matchNumber() ||
				  matchChar() ||
				  matchBool()) ||
				  (identifier->ident == context->string_literal && matchString())||
				  (identifier->ident == context->integer_literal && matchInteger())||
				  (identifier->ident == context->char_literal && matchChar())||
				  (identifier->ident == context->double_literal && matchNumber()) ||
				  (identifier->ident == context->bool_literal && matchBool()))
			{//literal
				match = true;
				nextToken();
			}
			else if(identifier->ident == context->identifier)
			{
				if(matchIdent() && !context->isScopeKeyword(getTokenIdent()))
				{
					match = true;
					nextToken();
				}
			}
			else 
			{//lhs
				//find ruler to match
				if(context->rulers.find(identifier->ident)!=context->rulers.end())
				{
					const wstring_t  ruler = util::getHashString(identifier->ident);
					getLexer()->save();
					if(ensure_unify_term(context,context->rulers[identifier->ident],option))
					{
						match = true;
						getLexer()->pop();
					}else
						getLexer()->restore();
				}
			}
			anode = anode->getNext();
		}
		else
		{
			//axisnode
			if(anode->node->kind==akAxisNode)
			{
				vlAstAxis * item = (vlAstAxis *)anode->node;
				if(ensure_unify_term(context,item,option))
					match = true;
				anode = anode->getNext();
			}
			//复合项
			else if(anode->node->kind==akBnfOneof)
			{
				vlAstAxis * item = anode->getChild();
				while(item)
				{
					if(ensure_unify_term(context,item,true))break;
					item = item->getSibling();
				}
				if(item) match = true;
				anode = anode->getNext();
			}
			else if(anode->node->kind==akBnfOption)
			{
				vlAstAxis * item = anode->getChild();
				ensure_unify_term(context,item,true);
				match = true;
				anode = anode->getNext();
			}
			else if(anode->node->kind==akBnfRepeat)
			{
				vlAstAxis * item = anode->getChild();
				while(true)
				{
					if(!ensure_unify_term(context,item,true))break;
					match = true;
				}
				anode = anode->getNext();
				match = true; //repeat can skip
			}else
			{
				//not reach here
				assert(false);
				break;
			}
		}
		if(!match)break;
	}
	return match;
}

int vlParser::parser_unify_term(vlContext * context)
{
	vlAstAxis * node = unify_term(context,context->primaryRuler);

	if(node && context->bTraceAst)
	{
		format_output(L"\n\n---- entity ast tree ------");
		format_output(L"\n%s",node->printOwner().c_str());
	}

	return 1;
}

bool vlParser::keywordCanIdentifier(vlContext * context, vlAstAxis * anode,vlToken & tk)
{
	return false;
}

vlAstAxis *  vlParser::unify_term(vlContext * context, vlAstAxis * anode, bool option,bool trydo)
{
	int matchCount = 0;
	int missCount = 0;
	int count = 0;

	int missKind = MissKind::none;
	int matchKind = MatchKind::none;
	struct NodeList
	{
		vlAstAxis * head;
		vlAstAxis * tail;
		vlContext * context;
		NodeList(vlContext * context):head(0),tail(0),context(context){}

		void push(vlAstAxis * node)
		{
			if(tail)
			{
				assert(!tail->getNext());
				/*if(node->getNext())
				{
					vlAstAxis * anode =	context->getAstFactory()->CreateAxis(node);
					node = anode;
				}*/
				tail->setNext(node,false);
				tail = node;
			}else
			{
				head = node;
				tail = node;
			}
			while(tail->getNext())tail = tail->getNext(); //parent error
		}
	}nodelist(context);

	wchar_t missItem[255];
	while(anode)
	{
		
		missKind = MissKind::none;
		matchKind = MatchKind::none;

		if(matchEof())break;

		missItem[0] = 0;
		//keyword、val、operator
		if(anode->node->kind==akBnfTerminal)
		{
			vlAstToken* terminal = (vlAstToken*)anode->node;
			
			//match keyword
			if(matchString(*terminal->getToken()))
			{
				missKind = MissKind::keyword;
				if(matchIdent() && terminal->getToken()->inum == getTokenIdent() && context->isScopeKeyword(getTokenIdent()))
				{
					matchKind = MatchKind::keyword;
					context->getAstFactory()->CreateNode(terminal->getToken()->snum);
					nodelist.push(context->getAstFactory()->CreateAxis(
						context->getAstFactory()->CreateAstToken(getToken())
					));
				}
				//record miss
				if(matchKind == MatchKind::none)
					wcscpy_s(missItem,255,getTokenString(*terminal->getToken()));
			}

			//match operator
			else if(matchChar(*terminal->getToken()))
			{
				missKind = MissKind::operate;
				if(matchOperate(getToken()))
				{
					bool matchop = false;
					if(terminal->getToken()->slen > 1)
					{
						wchar_t buf[8];
						::wcsncpy_s(buf,8,terminal->getToken()->snum + 1,terminal->getToken()->slen);
						if(matchOperate(opValue::getOperateByString(buf))) matchop = true;
					}
					else if(matchOperate(opValue::getOperateByToken(terminal->getToken()->inum)))  matchop = true;
					if(matchop)
					{
						matchKind = MatchKind::operate;
						context->getAstFactory()->CreateNode(L" operate");
						nodelist.push(context->getAstFactory()->CreateAxis(
							context->getAstFactory()->CreateAstToken(getToken())
						));
					}
				}
				//record miss
				if(matchKind == MatchKind::none)
					wcsncpy_s(missItem,255,terminal->getToken()->snum + 1,terminal->getToken()->slen);

			}
			if(matchKind != MatchKind::none) nextToken();
			anode = anode->getNext();
		}
		else if(anode->node->kind == akBnfIdentifier)
		{
			//match string-literal
			//match integer-literal
			//match double-literal
			//match char-literal
			//match bool-literal
			vlAstIdentifier* identifier = (vlAstIdentifier*)anode->node;
			missKind = MissKind::literal;
			if( identifier->ident == context->literal ||
				identifier->ident == context->string_literal ||
				identifier->ident == context->integer_literal||
				identifier->ident == context->char_literal	 ||
				identifier->ident == context->double_literal ||
				identifier->ident == context->bool_literal)
			{
				if( identifier->ident == context->literal &&
					( matchString()||
					  matchNumber() ||
					  matchChar() ||
					  matchBool()) ||
					  (identifier->ident == context->string_literal && matchString())||
					  (identifier->ident == context->integer_literal && matchInteger())||
					  (identifier->ident == context->char_literal && matchChar())||
					  (identifier->ident == context->double_literal && matchNumber()) ||
					  (identifier->ident == context->bool_literal && matchBool()))
				{//literal
					matchKind = MatchKind::literal;
					context->getAstFactory()->CreateNode(L" literal");
					nodelist.push(context->getAstFactory()->CreateAxis(
						context->getAstFactory()->CreateAstToken(getToken())
					));
					nextToken();
				}else
				{
					wcscpy_s(missItem,255,util::getHashString(identifier->ident));
				}
			}
			else if(identifier->ident == context->identifier)
			{
				missKind = MissKind::identifier;
				if(matchIdent())
				{
					bool bIdentifier = true;
					if(context->isScopeKeyword(getTokenIdent()))
					{
						bIdentifier = keywordCanIdentifier(context,anode,getToken());
					}
					if(bIdentifier)
					{
						matchKind = MatchKind::identifier;
						context->getAstFactory()->CreateNode(L" identifier");
						nodelist.push(context->getAstFactory()->CreateAxis(
							context->getAstFactory()->CreateAstIdentifier(getTokenIdent(),getToken())
						));
					}
				}
				if(matchKind != MatchKind::none)
					nextToken();
				else
					wcscpy_s(missItem,255,L" identifier");
			}
			else 
			{//lhs
				//find ruler to match
				bool match = true;
				if(context->surerulers.find(identifier->ident)!=context->surerulers.end())
				{
					 match = false;
					const wstring_t  ruler = util::getHashString(identifier->ident);
					getLexer()->save();
					if(ensure_unify_term(context,context->surerulers[identifier->ident],false))match=true;
					getLexer()->restore();
				}

				missKind = MissKind::ruler;
				if(match && context->rulers.find(identifier->ident)!=context->rulers.end())
				{
					const wstring_t  ruler = util::getHashString(identifier->ident);
					getLexer()->save();

					vlAstAxis * node = unify_term(context,context->rulers[identifier->ident],option,trydo);
					if(node)
					{
						matchKind = MatchKind::ruler;
						wchar_t buf[255];
						swprintf_s(buf,255,L" %s",util::getHashString(identifier->ident));
						context->getAstFactory()->CreateNode(buf);
						
						vlAstAxis * rnode = context->getAstFactory()->CreateAxis(
							context->getAstFactory()->CreateAstIdentifier(akBnfIdentifier,identifier->ident,getToken())
						);
						rnode->setChild(node);
						nodelist.push(rnode);

						getLexer()->pop();
					}else
					{
						getLexer()->restore();
					}
				}
				if(matchKind != MatchKind::none)
					wcscpy_s(missItem,255,util::getHashString(identifier->ident));
			}
			anode = anode->getNext();
		}
		else
		{
			//axisnode
			if(anode->node->kind==akAxisNode)
			{
				vlAstAxis * item = (vlAstAxis *)anode->node;
				missKind = MissKind::axisnode;
				vlAstAxis * node = unify_term(context,item,option,trydo);
				if(node)
				{
					matchKind = MatchKind::axisnode;
					nodelist.push(node);
				}
				anode = anode->getNext();
			}
			//复合项
			else if(anode->node->kind==akBnfOneof)
			{
				if(!anode->getChild())break; //error
				vlAstAxis * item = anode->getChild();
				vlAstAxis * node = 0;
				while(item)
				{
					node = unify_term(context,item,true,trydo);
					if(node)break;
					item = item->getSibling();
				}
				missKind = MissKind::oneof;
				if(item)
				{
					matchKind = MatchKind::oneof;
					if(node) nodelist.push(node);
				}
				anode = anode->getNext();
			}
			else if(anode->node->kind==akBnfOption)
			{
				if(!anode->getChild())break; //error
				vlAstAxis * item = anode->getChild();
				missKind = MissKind::none;
				vlAstAxis * node = unify_term(context,item,true,trydo);
				if(node)
				{
					matchKind = MatchKind::option;
					nodelist.push(node);
				}
				anode = anode->getNext();
			}
			else if(anode->node->kind==akBnfRepeat)
			{
				if(!anode->getChild())break; //error
				vlAstAxis * item = anode->getChild();
				//missKind = MissKind::repeat;
				missKind = MissKind::none;

				int items = 0;
				vlAstAxis* head = 0;
				vlAstAxis* tail = 0;
				while(true)
				{
					vlAstAxis * node = unify_term(context,item,true,trydo);
					if(!node)break;
					matchKind = MatchKind::repeat;
					if(node)
					{
						items++;
						if(items==1)
						{
							head = node;
							tail = node;
						}
						else
						{
							assert(!tail->getSibling());
							tail->setSibling(node);
							tail = node;
						}
					}
				}
				if(items >= 1 && head)
				{
					vlAstAxis * rnode = context->getAstFactory()->CreateAxis(
						context->getAstFactory()->CreateAstNode(akListNode)
					);
					rnode->setChild(head);
					nodelist.push(rnode);
				}

				anode = anode->getNext();
			}else if(anode->node->kind==akBnfList)
			{//处理列表项 et L(xxx,',')
				if(!anode->getChild())break; //error
				vlAstAxis * item = (vlAstAxis *)anode->getChild()->node;
				vlAstAxis * tk = anode->getChild()->getNext();
				//missKind = MissKind::repeat;
				missKind = MissKind::none;

				int items = 0;
				vlAstAxis* head = 0;
				vlAstAxis* tail = 0;
				while(true)
				{
					vlAstAxis * node = unify_term(context,item,true,trydo);
					if(!node)break;
					matchKind = MatchKind::repeat;
					if(node)
					{
						items++;
						if(items==1)
						{
							head = node;
							tail = node;
						}
						else
						{
							assert(!tail->getSibling());
							tail->setSibling(node);
							tail = node;
						}
					}
					//判断分割符号是否匹配
					node = unify_term(context,tk,true,trydo);
					if(!node || node->kind== akMissToken)break;
				}
				if(items >= 1 && head)
				{
					vlAstAxis * rnode = context->getAstFactory()->CreateAxis(
						context->getAstFactory()->CreateAstNode(akListNode)
					);
					rnode->setChild(head);
					nodelist.push(rnode);
				}

				anode = anode->getNext();
			}else
			{
				//not reach here
				assert(false);
				break;
			}
		}
		if(matchKind != MatchKind::none)
			matchCount++;
		if(option && matchCount < 1 && missKind != MissKind::none) return 0;
		if(missKind != MissKind::none && matchKind == MatchKind::none)
		{
			nodelist.push(context->getAstFactory()->CreateAxis(
				context->getAstFactory()->CreateAstMissToken(missKind,getToken())
			));
			switch(missKind)
			{
			case MissKind::keyword :
				error(error::err_needkeyword,missItem,getToken());
				context->getAstFactory()->CreateNode(L" miss-keyword");
				break;
			case MissKind::operate:
				error(error::err_needoperator,missItem,getToken());
				context->getAstFactory()->CreateNode(L" miss-operate");
				break;
			case MissKind::literal:
				error(error::err_needToken,missItem,getToken());
				context->getAstFactory()->CreateNode(L" miss-literal");
				break;
			case MissKind::ruler:
				context->getAstFactory()->CreateNode(L" miss-ruler");
				error(error::err_needToken,missItem,getToken());
				break;
			case MissKind::oneof:
				context->getAstFactory()->CreateNode(L" miss-oneof");
				break;
			case MissKind::option:
				context->getAstFactory()->CreateNode(L" miss-option");
				break;
			case MissKind::repeat:
				context->getAstFactory()->CreateNode(L" miss-repeat");
				break;
			case MissKind::identifier:
				error(error::err_needident,missItem,getToken());
				context->getAstFactory()->CreateNode(L" miss-identifier");
				break;
			}
			missCount++;
		}
	}
	
	vlAstAxis * fnode = nodelist.head;
	while(fnode)
	{
		if(fnode && matchKind != MatchKind::oneof && matchKind != MatchKind::option &&
			matchKind != MatchKind::repeat )
		{
			firePrioritySort(context,fnode);
			fireConstructor(context,fnode);
			fireNode(context,fnode);
			firePrint(context,fnode);
			fireTrace(context,fnode);
			fireGener(context,fnode);
		}
		fnode = fnode->getNext();
	}
	return nodelist.head;//matchCount + missCount;
}

int vlParser::printAstAxis(vlAstAxis * anode)
{
	while(anode)
	{
		//keyword、val、operator
		if(anode->node->kind==akBnfTerminal)
		{
			vlAstToken* terminal = (vlAstToken*)anode->node;
			
			//match keyword
			if(matchString(*terminal->getToken()))
			{
				format_output(L" \"%s\"",terminal->getToken()->snum);
			}

			//match operator
			else if(matchChar(*terminal->getToken()))
			{
				format_output(L" '%c'",(wchar_t)terminal->getToken()->inum);
			}
			anode = anode->getNext();
		}
		else if(anode->node->kind==akBnfIdentifier)
		{
			vlAstIdentifier* identifier = (vlAstIdentifier*)anode->node;
			format_output(L" %s",util::getHashString(identifier->ident));
			anode = anode->getNext();
		}
		else
		{
			//axisnode
			if(anode->node->kind==akAxisNode)
			{
				printAstAxis((vlAstAxis*)anode->node);
				anode = anode->getNext();
			}
			//复合项
			else if(anode->node->kind==akBnfOneof)
			{
				vlAstAxis * item = anode->getChild();
				format_output(L" (");
				printAstAxis(item);
				item = item->getSibling();
				while(item)
				{
					format_output(L"|");
					printAstAxis(item);
					item = item->getSibling();
				}
				format_output(L")");
				anode = anode->getNext();
			}
			else if(anode->node->kind==akBnfOption)
			{
				format_output(L" [");
				printAstAxis(anode->getChild());
				format_output(L"]");
				anode = anode->getNext();
			}
			else if(anode->node->kind==akBnfRepeat)
			{
				format_output(L" {");
				printAstAxis(anode->getChild());
				format_output(L"}");
				anode = anode->getNext();
			}else if(anode->node->kind==akBnfList)
			{
				format_output(L" L(");
				printAstAxis((vlAstAxis *)anode->getChild()->node);
				format_output(L",");
				printAstAxis(anode->getChild()->getNext());
				format_output(L")");
				anode = anode->getNext();
			}else
			{
				//not reach here
				assert(false);
				break;
			}
		}
	}
	return 1;
}
