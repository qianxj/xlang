#include "vlpreparse.hpp"

using namespace trap::complier;
TNode* parse_unit(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_context(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_vord(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_lang(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_with(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_constructor(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_with_unit_syntax(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_with_term_syntax(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_bnf_rhs(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_bnf_term(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_term_syntax_define(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_term_sure_define(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_term_param_syntax(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_with_body_syntax(Context &ctx,TParser* parser, TToken &tk);

TNode * parse_unit(Context &ctx,TParser* parser, TToken &tk)
{
	TNodeList* $result = parser->makeList();
	if(parser->isKeyword(tk,L"context"))
	{
		return parse_context(ctx,parser,tk);

	}
	else if(parser->isKeyword(tk,L"vordx") || parser->isKeyword(tk,L"vord") || parser->isKeyword(tk,L"voc"))
	{
		return parse_vord(ctx,parser,tk);

	}
	else if(parser->isKeyword(tk,L"constructor"))
	{
		return parse_constructor(ctx,parser,tk);

	}
	else if(parser->isKeyword(tk,L"lang"))
	{
		return parse_lang(ctx,parser,tk);

	}
	else if(parser->isKeyword(tk,L"with"))
	{
		return parse_with(ctx,parser,tk);

	}
	parser->skipLine();
	return $result;

}

TNode * parse_context(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;	
	Ensure(parser->isKeyword(tk,L"context"),9,L"context");
	TTokenNode* $kind = parser->TokenKeyword(tk);
	parser->nextToken(tk);
	Ensure(parser->isIdent(tk),6,0);
	TTokenNode* $name = parser->TokenNode(tk);
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	$result = makeLangTerm(ctx,$kind,$name);
	return $result;

}

TNode * parse_vord(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;	
	TTokenNode* $kind = 0 ;
	if(parser->isKeyword(tk,L"vordx"))
	{	
		
		Ensure(parser->isKeyword(tk,L"vordx"),9,L"vordx");
		parser->nextToken(tk);
	}
	if(parser->isKeyword(tk,L"vord"))
	{		
		Ensure(parser->isKeyword(tk,L"vord"),9,L"vord");
		$kind = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"voc"))
	{		
		Ensure(parser->isKeyword(tk,L"voc"),9,L"voc");
		$kind = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	Ensure(parser->isIdent(tk),6,0);
	TTokenNode* $name = parser->TokenNode(tk);
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	$result = makeLangTerm(ctx,$kind,$name);
	return $result;

}

TNode * parse_lang(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;	
	Ensure(parser->isKeyword(tk,L"lang"),9,L"lang");
	TTokenNode* $kind = parser->TokenKeyword(tk);
	parser->nextToken(tk);
	Ensure(parser->isIdent(tk),6,0);
	TTokenNode* $name = parser->TokenNode(tk);
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	$result = makeLangTerm(ctx,$kind,$name);
	return $result;

}

TNode * parse_with(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isKeyword(tk,L"with"),9,L"with");
	parser->nextToken(tk);
	Ensure(parser->isIdent(tk),6,0);
	parser->nextToken(tk);
	$result = parse_with_body_syntax(ctx,parser,tk);
	Ensure(parser->isKeyword(tk,L"end"),9,L"end");
	parser->nextToken(tk);
	Ensure(parser->isKeyword(tk,L"with"),9,L"with");
	parser->nextToken(tk);
	return $result;

}

TNode * parse_constructor(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;	
	Ensure(parser->isKeyword(tk,L"constructor"),9,L"constructor");
	TTokenNode* $kind = parser->TokenKeyword(tk);
	parser->nextToken(tk);
	Ensure(parser->isIdent(tk),6,0);
	TTokenNode* $name = parser->TokenNode(tk);
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	$result = makeLangTerm(ctx,$kind,$name);
	return $result;

}

TNode * parse_with_unit_syntax(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isKeyword(tk,L"unit"),9,L"unit");
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,':'),90,0);
	parser->nextToken(tk);
	do
	{		
		if(parser->isKeyword(tk,L"list"))
		{		
			
			Ensure(parser->isKeyword(tk,L"list"),9,L"list");
			parser->nextToken(tk);
		}
		Ensure(parser->isIdent(tk),6,0);
		parser->nextToken(tk);
		if(!(parser->isOperator(tk,',')))
		{		

			break;
		}
		parser->nextToken(tk);
	}while(true);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	return $result;

}

TNode * parse_with_term_syntax(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isIdent(tk) && parser->isKeyword(parser->pickToken(),L"syntax"),6,0);
	TTokenNode* $id = parser->TokenNode(tk);
	parser->nextToken(tk);
	Ensure(parser->isKeyword(tk,L"syntax"),9,L"syntax");
	TTokenNode* $kind = parser->TokenKeyword(tk);
	parser->nextToken(tk);
	Ensure(parser->isIdent(tk),6,0);
	TTokenNode* $name = parser->TokenNode(tk);
	parser->nextToken(tk);
	$result = makeSyntax(ctx,$id,$name);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	return $result;

}

TNode * parse_bnf_rhs(Context &ctx,TParser* parser, TToken &tk)
{	
	TNodeList* $result = parser->makeList();
	if(parser->isKeyword(tk,L"var"))
	{	
		
		Ensure(parser->isKeyword(tk,L"var"),9,L"var");
		parser->nextToken(tk);
		do
		{			
			TNode* $isList = 0 ;
			if(parser->isKeyword(tk,L"list"))
			{			
				
				Ensure(parser->isKeyword(tk,L"list"),9,L"list");
				$isList = parser->TokenKeyword(tk);
				parser->nextToken(tk);
			}
			Ensure(parser->isIdent(tk),6,0);
			TTokenNode* $id = parser->TokenNode(tk);
			parser->nextToken(tk);
			parser->makeList($result,makeVar(ctx,$id,$isList));
			if(!(parser->isOperator(tk,',')))
			{			

				break;
			}
			parser->nextToken(tk);
		}while(true);
	}
	do
	{
		if(!(parser->isKeyword(tk,L"one") || parser->isKeyword(tk,L"option") || parser->isKeyword(tk,L"list") || parser->isIdent(tk) || parser->isOperator(tk,'!') || parser->isOperator(tk,'-') || parser->isString(tk) || parser->isChar(tk) || parser->isOperator(tk,'(')))
		{		

			break;
		}				
		parser->makeList($result,parse_bnf_term(ctx,parser,tk));
	}while(true);
	if(parser->isOperator(tk,'|'))
	{	
		
		Ensure(parser->isOperator(tk,'|'),45,0);
		parser->nextToken(tk);
		parser->makeList($result,parse_bnf_rhs(ctx,parser,tk));
	}
	return $result;

}

TNode * parse_bnf_term(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	if(parser->isKeyword(tk,L"one"))
	{		
		Ensure(parser->isKeyword(tk,L"one"),9,L"one");
		parser->nextToken(tk);
		Ensure(parser->isKeyword(tk,L"of"),9,L"of");
		parser->nextToken(tk);
		TNodeList* $items = parser->makeList();
		Ensure(parser->isOperator(tk,'('),65,0);
		parser->nextToken(tk);
		do
		{			
			parser->makeList($items,parse_bnf_rhs(ctx,parser,tk));
			if(!(parser->isOperator(tk,',')))
			{			

				break;
			}
			parser->nextToken(tk);
		}while(true);
		Ensure(parser->isOperator(tk,')'),64,0);
		parser->nextToken(tk);
		$result = makeBnfOneof(ctx,$items);
	}
	else if(parser->isKeyword(tk,L"option"))
	{		
		Ensure(parser->isKeyword(tk,L"option"),9,L"option");
		parser->nextToken(tk);
		TNode* $options = parse_bnf_term(ctx,parser,tk);
		$result = makeBnfOption(ctx,$options);
	}
	else if(parser->isKeyword(tk,L"list"))
	{		
		Ensure(parser->isKeyword(tk,L"list"),9,L"list");
		parser->nextToken(tk);
		Ensure(parser->isOperator(tk,'('),65,0);
		parser->nextToken(tk);
		TNode* $lists = parse_bnf_rhs(ctx,parser,tk);
		TNodeList* $listcond = parser->makeList();
		if(parser->isOperator(tk,','))
		{		
			
			Ensure(parser->isOperator(tk,','),70,0);
			parser->nextToken(tk);
			parser->makeList($listcond,parse_bnf_rhs(ctx,parser,tk));
		}
		Ensure(parser->isOperator(tk,')'),64,0);
		parser->nextToken(tk);
		$result = makeBnfList(ctx,$lists,$listcond);
	}
	else if(parser->isIdent(tk))
	{		
		Ensure(parser->isIdent(tk),6,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'!'))
	{		
		Ensure(parser->isOperator(tk,'!'),56,0);
		parser->nextToken(tk);
		Ensure(parser->isIdent(tk),6,0);
		TTokenNode* $_id = parser->TokenNode(tk);
		parser->nextToken(tk);
		TNodeList* $_params = parser->makeList();
		Ensure(parser->isOperator(tk,'('),65,0);
		parser->nextToken(tk);
		do
		{			
			if(parser->isIdent(tk))
			{				
				Ensure(parser->isIdent(tk),6,0);
				parser->makeList($_params,parser->TokenNode(tk));
				parser->nextToken(tk);
			}
			else if(parser->isLiteral(tk))
			{				
				Ensure(parser->isLiteral(tk),12,0);
				parser->makeList($_params,parser->TokenNode(tk));
				parser->nextToken(tk);
			}
			if(!(parser->isOperator(tk,',')))
			{			

				break;
			}
			parser->nextToken(tk);
		}while(true);
		Ensure(parser->isOperator(tk,')'),64,0);
		parser->nextToken(tk);
		$result = makeType(ctx,$_id,$_params);
	}
	else if(parser->isOperator(tk,'-'))
	{		
		Ensure(parser->isOperator(tk,'-'),34,0);
		parser->nextToken(tk);
		$result = makeGuard(ctx);
	}
	else if(parser->isString(tk) || parser->isChar(tk))
	{		
		if(parser->isString(tk))
		{			
			Ensure(parser->isString(tk),1,0);
			$result = parser->TokenNode(tk);
			parser->nextToken(tk);
		}
		else if(parser->isChar(tk))
		{			
			Ensure(parser->isChar(tk),2,0);
			$result = parser->TokenNode(tk);
			parser->nextToken(tk);
		}
	}
	else if(parser->isOperator(tk,'('))
	{		
		Ensure(parser->isOperator(tk,'('),65,0);
		parser->nextToken(tk);
		$result = parse_bnf_rhs(ctx,parser,tk);
		Ensure(parser->isOperator(tk,')'),64,0);
		parser->nextToken(tk);
	}
	return $result;

}

TNode * parse_term_syntax_define(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isKeyword(tk,L"syntax"),9,L"syntax");
	parser->nextToken(tk);
	Ensure(parser->isIdent(tk),6,0);
	TTokenNode* $name = parser->TokenNode(tk);
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,':'),90,0);
	parser->nextToken(tk);
	TNode* $snode = parse_bnf_rhs(ctx,parser,tk);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	$result = makeBnfRuler(ctx,$name,$snode);
	return $result;

}

TNode * parse_term_sure_define(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isKeyword(tk,L"sure"),9,L"sure");
	parser->nextToken(tk);
	Ensure(parser->isIdent(tk),6,0);
	TTokenNode* $name = parser->TokenNode(tk);
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,':'),90,0);
	parser->nextToken(tk);
	TNode* $snode = parse_bnf_rhs(ctx,parser,tk);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	$result = makeSureBnfRuler(ctx,$name,$snode);
	return $result;

}

TNode * parse_term_param_syntax(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNodeList* $paramItems = parser->makeList();
	Ensure(parser->isIdent(tk) && parser->isKeyword(parser->pickToken(),L"param"),6,0);
	TTokenNode* $paramID = parser->TokenNode(tk);
	parser->nextToken(tk);
	Ensure(parser->isKeyword(tk,L"param"),9,L"param");
	parser->nextToken(tk);
	do
	{		
		Ensure(parser->isIdent(tk),6,0);
		TTokenNode* $paramItemID = parser->TokenNode(tk);
		parser->nextToken(tk);
		if(parser->isKeyword(tk,L"option"))
		{		
			
			Ensure(parser->isKeyword(tk,L"option"),9,L"option");
			parser->nextToken(tk);
		}
		Ensure(parser->isKeyword(tk,L"as"),9,L"as");
		parser->nextToken(tk);
		if(parser->isKeyword(tk,L"list"))
		{		
			
			Ensure(parser->isKeyword(tk,L"list"),9,L"list");
			parser->nextToken(tk);
		}
		if(parser->isKeyword(tk,L"pair"))
		{			
			Ensure(parser->isKeyword(tk,L"pair"),9,L"pair");
			parser->nextToken(tk);
			Ensure(parser->isIdent(tk) && parser->isKeyword(parser->pickToken(),L"or"),6,0);
			parser->nextToken(tk);
			Ensure(parser->isKeyword(tk,L"or"),9,L"or");
			parser->nextToken(tk);
			Ensure(parser->isIdent(tk),6,0);
			parser->nextToken(tk);
			parser->makeList($paramItems,makeItem(ctx,$paramItemID));
		}
		else if(parser->isIdent(tk))
		{			
			Ensure(parser->isIdent(tk),6,0);
			parser->nextToken(tk);
			parser->makeList($paramItems,makeItem(ctx,$paramItemID));
		}
		if(!(parser->isOperator(tk,',')))
		{		

			break;
		}
		parser->nextToken(tk);
	}while(true);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	$result = makeParam(ctx,$paramID,$paramItems);
	return $result;

}

TNode * parse_with_body_syntax(Context &ctx,TParser* parser, TToken &tk)
{	
	TNodeList* $result = parser->makeList();
	do
	{
		if(!(parser->isKeyword(tk,L"unit") || parser->isIdent(tk) && parser->isKeyword(parser->pickToken(),L"syntax") || parser->isKeyword(tk,L"syntax") || parser->isKeyword(tk,L"sure") || parser->isIdent(tk) && parser->isKeyword(parser->pickToken(),L"param")))
		{		

			break;
		}				
		if(parser->isIdent(tk) && parser->isKeyword(parser->pickToken(),L"param"))
		{			
			parser->makeList($result,parse_term_param_syntax(ctx,parser,tk));
		}
		else if(parser->isKeyword(tk,L"syntax"))
		{			
			parser->makeList($result,parse_term_syntax_define(ctx,parser,tk));
		}
		else if(parser->isKeyword(tk,L"unit"))
		{			
			parser->makeList($result,parse_with_unit_syntax(ctx,parser,tk));
		}
		else if(parser->isIdent(tk) && parser->isKeyword(parser->pickToken(),L"syntax"))
		{			
			parser->makeList($result,parse_with_term_syntax(ctx,parser,tk));
		}
		else if(parser->isKeyword(tk,L"sure"))
		{			
			parser->makeList($result,parse_term_sure_define(ctx,parser,tk));
		}
	}while(true);
	return $result;

}
