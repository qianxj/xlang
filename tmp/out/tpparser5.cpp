#include "vlpreparse.hpp"

using namespace trap::complier;
TNode* parse_trans_unit(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_identifier(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_declare_namespace(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_declare_class(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_method_field_declare(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_literal(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_base_clause(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_native_spec(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_class_member(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_type_id(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_apply_declare(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_native_spec_list(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_native_spec_item(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_paramlist(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_function_body(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_expression(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_type_spec(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_type_primary(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_type_param(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_type_rank(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_stmt(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_id_expression(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_primary_expession(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_postfix_expression(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_unary_expression(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_cast_expression(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_binary_op(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_binary_expression(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_expression_or_variant_declare_stmt(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_expression_stmt(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_comp_stmt(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_if_stmt(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_cond(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_return_stmt(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_while_stmt(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_until_stmt(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_for_stmt(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_break_stmt(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_empty_stmt(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_continue_stmt(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_match_param_list(Context &ctx,TParser* parser, TToken &tk);
TNode* parse_match_param(Context &ctx,TParser* parser, TToken &tk);

TNode * parse_trans_unit(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	do
	{
		if(!(parser->isKeyword(tk,L"unit")))
		{		

			break;
		}				
		Ensure(parser->isKeyword(tk,L"unit"),9,L"unit");
		parser->nextToken(tk);
		parse_identifier(ctx,parser,tk);
		Ensure(parser->isOperator(tk,'['),69,0);
		parser->nextToken(tk);
		do
		{
			if(!(parser->isKeyword(tk,L"namespace") || parser->isKeyword(tk,L"class") || parser->isKeyword(tk,L"struct") || parser->isKeyword(tk,L"enum") || parser->isKeyword(tk,L"interface") || parser->isKeyword(tk,L"context") || parser->isKeyword(tk,L"function") || parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isIdent(tk)))
			{			

				break;
			}						
			if(parser->isKeyword(tk,L"namespace"))
			{				
				parse_declare_namespace(ctx,parser,tk);
			}
			else if(parser->isKeyword(tk,L"class") || parser->isKeyword(tk,L"struct") || parser->isKeyword(tk,L"enum") || parser->isKeyword(tk,L"interface") || parser->isKeyword(tk,L"context"))
			{				
				parse_declare_class(ctx,parser,tk);
			}
			else if(parser->isKeyword(tk,L"function") || parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isIdent(tk))
			{				
				parse_method_field_declare(ctx,parser,tk);
			}
		}while(true);
		Ensure(parser->isOperator(tk,']'),68,0);
		parser->nextToken(tk);
	}while(true);
	return $result;

}

TNode * parse_identifier(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isIdent(tk),6,0);
	$result = parser->TokenNode(tk);
	parser->nextToken(tk);
	return $result;

}

TNode * parse_declare_namespace(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNode* $id = 0 ;
	Ensure(parser->isKeyword(tk,L"namespace"),9,L"namespace");
	parser->nextToken(tk);
	if(parser->isIdent(tk))
	{	
		
		$id = parse_identifier(ctx,parser,tk);
	}
	if(parser->isKeyword(tk,L"for"))
	{		
		Ensure(parser->isKeyword(tk,L"for"),9,L"for");
		parser->nextToken(tk);
		Ensure(parser->isKeyword(tk,L"ole"),9,L"ole");
		parser->nextToken(tk);
		TNode* $r = parse_literal(ctx,parser,tk);
		Ensure(parser->isOperator(tk,';'),71,0);
		parser->nextToken(tk);
		addOleNamespace(ctx,$id,$r);
	}
	else if(parser->isOperator(tk,'{'))
	{		
		enterNamespace(ctx,$id);
		Ensure(parser->isOperator(tk,'{'),67,0);
		parser->nextToken(tk);
		if(parser->isKeyword(tk,L"namespace") || parser->isKeyword(tk,L"class") || parser->isKeyword(tk,L"struct") || parser->isKeyword(tk,L"enum") || parser->isKeyword(tk,L"interface") || parser->isKeyword(tk,L"context") || parser->isKeyword(tk,L"function") || parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isIdent(tk))
		{		
			
			do
			{
				if(!(parser->isKeyword(tk,L"namespace") || parser->isKeyword(tk,L"class") || parser->isKeyword(tk,L"struct") || parser->isKeyword(tk,L"enum") || parser->isKeyword(tk,L"interface") || parser->isKeyword(tk,L"context") || parser->isKeyword(tk,L"function") || parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isIdent(tk)))
				{				

					break;
				}								
				if(parser->isKeyword(tk,L"namespace"))
				{					
					parse_declare_namespace(ctx,parser,tk);
				}
				else if(parser->isKeyword(tk,L"class") || parser->isKeyword(tk,L"struct") || parser->isKeyword(tk,L"enum") || parser->isKeyword(tk,L"interface") || parser->isKeyword(tk,L"context"))
				{					
					parse_declare_class(ctx,parser,tk);
				}
				else if(parser->isKeyword(tk,L"function") || parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isIdent(tk))
				{					
					parse_method_field_declare(ctx,parser,tk);
				}
			}while(true);
		}
		Ensure(parser->isOperator(tk,'}'),66,0);
		parser->nextToken(tk);
		leaveNamespace(ctx);
	}
	return $result;

}

TNode * parse_declare_class(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNode* $base = 0 ;
	TNode* $class_key = 0 ;
	if(parser->isKeyword(tk,L"class"))
	{		
		Ensure(parser->isKeyword(tk,L"class"),9,L"class");
		$class_key = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"struct"))
	{		
		Ensure(parser->isKeyword(tk,L"struct"),9,L"struct");
		$class_key = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"enum"))
	{		
		Ensure(parser->isKeyword(tk,L"enum"),9,L"enum");
		$class_key = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"interface"))
	{		
		Ensure(parser->isKeyword(tk,L"interface"),9,L"interface");
		$class_key = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"context"))
	{		
		Ensure(parser->isKeyword(tk,L"context"),9,L"context");
		$class_key = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	TNode* $id = parse_identifier(ctx,parser,tk);
	if(parser->isOperator(tk,';'))
	{		
		Ensure(parser->isOperator(tk,';'),71,0);
		parser->nextToken(tk);
		$result = forwardClass(ctx,$class_key,$id,$base);
	}
	else if(parser->isOperator(tk,':') || parser->isOperator(tk,'{') || parser->isOperator(tk,';') || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"from") || parser->isKeyword(tk,L"alias"))
	{		
		if(parser->isOperator(tk,':'))
		{		
			
			Ensure(parser->isOperator(tk,':'),90,0);
			parser->nextToken(tk);
			$base = parse_base_clause(ctx,parser,tk);
		}
		$result = makeClass(ctx,$class_key,$id,$base);
		if(parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"from") || parser->isKeyword(tk,L"alias"))
		{		
			
			parse_native_spec(ctx,parser,tk);
		}
		if(parser->isOperator(tk,'{'))
		{		
			
			Ensure(parser->isOperator(tk,'{'),67,0);
			parser->nextToken(tk);
			if(parser->isKeyword(tk,L"public") || parser->isKeyword(tk,L"protected") || parser->isKeyword(tk,L"private") || parser->isKeyword(tk,L"function") || parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isKeyword(tk,L"apply") || parser->isIdent(tk))
			{			
				
				parse_class_member(ctx,parser,tk);
			}
			Ensure(parser->isOperator(tk,'}'),66,0);
			parser->nextToken(tk);
		}
		finishedClass(ctx);
		Ensure(parser->isOperator(tk,';'),71,0);
		parser->nextToken(tk);
	}
	return $result;

}

TNode * parse_method_field_declare(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNode* $paramlist = 0 ;
	TNode* $init = 0 ;
	if(parser->isKeyword(tk,L"function"))
	{	
		
		Ensure(parser->isKeyword(tk,L"function"),9,L"function");
		parser->nextToken(tk);
	}
	TNode* $type = parse_type_id(ctx,parser,tk);
	TNode* $id = parse_identifier(ctx,parser,tk);
	if(parser->isOperator(tk,'('))
	{		
		Ensure(parser->isOperator(tk,'('),65,0);
		parser->nextToken(tk);
		if(parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isIdent(tk))
		{		
			
			$paramlist = parse_paramlist(ctx,parser,tk);
		}
		Ensure(parser->isOperator(tk,')'),64,0);
		parser->nextToken(tk);
		if(parser->isKeyword(tk,L"override"))
		{		
			
			Ensure(parser->isKeyword(tk,L"override"),9,L"override");
			parser->nextToken(tk);
		}
		makeFunction(ctx,$id,$type,$paramlist);
		parse_native_spec(ctx,parser,tk);
		if(parser->isOperator(tk,';'))
		{			
			Ensure(parser->isOperator(tk,';'),71,0);
			parser->nextToken(tk);
		}
		else if(parser->isOperator(tk,'{'))
		{			
			Ensure(parser->isOperator(tk,'{'),67,0);
			parser->nextToken(tk);
			TNode* $body = parse_function_body(ctx,parser,tk);
			Ensure(parser->isOperator(tk,'}'),66,0);
			parser->nextToken(tk);
		}
		finishedMethod(ctx);
	}
	else
	{
		if(parser->isOperator(tk,'='))
		{		
			
			Ensure(parser->isOperator(tk,'='),38,0);
			parser->nextToken(tk);
			$init = parse_expression(ctx,parser,tk);
		}
		makeTypeField(ctx,$type,$id,$init);
		Ensure(parser->isOperator(tk,';'),71,0);
		parser->nextToken(tk);
	}
	return $result;

}

TNode * parse_literal(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isLiteral(tk),12,0);
	$result = parser->TokenNode(tk);
	parser->nextToken(tk);
	return $result;

}

TNode * parse_base_clause(Context &ctx,TParser* parser, TToken &tk)
{	
	TNodeList* $result = parser->makeList();
	TNode* $access = 0 ;
	if(parser->isKeyword(tk,L"public"))
	{		
		Ensure(parser->isKeyword(tk,L"public"),9,L"public");
		$access = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"private"))
	{		
		Ensure(parser->isKeyword(tk,L"private"),9,L"private");
		$access = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	TNode* $type = parse_type_id(ctx,parser,tk);
	parser->makeList($result,makeClassBase(ctx,$access,$type));
	if(parser->isOperator(tk,','))
	{	
		
		Ensure(parser->isOperator(tk,','),70,0);
		parser->nextToken(tk);
		parser->makeList($result,parse_base_clause(ctx,parser,tk));
	}
	return $result;

}

TNode * parse_native_spec(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	if(parser->isKeyword(tk,L"native"))
	{	
		
		Ensure(parser->isKeyword(tk,L"native"),9,L"native");
		parser->nextToken(tk);
	}
	parse_native_spec_list(ctx,parser,tk);
	return $result;

}

TNode * parse_class_member(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	do
	{
		if(!(parser->isKeyword(tk,L"public") || parser->isKeyword(tk,L"protected") || parser->isKeyword(tk,L"private") || parser->isKeyword(tk,L"function") || parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isKeyword(tk,L"apply") || parser->isIdent(tk)))
		{		

			break;
		}				
		if(parser->isKeyword(tk,L"public"))
		{			
			Ensure(parser->isKeyword(tk,L"public"),9,L"public");
			parser->nextToken(tk);
			Ensure(parser->isOperator(tk,':'),90,0);
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"protected"))
		{			
			Ensure(parser->isKeyword(tk,L"protected"),9,L"protected");
			parser->nextToken(tk);
			Ensure(parser->isOperator(tk,':'),90,0);
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"private"))
		{			
			Ensure(parser->isKeyword(tk,L"private"),9,L"private");
			parser->nextToken(tk);
			Ensure(parser->isOperator(tk,':'),90,0);
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"apply"))
		{			
			parse_apply_declare(ctx,parser,tk);
		}
		else if(parser->isKeyword(tk,L"function") || parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isIdent(tk))
		{			
			parse_method_field_declare(ctx,parser,tk);
		}
	}while(true);
	return $result;

}

TNode * parse_type_id(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNode* $id = 0 ;
	TNode* $spec = 0 ;
	TNode* $param = 0 ;
	TNode* $rank = 0 ;
	TNode* $term = 0 ;
	if(parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native"))
	{	
		
		$spec = parse_type_spec(ctx,parser,tk);
	}
	if(parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char"))
	{		
		$id = parse_type_primary(ctx,parser,tk);
		$term = makeQualifiedTerm(ctx,$term,$id,$param);
	}
	else
	{
		do
		{			
			$id = parse_identifier(ctx,parser,tk);
			$term = makeQualifiedTerm(ctx,$term,$id,$param);
			if(!(parser->isOperator(tk,L"::")))
			{			

				break;
			}
			parser->nextToken(tk);
		}while(true);
	}
	if(parser->isOperator(tk,'['))
	{	
		
		$rank = parse_type_rank(ctx,parser,tk);
	}
	$result = makeQualifiedId(ctx,$spec,$term,$rank);
	return $result;

}

TNode * parse_apply_declare(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNode* $type = 0 ;
	Ensure(parser->isKeyword(tk,L"apply"),9,L"apply");
	parser->nextToken(tk);
	TNode* $id = parse_identifier(ctx,parser,tk);
	Ensure(parser->isOperator(tk,'('),65,0);
	parser->nextToken(tk);
	TNode* $params = parse_match_param_list(ctx,parser,tk);
	Ensure(parser->isOperator(tk,')'),64,0);
	parser->nextToken(tk);
	if(parser->isOperator(tk,':'))
	{	
		
		Ensure(parser->isOperator(tk,':'),90,0);
		parser->nextToken(tk);
		$type = parse_type_id(ctx,parser,tk);
	}
	$result = makeApply(ctx,$id,$type,$params);
	if(parser->isOperator(tk,';'))
	{		
		Ensure(parser->isOperator(tk,';'),71,0);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'{'))
	{		
		Ensure(parser->isOperator(tk,'{'),67,0);
		parser->nextToken(tk);
		if(parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isKeyword(tk,L"this") || parser->isKeyword(tk,L"super") || parser->isOperator(tk,'(') || parser->isKeyword(tk,L"sizeof") || parser->isKeyword(tk,L"new") || parser->isKeyword(tk,L"delete") || parser->isOperator(tk,'~') || parser->isOperator(tk,'!') || parser->isOperator(tk,L"++") || parser->isOperator(tk,L"--") || parser->isOperator(tk,'+') || parser->isOperator(tk,'-') || parser->isKeyword(tk,L"cast") || parser->isKeyword(tk,L"trust") || parser->isOperator(tk,'{') || parser->isKeyword(tk,L"if") || parser->isKeyword(tk,L"return") || parser->isKeyword(tk,L"while") || parser->isKeyword(tk,L"do") || parser->isKeyword(tk,L"for") || parser->isKeyword(tk,L"break") || parser->isOperator(tk,';') || parser->isKeyword(tk,L"continue") || parser->isIdent(tk) || parser->isLiteral(tk))
		{		
			
			TNode* $body = parse_function_body(ctx,parser,tk);
		}
		Ensure(parser->isOperator(tk,'}'),66,0);
		parser->nextToken(tk);
	}
	finishedMethod(ctx);
	return $result;

}

TNode * parse_native_spec_list(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	do
	{
		if(!(parser->isKeyword(tk,L"from") || parser->isKeyword(tk,L"alias")))
		{		

			break;
		}				
		parse_native_spec_item(ctx,parser,tk);
	}while(true);
	return $result;

}

TNode * parse_native_spec_item(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	if(parser->isKeyword(tk,L"from"))
	{		
		Ensure(parser->isKeyword(tk,L"from"),9,L"from");
		parser->nextToken(tk);
		TNode* $from = parse_literal(ctx,parser,tk);
		SetNativeFrom(ctx,$from);
	}
	else if(parser->isKeyword(tk,L"alias"))
	{		
		Ensure(parser->isKeyword(tk,L"alias"),9,L"alias");
		parser->nextToken(tk);
		TNode* $alias = parse_literal(ctx,parser,tk);
		SetNativeAlias(ctx,$alias);
	}
	return $result;

}

TNode * parse_paramlist(Context &ctx,TParser* parser, TToken &tk)
{	
	TNodeList* $result = parser->makeList();
	do
	{		
		TNode* $type_id = 0 ;
		TNode* $arg = 0 ;
		TNode* $init = 0 ;
		$type_id = parse_type_id(ctx,parser,tk);
		if(parser->isIdent(tk))
		{		
			
			$arg = parse_identifier(ctx,parser,tk);
		}
		if(parser->isOperator(tk,'='))
		{		
			
			Ensure(parser->isOperator(tk,'='),38,0);
			parser->nextToken(tk);
			$init = parse_expression(ctx,parser,tk);
		}
		parser->makeList($result,makeParam(ctx,$arg,$type_id,$init));
		if(!(parser->isOperator(tk,',')))
		{		

			break;
		}
		parser->nextToken(tk);
	}while(true);
	return $result;

}

TNode * parse_function_body(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNodeList* $stmt = parser->makeList();
	do
	{
		if(!(parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isKeyword(tk,L"this") || parser->isKeyword(tk,L"super") || parser->isOperator(tk,'(') || parser->isKeyword(tk,L"sizeof") || parser->isKeyword(tk,L"new") || parser->isKeyword(tk,L"delete") || parser->isOperator(tk,'~') || parser->isOperator(tk,'!') || parser->isOperator(tk,L"++") || parser->isOperator(tk,L"--") || parser->isOperator(tk,'+') || parser->isOperator(tk,'-') || parser->isKeyword(tk,L"cast") || parser->isKeyword(tk,L"trust") || parser->isOperator(tk,'{') || parser->isKeyword(tk,L"if") || parser->isKeyword(tk,L"return") || parser->isKeyword(tk,L"while") || parser->isKeyword(tk,L"do") || parser->isKeyword(tk,L"for") || parser->isKeyword(tk,L"break") || parser->isOperator(tk,';') || parser->isKeyword(tk,L"continue") || parser->isIdent(tk) || parser->isLiteral(tk)))
		{		

			break;
		}				
		parser->makeList($stmt,parse_stmt(ctx,parser,tk));
	}while(true);
	$result = makeCompStmt(ctx,$stmt);
	$result = setFunctionBody(ctx,$result);
	return $result;

}

TNode * parse_expression(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	$result = parse_binary_expression(ctx,parser,tk);
	makeExpression(ctx,$result);
	return $result;

}

TNode * parse_type_spec(Context &ctx,TParser* parser, TToken &tk)
{	
	TNodeList* $result = parser->makeList();
	do
	{
		if(!(parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native")))
		{		

			break;
		}				
		if(parser->isKeyword(tk,L"typedef"))
		{			
			Ensure(parser->isKeyword(tk,L"typedef"),9,L"typedef");
			parser->makeList($result,parser->TokenKeyword(tk));
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"manage"))
		{			
			Ensure(parser->isKeyword(tk,L"manage"),9,L"manage");
			parser->makeList($result,parser->TokenKeyword(tk));
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"unsigned"))
		{			
			Ensure(parser->isKeyword(tk,L"unsigned"),9,L"unsigned");
			parser->makeList($result,parser->TokenKeyword(tk));
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"static"))
		{			
			Ensure(parser->isKeyword(tk,L"static"),9,L"static");
			parser->makeList($result,parser->TokenKeyword(tk));
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"virtual"))
		{			
			Ensure(parser->isKeyword(tk,L"virtual"),9,L"virtual");
			parser->makeList($result,parser->TokenKeyword(tk));
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"stdcall"))
		{			
			Ensure(parser->isKeyword(tk,L"stdcall"),9,L"stdcall");
			parser->makeList($result,parser->TokenKeyword(tk));
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"const"))
		{			
			Ensure(parser->isKeyword(tk,L"const"),9,L"const");
			parser->makeList($result,parser->TokenKeyword(tk));
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"byref"))
		{			
			Ensure(parser->isKeyword(tk,L"byref"),9,L"byref");
			parser->makeList($result,parser->TokenKeyword(tk));
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"override"))
		{			
			Ensure(parser->isKeyword(tk,L"override"),9,L"override");
			parser->makeList($result,parser->TokenKeyword(tk));
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"inline"))
		{			
			Ensure(parser->isKeyword(tk,L"inline"),9,L"inline");
			parser->makeList($result,parser->TokenKeyword(tk));
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"ref"))
		{			
			Ensure(parser->isKeyword(tk,L"ref"),9,L"ref");
			parser->makeList($result,parser->TokenKeyword(tk));
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"native"))
		{			
			Ensure(parser->isKeyword(tk,L"native"),9,L"native");
			parser->makeList($result,parser->TokenKeyword(tk));
			parser->nextToken(tk);
		}
	}while(true);
	return $result;

}

TNode * parse_type_primary(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	if(parser->isKeyword(tk,L"int"))
	{		
		Ensure(parser->isKeyword(tk,L"int"),9,L"int");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"double"))
	{		
		Ensure(parser->isKeyword(tk,L"double"),9,L"double");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"float"))
	{		
		Ensure(parser->isKeyword(tk,L"float"),9,L"float");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"string"))
	{		
		Ensure(parser->isKeyword(tk,L"string"),9,L"string");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"bool"))
	{		
		Ensure(parser->isKeyword(tk,L"bool"),9,L"bool");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"int64"))
	{		
		Ensure(parser->isKeyword(tk,L"int64"),9,L"int64");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"short"))
	{		
		Ensure(parser->isKeyword(tk,L"short"),9,L"short");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"long"))
	{		
		Ensure(parser->isKeyword(tk,L"long"),9,L"long");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"oleobject"))
	{		
		Ensure(parser->isKeyword(tk,L"oleobject"),9,L"oleobject");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"void"))
	{		
		Ensure(parser->isKeyword(tk,L"void"),9,L"void");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"uint"))
	{		
		Ensure(parser->isKeyword(tk,L"uint"),9,L"uint");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"ushort"))
	{		
		Ensure(parser->isKeyword(tk,L"ushort"),9,L"ushort");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"char"))
	{		
		Ensure(parser->isKeyword(tk,L"char"),9,L"char");
		$result = parser->TokenKeyword(tk);
		parser->nextToken(tk);
	}
	return $result;

}

TNode * parse_type_param(Context &ctx,TParser* parser, TToken &tk)
{	
	TNodeList* $result = parser->makeList();
	Ensure(parser->isOperator(tk,'<'),52,0);
	parser->nextToken(tk);
	do
	{		
		parser->makeList($result,parse_type_id(ctx,parser,tk));
		if(!(parser->isOperator(tk,',')))
		{		

			break;
		}
		parser->nextToken(tk);
	}while(true);
	Ensure(parser->isOperator(tk,'>'),50,0);
	parser->nextToken(tk);
	return $result;

}

TNode * parse_type_rank(Context &ctx,TParser* parser, TToken &tk)
{	
	TNodeList* $result = parser->makeList();
	do
	{
		if(!(parser->isOperator(tk,'[')))
		{		

			break;
		}				
		TNode* $r = 0 ;
		Ensure(parser->isOperator(tk,'['),69,0);
		parser->nextToken(tk);
		if(parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isKeyword(tk,L"this") || parser->isKeyword(tk,L"super") || parser->isOperator(tk,'(') || parser->isKeyword(tk,L"sizeof") || parser->isKeyword(tk,L"new") || parser->isKeyword(tk,L"delete") || parser->isOperator(tk,'~') || parser->isOperator(tk,'!') || parser->isOperator(tk,L"++") || parser->isOperator(tk,L"--") || parser->isOperator(tk,'+') || parser->isOperator(tk,'-') || parser->isKeyword(tk,L"cast") || parser->isKeyword(tk,L"trust") || parser->isIdent(tk) || parser->isLiteral(tk))
		{		
			
			$r = parse_expression(ctx,parser,tk);
		}
		parser->makeList($result,$r);
		Ensure(parser->isOperator(tk,']'),68,0);
		parser->nextToken(tk);
	}while(true);
	return $result;

}

TNode * parse_stmt(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	if(parser->isOperator(tk,';'))
	{		
		parse_empty_stmt(ctx,parser,tk);
	}
	else if(parser->isKeyword(tk,L"if"))
	{		
		$result = parse_if_stmt(ctx,parser,tk);
	}
	else if(parser->isKeyword(tk,L"return"))
	{		
		$result = parse_return_stmt(ctx,parser,tk);
	}
	else if(parser->isKeyword(tk,L"continue"))
	{		
		$result = parse_continue_stmt(ctx,parser,tk);
	}
	else if(parser->isKeyword(tk,L"break"))
	{		
		$result = parse_break_stmt(ctx,parser,tk);
	}
	else if(parser->isKeyword(tk,L"for"))
	{		
		$result = parse_for_stmt(ctx,parser,tk);
	}
	else if(parser->isKeyword(tk,L"while"))
	{		
		$result = parse_while_stmt(ctx,parser,tk);
	}
	else if(parser->isKeyword(tk,L"do"))
	{		
		$result = parse_until_stmt(ctx,parser,tk);
	}
	else if(parser->isOperator(tk,'{'))
	{		
		$result = parse_comp_stmt(ctx,parser,tk);
	}
	else if(parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isKeyword(tk,L"this") || parser->isKeyword(tk,L"super") || parser->isOperator(tk,'(') || parser->isKeyword(tk,L"sizeof") || parser->isKeyword(tk,L"new") || parser->isKeyword(tk,L"delete") || parser->isOperator(tk,'~') || parser->isOperator(tk,'!') || parser->isOperator(tk,L"++") || parser->isOperator(tk,L"--") || parser->isOperator(tk,'+') || parser->isOperator(tk,'-') || parser->isKeyword(tk,L"cast") || parser->isKeyword(tk,L"trust") || parser->isIdent(tk) || parser->isLiteral(tk))
	{		
		$result = parse_expression_or_variant_declare_stmt(ctx,parser,tk);
	}
	return $result;

}

TNode * parse_id_expression(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	$result = parse_type_id(ctx,parser,tk);
	return $result;

}

TNode * parse_primary_expession(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNode* $r = 0 ;
	if(parser->isKeyword(tk,L"this"))
	{		
		Ensure(parser->isKeyword(tk,L"this"),9,L"this");
		parser->nextToken(tk);
		$result = makeThis(ctx);
	}
	else if(parser->isKeyword(tk,L"super"))
	{		
		Ensure(parser->isKeyword(tk,L"super"),9,L"super");
		parser->nextToken(tk);
		$result = makeSuper(ctx);
	}
	else if(parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isIdent(tk))
	{		
		$result = parse_id_expression(ctx,parser,tk);
	}
	else if(parser->isLiteral(tk))
	{		
		$r = parse_literal(ctx,parser,tk);
		$result = makeLiteral(ctx,$r);
	}
	else if(parser->isOperator(tk,'('))
	{		
		Ensure(parser->isOperator(tk,'('),65,0);
		parser->nextToken(tk);
		$result = parse_expression(ctx,parser,tk);
		Ensure(parser->isOperator(tk,')'),64,0);
		parser->nextToken(tk);
	}
	return $result;

}

TNode * parse_postfix_expression(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNodeList* $postfix = parser->makeList();
	$result = parse_primary_expession(ctx,parser,tk);
	if(parser->isOperator(tk,'.') || parser->isOperator(tk,'(') || parser->isOperator(tk,'[') || parser->isOperator(tk,L"++") || parser->isOperator(tk,L"--"))
	{	
		
		do
		{
			if(!(parser->isOperator(tk,'.') || parser->isOperator(tk,'(') || parser->isOperator(tk,'[') || parser->isOperator(tk,L"++") || parser->isOperator(tk,L"--")))
			{			

				break;
			}						
			if(parser->isOperator(tk,'.'))
			{				
				Ensure(parser->isOperator(tk,'.'),92,0);
				parser->nextToken(tk);
				TNode* $field = parse_identifier(ctx,parser,tk);
				$result = makeField(ctx,$result,$field);
			}
			else if(parser->isOperator(tk,'('))
			{				
				TNodeList* $args = parser->makeList();
				Ensure(parser->isOperator(tk,'('),65,0);
				parser->nextToken(tk);
				do
				{					
					parser->makeList($args,parse_expression(ctx,parser,tk));
					if(!(parser->isOperator(tk,',')))
					{					

						break;
					}
					parser->nextToken(tk);
				}while(true);
				Ensure(parser->isOperator(tk,')'),64,0);
				parser->nextToken(tk);
				$result = makeCall(ctx,$result,$args);
			}
			else if(parser->isOperator(tk,'['))
			{				
				Ensure(parser->isOperator(tk,'['),69,0);
				parser->nextToken(tk);
				TNode* $rank = parse_expression(ctx,parser,tk);
				Ensure(parser->isOperator(tk,']'),68,0);
				parser->nextToken(tk);
				$result = makeElement(ctx,$result,$rank);
			}
			else if(parser->isOperator(tk,L"++"))
			{				
				Ensure(parser->isOperator(tk,L"++"),39,0);
				parser->nextToken(tk);
				$result = makeInc(ctx,$result);
			}
			else if(parser->isOperator(tk,L"--"))
			{				
				Ensure(parser->isOperator(tk,L"--"),40,0);
				parser->nextToken(tk);
				$result = makeDec(ctx,$result);
			}
		}while(true);
	}
	return $result;

}

TNode * parse_unary_expression(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNode* $unary = 0 ;
	if(parser->isKeyword(tk,L"sizeof") || parser->isKeyword(tk,L"new") || parser->isKeyword(tk,L"delete") || parser->isOperator(tk,'~') || parser->isOperator(tk,'!') || parser->isOperator(tk,L"++") || parser->isOperator(tk,L"--") || parser->isOperator(tk,'+') || parser->isOperator(tk,'-'))
	{	
		
		if(parser->isKeyword(tk,L"sizeof"))
		{			
			Ensure(parser->isKeyword(tk,L"sizeof"),9,L"sizeof");
			$unary = parser->TokenKeyword(tk);
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"new"))
		{			
			Ensure(parser->isKeyword(tk,L"new"),9,L"new");
			$unary = parser->TokenKeyword(tk);
			parser->nextToken(tk);
		}
		else if(parser->isKeyword(tk,L"delete"))
		{			
			Ensure(parser->isKeyword(tk,L"delete"),9,L"delete");
			$unary = parser->TokenKeyword(tk);
			parser->nextToken(tk);
		}
		else if(parser->isOperator(tk,'~'))
		{			
			Ensure(parser->isOperator(tk,'~'),47,0);
			$unary = parser->TokenNode(tk);
			parser->nextToken(tk);
		}
		else if(parser->isOperator(tk,'!'))
		{			
			Ensure(parser->isOperator(tk,'!'),56,0);
			$unary = parser->TokenNode(tk);
			parser->nextToken(tk);
		}
		else if(parser->isOperator(tk,L"++"))
		{			
			Ensure(parser->isOperator(tk,L"++"),39,0);
			$unary = parser->TokenNode(tk);
			parser->nextToken(tk);
		}
		else if(parser->isOperator(tk,L"--"))
		{			
			Ensure(parser->isOperator(tk,L"--"),40,0);
			$unary = parser->TokenNode(tk);
			parser->nextToken(tk);
		}
		else if(parser->isOperator(tk,'+'))
		{			
			Ensure(parser->isOperator(tk,'+'),33,0);
			$unary = parser->TokenNode(tk);
			parser->nextToken(tk);
		}
		else if(parser->isOperator(tk,'-'))
		{			
			Ensure(parser->isOperator(tk,'-'),34,0);
			$unary = parser->TokenNode(tk);
			parser->nextToken(tk);
		}
	}
	TNode* $postfix_expression = parse_postfix_expression(ctx,parser,tk);
	$result = makeUnryExpr(ctx,$unary,$postfix_expression);
	return $result;

}

TNode * parse_cast_expression(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	if(parser->isKeyword(tk,L"cast"))
	{		
		Ensure(parser->isKeyword(tk,L"cast"),9,L"cast");
		parser->nextToken(tk);
		Ensure(parser->isOperator(tk,'('),65,0);
		parser->nextToken(tk);
		$result = parse_cast_expression(ctx,parser,tk);
		Ensure(parser->isKeyword(tk,L"as"),9,L"as");
		parser->nextToken(tk);
		TNode* $to = parse_type_id(ctx,parser,tk);
		$result = makeCast(ctx,$result,$to);
		Ensure(parser->isOperator(tk,')'),64,0);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"trust"))
	{		
		Ensure(parser->isKeyword(tk,L"trust"),9,L"trust");
		parser->nextToken(tk);
		Ensure(parser->isOperator(tk,'('),65,0);
		parser->nextToken(tk);
		$result = parse_cast_expression(ctx,parser,tk);
		Ensure(parser->isKeyword(tk,L"as"),9,L"as");
		parser->nextToken(tk);
		TNode* $to1 = parse_type_id(ctx,parser,tk);
		$result = makeCast(ctx,$result,$to1);
		Ensure(parser->isOperator(tk,')'),64,0);
		parser->nextToken(tk);
	}
	else if(parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isKeyword(tk,L"this") || parser->isKeyword(tk,L"super") || parser->isOperator(tk,'(') || parser->isKeyword(tk,L"sizeof") || parser->isKeyword(tk,L"new") || parser->isKeyword(tk,L"delete") || parser->isOperator(tk,'~') || parser->isOperator(tk,'!') || parser->isOperator(tk,L"++") || parser->isOperator(tk,L"--") || parser->isOperator(tk,'+') || parser->isOperator(tk,'-') || parser->isIdent(tk) || parser->isLiteral(tk))
	{		
		$result = parse_unary_expression(ctx,parser,tk);
	}
	return $result;

}

TNode * parse_binary_op(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	if(parser->isOperator(tk,'+'))
	{		
		Ensure(parser->isOperator(tk,'+'),33,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'-'))
	{		
		Ensure(parser->isOperator(tk,'-'),34,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'*'))
	{		
		Ensure(parser->isOperator(tk,'*'),35,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'/'))
	{		
		Ensure(parser->isOperator(tk,'/'),36,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'%'))
	{		
		Ensure(parser->isOperator(tk,'%'),37,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'*'))
	{		
		Ensure(parser->isOperator(tk,'*'),35,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'&'))
	{		
		Ensure(parser->isOperator(tk,'&'),44,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'|'))
	{		
		Ensure(parser->isOperator(tk,'|'),45,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'^'))
	{		
		Ensure(parser->isOperator(tk,'^'),46,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"<<"))
	{		
		Ensure(parser->isOperator(tk,L"<<"),42,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L">>"))
	{		
		Ensure(parser->isOperator(tk,L">>"),43,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"&&"))
	{		
		Ensure(parser->isOperator(tk,L"&&"),49,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"||"))
	{		
		Ensure(parser->isOperator(tk,L"||"),48,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'='))
	{		
		Ensure(parser->isOperator(tk,'='),38,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'<'))
	{		
		Ensure(parser->isOperator(tk,'<'),52,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,'>'))
	{		
		Ensure(parser->isOperator(tk,'>'),50,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L">="))
	{		
		Ensure(parser->isOperator(tk,L">="),51,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"<="))
	{		
		Ensure(parser->isOperator(tk,L"<="),53,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"=="))
	{		
		Ensure(parser->isOperator(tk,L"=="),54,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"!="))
	{		
		Ensure(parser->isOperator(tk,L"!="),55,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"+="))
	{		
		Ensure(parser->isOperator(tk,L"+="),113,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"-="))
	{		
		Ensure(parser->isOperator(tk,L"-="),114,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"*="))
	{		
		Ensure(parser->isOperator(tk,L"*="),115,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"/="))
	{		
		Ensure(parser->isOperator(tk,L"/="),116,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"%="))
	{		
		Ensure(parser->isOperator(tk,L"%="),117,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L">>="))
	{		
		Ensure(parser->isOperator(tk,L">>="),123,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"<<="))
	{		
		Ensure(parser->isOperator(tk,L"<<="),122,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"&="))
	{		
		Ensure(parser->isOperator(tk,L"&="),124,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"|="))
	{		
		Ensure(parser->isOperator(tk,L"|="),125,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"^="))
	{		
		Ensure(parser->isOperator(tk,L"^="),126,0);
		$result = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	return $result;

}

TNode * parse_binary_expression(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNode* $op = 0 ;
	$result = parse_cast_expression(ctx,parser,tk);
	if(parser->isOperator(tk,'+') || parser->isOperator(tk,'-') || parser->isOperator(tk,'*') || parser->isOperator(tk,'/') || parser->isOperator(tk,'%') || parser->isOperator(tk,'*') || parser->isOperator(tk,'&') || parser->isOperator(tk,'|') || parser->isOperator(tk,'^') || parser->isOperator(tk,L"<<") || parser->isOperator(tk,L">>") || parser->isOperator(tk,L"&&") || parser->isOperator(tk,L"||") || parser->isOperator(tk,'=') || parser->isOperator(tk,'<') || parser->isOperator(tk,'>') || parser->isOperator(tk,L">=") || parser->isOperator(tk,L"<=") || parser->isOperator(tk,L"==") || parser->isOperator(tk,L"!=") || parser->isOperator(tk,L"+=") || parser->isOperator(tk,L"-=") || parser->isOperator(tk,L"*=") || parser->isOperator(tk,L"/=") || parser->isOperator(tk,L"%=") || parser->isOperator(tk,L">>=") || parser->isOperator(tk,L"<<=") || parser->isOperator(tk,L"&=") || parser->isOperator(tk,L"|=") || parser->isOperator(tk,L"^="))
	{	
		
		do
		{
			if(!(parser->isOperator(tk,'+') || parser->isOperator(tk,'-') || parser->isOperator(tk,'*') || parser->isOperator(tk,'/') || parser->isOperator(tk,'%') || parser->isOperator(tk,'*') || parser->isOperator(tk,'&') || parser->isOperator(tk,'|') || parser->isOperator(tk,'^') || parser->isOperator(tk,L"<<") || parser->isOperator(tk,L">>") || parser->isOperator(tk,L"&&") || parser->isOperator(tk,L"||") || parser->isOperator(tk,'=') || parser->isOperator(tk,'<') || parser->isOperator(tk,'>') || parser->isOperator(tk,L">=") || parser->isOperator(tk,L"<=") || parser->isOperator(tk,L"==") || parser->isOperator(tk,L"!=") || parser->isOperator(tk,L"+=") || parser->isOperator(tk,L"-=") || parser->isOperator(tk,L"*=") || parser->isOperator(tk,L"/=") || parser->isOperator(tk,L"%=") || parser->isOperator(tk,L">>=") || parser->isOperator(tk,L"<<=") || parser->isOperator(tk,L"&=") || parser->isOperator(tk,L"|=") || parser->isOperator(tk,L"^=")))
			{			

				break;
			}						
			$op = parse_binary_op(ctx,parser,tk);
			TNode* $rhs = parse_cast_expression(ctx,parser,tk);
			$result = makeBinaryExpr(ctx,$op,$result,$rhs);
		}while(true);
	}
	return $result;

}

TNode * parse_expression_or_variant_declare_stmt(Context &ctx,TParser* parser, TToken &tk)
{	
	TNodeList* $result = parser->makeList();
	TNode* $expr = parse_expression(ctx,parser,tk);
	if(parser->isOperator(tk,';'))
	{		
		Ensure(parser->isOperator(tk,';'),71,0);
		parser->nextToken(tk);
		parser->makeList($result,makeExprStmt(ctx,$expr));
	}
	else if(parser->isIdent(tk))
	{		
		do
		{			
			TNode* $init = 0 ;
			TNode* $id = parse_identifier(ctx,parser,tk);
			if(parser->isOperator(tk,'='))
			{			
				
				Ensure(parser->isOperator(tk,'='),38,0);
				parser->nextToken(tk);
				$init = parse_expression(ctx,parser,tk);
			}
			parser->makeList($result,makeVar(ctx,$id,$expr,$init));
			if(!(parser->isOperator(tk,',')))
			{			

				break;
			}
			parser->nextToken(tk);
		}while(true);
		Ensure(parser->isOperator(tk,';'),71,0);
		parser->nextToken(tk);
	}
	return $result;

}

TNode * parse_expression_stmt(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNode* $expr = parse_expression(ctx,parser,tk);
	$result = makeExprStmt(ctx,$expr);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	return $result;

}

TNode * parse_comp_stmt(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNodeList* $body = parser->makeList();
	Ensure(parser->isOperator(tk,'{'),67,0);
	parser->nextToken(tk);
	do
	{
		if(!(parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isKeyword(tk,L"this") || parser->isKeyword(tk,L"super") || parser->isOperator(tk,'(') || parser->isKeyword(tk,L"sizeof") || parser->isKeyword(tk,L"new") || parser->isKeyword(tk,L"delete") || parser->isOperator(tk,'~') || parser->isOperator(tk,'!') || parser->isOperator(tk,L"++") || parser->isOperator(tk,L"--") || parser->isOperator(tk,'+') || parser->isOperator(tk,'-') || parser->isKeyword(tk,L"cast") || parser->isKeyword(tk,L"trust") || parser->isOperator(tk,'{') || parser->isKeyword(tk,L"if") || parser->isKeyword(tk,L"return") || parser->isKeyword(tk,L"while") || parser->isKeyword(tk,L"do") || parser->isKeyword(tk,L"for") || parser->isKeyword(tk,L"break") || parser->isOperator(tk,';') || parser->isKeyword(tk,L"continue") || parser->isIdent(tk) || parser->isLiteral(tk)))
		{		

			break;
		}				
		parser->makeList($body,parse_stmt(ctx,parser,tk));
	}while(true);
	Ensure(parser->isOperator(tk,'}'),66,0);
	parser->nextToken(tk);
	$result = makeCompStmt(ctx,$body);
	return $result;

}

TNode * parse_if_stmt(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNode* $fbody = 0 ;
	Ensure(parser->isKeyword(tk,L"if"),9,L"if");
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,'('),65,0);
	parser->nextToken(tk);
	TNode* $cond = parse_cond(ctx,parser,tk);
	Ensure(parser->isOperator(tk,')'),64,0);
	parser->nextToken(tk);
	TNode* $tbody = parse_stmt(ctx,parser,tk);
	if(parser->isKeyword(tk,L"else"))
	{	
		
		Ensure(parser->isKeyword(tk,L"else"),9,L"else");
		parser->nextToken(tk);
		$fbody = parse_stmt(ctx,parser,tk);
	}
	$result = makeIf(ctx,$cond,$tbody,$fbody);
	return $result;

}

TNode * parse_cond(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	$result = parse_expression(ctx,parser,tk);
	return $result;

}

TNode * parse_return_stmt(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isKeyword(tk,L"return"),9,L"return");
	parser->nextToken(tk);
	if(parser->isKeyword(tk,L"typedef") || parser->isKeyword(tk,L"manage") || parser->isKeyword(tk,L"unsigned") || parser->isKeyword(tk,L"static") || parser->isKeyword(tk,L"virtual") || parser->isKeyword(tk,L"stdcall") || parser->isKeyword(tk,L"const") || parser->isKeyword(tk,L"byref") || parser->isKeyword(tk,L"override") || parser->isKeyword(tk,L"inline") || parser->isKeyword(tk,L"ref") || parser->isKeyword(tk,L"native") || parser->isKeyword(tk,L"int") || parser->isKeyword(tk,L"double") || parser->isKeyword(tk,L"float") || parser->isKeyword(tk,L"string") || parser->isKeyword(tk,L"bool") || parser->isKeyword(tk,L"int64") || parser->isKeyword(tk,L"short") || parser->isKeyword(tk,L"long") || parser->isKeyword(tk,L"oleobject") || parser->isKeyword(tk,L"void") || parser->isKeyword(tk,L"uint") || parser->isKeyword(tk,L"ushort") || parser->isKeyword(tk,L"char") || parser->isKeyword(tk,L"this") || parser->isKeyword(tk,L"super") || parser->isOperator(tk,'(') || parser->isKeyword(tk,L"sizeof") || parser->isKeyword(tk,L"new") || parser->isKeyword(tk,L"delete") || parser->isOperator(tk,'~') || parser->isOperator(tk,'!') || parser->isOperator(tk,L"++") || parser->isOperator(tk,L"--") || parser->isOperator(tk,'+') || parser->isOperator(tk,'-') || parser->isKeyword(tk,L"cast") || parser->isKeyword(tk,L"trust") || parser->isIdent(tk) || parser->isLiteral(tk))
	{	
		
		$result = parse_expression(ctx,parser,tk);
	}
	$result = makeReturn(ctx,$result);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	return $result;

}

TNode * parse_while_stmt(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isKeyword(tk,L"while"),9,L"while");
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,'('),65,0);
	parser->nextToken(tk);
	TNode* $cond = parse_cond(ctx,parser,tk);
	Ensure(parser->isOperator(tk,')'),64,0);
	parser->nextToken(tk);
	TNode* $stmt = parse_stmt(ctx,parser,tk);
	$result = makeWhile(ctx,$cond,$stmt);
	return $result;

}

TNode * parse_until_stmt(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isKeyword(tk,L"do"),9,L"do");
	parser->nextToken(tk);
	TNode* $stmt = parse_comp_stmt(ctx,parser,tk);
	Ensure(parser->isKeyword(tk,L"while"),9,L"while");
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,'('),65,0);
	parser->nextToken(tk);
	TNode* $cond = parse_cond(ctx,parser,tk);
	Ensure(parser->isOperator(tk,')'),64,0);
	parser->nextToken(tk);
	$result = makeUntil(ctx,$cond,$stmt);
	return $result;

}

TNode * parse_for_stmt(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isKeyword(tk,L"for"),9,L"for");
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,'('),65,0);
	parser->nextToken(tk);
	TNode* $init = parse_expression_or_variant_declare_stmt(ctx,parser,tk);
	TNode* $cond = parse_cond(ctx,parser,tk);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	TNode* $inc = parse_expression(ctx,parser,tk);
	Ensure(parser->isOperator(tk,')'),64,0);
	parser->nextToken(tk);
	TNode* $stmt = parse_stmt(ctx,parser,tk);
	$result = makeFor(ctx,$init,$cond,$inc,$stmt);
	return $result;

}

TNode * parse_break_stmt(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isKeyword(tk,L"break"),9,L"break");
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	$result = makeBreak(ctx);
	return $result;

}

TNode * parse_empty_stmt(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	return $result;

}

TNode * parse_continue_stmt(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	Ensure(parser->isKeyword(tk,L"continue"),9,L"continue");
	parser->nextToken(tk);
	Ensure(parser->isOperator(tk,';'),71,0);
	parser->nextToken(tk);
	$result = makeContinue(ctx);
	return $result;

}

TNode * parse_match_param_list(Context &ctx,TParser* parser, TToken &tk)
{	
	TNodeList* $result = parser->makeList();
	do
	{		
		parser->makeList($result,parse_match_param(ctx,parser,tk));
		if(!(parser->isOperator(tk,',')))
		{		

			break;
		}
		parser->nextToken(tk);
	}while(true);
	return $result;

}

TNode * parse_match_param(Context &ctx,TParser* parser, TToken &tk)
{	
	TNode* $result = 0 ;
	TNode* $id = 0 ;
	TNode* $type_id = 0 ;
	TNode* $child = 0 ;
	TNode* $init = 0 ;
	if(parser->isLiteral(tk))
	{		
		TNode* $r = parse_literal(ctx,parser,tk);
		$init = makeLiteral(ctx,$r);
	}
	else if(parser->isOperator(tk,'-'))
	{		
		Ensure(parser->isOperator(tk,'-'),34,0);
		$init = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,L"..."))
	{		
		Ensure(parser->isOperator(tk,L"..."),82,0);
		$init = parser->TokenNode(tk);
		parser->nextToken(tk);
	}
	else if(parser->isOperator(tk,':'))
	{		
		Ensure(parser->isOperator(tk,':'),90,0);
		parser->nextToken(tk);
		$type_id = parse_type_id(ctx,parser,tk);
		if(parser->isOperator(tk,'('))
		{		
			
			Ensure(parser->isOperator(tk,'('),65,0);
			parser->nextToken(tk);
			$child = parse_match_param_list(ctx,parser,tk);
			Ensure(parser->isOperator(tk,')'),64,0);
			parser->nextToken(tk);
		}
	}
	else if(parser->isIdent(tk))
	{		
		$id = parse_identifier(ctx,parser,tk);
		if(parser->isOperator(tk,'('))
		{		
			
			Ensure(parser->isOperator(tk,'('),65,0);
			parser->nextToken(tk);
			Ensure(parser->isOperator(tk,')'),64,0);
			parser->nextToken(tk);
		}
		if(parser->isOperator(tk,':'))
		{		
			
			Ensure(parser->isOperator(tk,':'),90,0);
			parser->nextToken(tk);
			$type_id = parse_type_id(ctx,parser,tk);
			if(parser->isOperator(tk,'('))
			{			
				
				Ensure(parser->isOperator(tk,'('),65,0);
				parser->nextToken(tk);
				$child = parse_match_param_list(ctx,parser,tk);
				Ensure(parser->isOperator(tk,')'),64,0);
				parser->nextToken(tk);
			}
		}
	}
	$result = makeParam(ctx,$id,$type_id,$init,$child);
	return $result;

}
