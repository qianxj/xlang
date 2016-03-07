#include <assert.h>
#include "termeval.hpp"
#include "termanalyse.hpp"
namespace trap {
namespace complier {

std::wstring eval(LangContext & context, coder::astNode * node)
{
	return eval(context,node,L"");
}

std::wstring eval(LangContext & context, coder::astNode * node,std::wstring space)
{
	std::wstring str;
	if(!node) return str;
	switch(node->kind)
	{
	case coder::kNode:
	case coder::kContext:
	case coder::kFunction:
	case coder::kParam:
		break;
	case coder::kVariant:
		{
			coder::astVariant * term = (coder::astVariant*)node;
			coder::astTerm * tm = (coder::astTerm *)term->type;
			str += (std::wstring)tm->name + L" " + term->name ;
			if(term->init)
				str += L" = " + eval(context,term->init);
			else
				str += L" = 0 ";
		}
		break;
	case coder::kType:
	case coder::kClass:
	case coder::kStmt:
		break;
	case coder::kBranch:
		{
			coder::astBranch* branch = (coder::astBranch*)node;
			bool bswitch = true;
			if(branch->expr)
				str = str+ space + L"switch("+eval(context,((coder::astBranch*)node)->expr)+L"){";
			else
				bswitch = false;

			coder::astList * list = (coder::astList *)branch->branchItemList;
			bool start = true;
			while(list)
			{
				coder::astBranchItem* branchItem = (coder::astBranchItem*)list->term;
				if(branchItem)
				{
					if(bswitch)
					{
						str += L"\r\n" + space + L"case "+ eval(context,branchItem->cond) +L":";
					}else
					{
						if(start)
							str += L"if("+ eval(context,branchItem->cond) +L")";
						else
							str += L"\r\n" +space+L"else if("+ eval(context,branchItem->cond) +L")";
					}
					str += L"\r\n"+space+L"{" + eval(context,branchItem->stmt,space+L"\t") + L"\r\n"+space+L"}";
					if(start) start = false;
				}
				list = list->next;
			}
			if(branch->defaultStmt)
			{
				str += L"\r\n"+space+ L"else" +  L"\r\n"+space+L"{";
				str += eval(context,branch->defaultStmt,space+L"\t");
				str += L"\r\n"+space + L"}";
			}
			if(bswitch)str = str + space + L"}\r\n";
		}
		break;
	case coder::kReturn:
		{
			coder::astReturn* ret = (coder::astReturn*)node;
			str += L"return " + eval(context,ret->expr) +L";\r\n";
		}
		break;
	case coder::kList:
		{
			//only for stmt
			coder::astList * list = (coder::astList *)node;
			while(list)
			{
				if(list->term)
				{
					if(list->term->kind==coder::kList)
						str = str + space + eval(context,list->term,space);
					else
					{
						str = str + L"\r\n" + space + eval(context,list->term,space);
						if(list->term->kind!=coder::kIf &&
							list->term->kind!=coder::kUntil &&
							list->term->kind!=coder::kLoop &&
							list->term->kind!=coder::kBranch &&
							list->term->kind!=coder::kReturn &&
							list->term->kind!=coder::kContinue &&
							list->term->kind!=coder::kBreak &&
							list->term->kind!=coder::kCompStmt
							) str+=L";";
					}
				}
				list = list->next;
			}
		}
		break;
	case coder::kExpr:
		assert(false);
		break;
	case coder::kBranchItem:
		assert(false);
		break;
	case coder::kCall:
		{
			coder::astCall * term = (coder::astCall *)node;
			str += (std::wstring)term->name + L"(";
			coder::astList * list = (coder::astList *)term->args;
			if(term->args)
			{	
				if(term->args->kind != coder::kList)
				{
					str += eval(context, term->args);
				}else
				{
					bool start = true;
					while(list)
					{
						if(start) start = false; else str += L",";
						if(list->term)str = str + eval(context,list->term);
						list = list->next;
					}
				}
			}
			str += L")";
		}
		break;
	case coder::kLiteral:
		{
			coder::astLiteral * term = (coder::astLiteral *)node;
			wchar_t buf[48];
			if(term->literalKind==coder::lkString)
				str += (std::wstring)L"L\"" + term->val.str + L"\"";
			else if(term->literalKind==coder::lkDouble)
			{
				wsprintf(buf,L"%f",term->val.d64);
				str += buf;
			}
			else if(term->literalKind==coder::lkFloat)
			{
				wsprintf(buf,L"%f",term->val.d32);
				str += buf;
			}
			else if(term->literalKind==coder::lkChar)
			{
				wsprintf(buf,L"'%c'",term->val.c16);
				str += buf;
			}
			else if(term->literalKind==coder::lkByte)
			{
				wsprintf(buf,L"'%C'",term->val.c8);
				str += buf;
			}
			else if(term->literalKind==coder::lkInt)
			{
				wsprintf(buf,L"%d",term->val.i32);
				str += buf;
			}
			else if(term->literalKind==coder::lkBool)
			{
				if(term->val.b)
					str += L"true";
				else
					str += L"false";
			}else
				assert(false);
		}
		break;
	case coder::kLoop:
		{
			coder::astLoop * term = (coder::astLoop *)node;
			str += L"while(" + eval(context,term->cond) +L")\r\n"+space+L"{";
			str += eval(context,term->stmt,space+L"\t");
			str += L"\r\n"+space+L"}";
		}
		break;
	case coder::kUntil:
		{
			coder::astUntil * term = (coder::astUntil *)node;
			str += L"do\r\n"+space+L"{";
			str += eval(context,term->stmt,space+L"\t");
			if(!term->cond)
				str += (std::wstring)L"\r\n"+space+L"}while(true);";
			else
				str += (std::wstring)L"\r\n" + space + L"}while(" + eval(context,term->cond)+L");";
		}
		break;
	case coder::kBreak:
		{
			coder::astBreak * term = (coder::astBreak *)node;
			str += L"break;";
		}
		break;
	case coder::kContinue:
		{
			coder::astContinue * term = (coder::astContinue *)node;
			str += L"continue;";
		}
		break;
	case coder::kIf:
		{
			coder::astIf * term = (coder::astIf *)node;
			str += L"if(";
			str += eval(context,term->expr);
			str += L")";
			if(term->trueStmt->kind!=coder::kList)
			{
				str += eval(context,term->trueStmt);
			}else
			{
				str+=L"\r\n"+space;
				str +=L"{"+space+L"\r\n";
				str += eval(context,term->trueStmt,space+L"\t");
				str += L"\r\n"+space+L"}";
			}

			if(term->falseStmt)
			{
				if(term->falseStmt->kind!=coder::kList)
				{
					str +=L"\r\nelse\r\n"+space + eval(context,term->falseStmt);
				}else
				{
					str +=L"\r\n"+space;
					str +=L"\r\nelse\r\n"+space+L"{"+space+L"\r\n";
					str += eval(context,term->falseStmt,space+L"\t");
					str += L"\r\n"+space+L"}";
				}
			}
		}
		break;
	case coder::kMatch:
		{
			coder::astMatch * term = (coder::astMatch *)node;
			str += L"match(";
			str += eval(context,term->term);
			str += L",";
			str += eval(context,term->temp);
			str += L")";
		}
		break;
	case coder::kOneof:
		{
			coder::astOneof * term = (coder::astOneof *)node;
			str += L"oneof(";
			coder::astList * list = (coder::astList *)term->nodes;
			bool start = true;
			while(list)
			{
				if(list->term)
				{
					if(start) start = false; else str += L",";
					str = str + eval(context,list->term);
				}
				list = list->next;
			}
			str += L")";
		}
		break;
	case coder::kCompStmt:
		{
			coder::astCompStmt * term = (coder::astCompStmt*)node;
			str +=  L"{\r\n" + eval(context,term->stmt,space+L"\t") +L"\r\n"+space+L"}";
		}
		break;
	case coder::kRef:
		{
			coder::astRef * term = (coder::astRef*)node;
			str += (std::wstring)L" &" + eval(context, term->node);
		}
		break;
	case coder::kBinary:
		{
			coder::astBinary* binary = (coder::astBinary*)node;
			switch(binary->opKind)
			{
			case opAdd:
			case opSub:
			case opMul:
			case opDiv:
				assert(false);
				break;
			case opAnd:
				str += eval(context,binary->lhs) +L" && "+ eval(context,binary->rhs);
				break;
			case opOr:
				str += eval(context,binary->lhs) +L" || "+ eval(context,binary->rhs);
				break;
			case opAsn:
				str += eval(context,binary->lhs) +L" = "+ eval(context,binary->rhs);
				break;
			default:
				assert(false);
				break;
			}
		}
		break;
	case coder::kUnary:
		{
			coder::astUnary * unary = (coder::astUnary *)node;
			switch(unary->opKind)
			{
			case opNot:
				str += (std::wstring)L"!(" + eval(context,unary->node) +L")";
				break;
			default:
				assert(false);
				break;
			}
		}
		break;
	case coder::kDec:
	case coder::kInc:
		break;
	case coder::kTerm:
		{
			coder::astTerm * term = (coder::astTerm *)node;
			str += term->name;
		}
		break;
	default:
		assert(false);
	}
	return str;
}

std::wstring eval(LangContext & context,int term)
{
	coder::astNode * stmt = context.cTerm[term];
	if(!stmt) return L"";
	
	std::wstring fct;
	fct = fct + L"\r\nTNode * parse_"+ getTermName(context,term)+L"(Context &ctx,TParser* parser, TToken &tk)\r\n{";
	fct += eval(context, stmt,L"\t");
	fct +=L"\r\n}\r\n";
	return fct;
}

std::wstring eval(LangContext & context)
{
	std::wstring str;
	str += L"#include \"vlpreparse.hpp\"\r\n";
	str += L"\r\n";
	str += L"using namespace trap::complier;";
	str += L"\r\n";

	for(int i=0;i<(int)context.cTerm.size();i++)
	{
		coder::astNode * stmt = context.cTerm[i];
		if(!stmt) continue;
		str += L"TNode* parse_"+ getTermName(context,i)+L"(Context &ctx,TParser* parser, TToken &tk);\r\n";
	}

	for(int i=0;i<(int)context.cTerm.size();i++)
	{
		int index = context.sureTerm[i];
		if(!index) continue;
		str += L"bool sure_"+ getTermName(context,i)+L"(Context &ctx,TParser* parser, TToken &tk, int pickIndex = 0);\r\n";
	}

	for(int i=0;i<(int)context.cTerm.size();i++)
	{
		str += eval(context,i);
	}

	for(int i=0;i<(int)context.cTerm.size();i++)
	{
		int pos = context.sureTerm[i];
		if(!pos) continue;
		str = str + L"\r\nbool sure_"+ getTermName(context,i)+L"(Context &ctx,TParser* parser, TToken &tk)\r\n{";
		str += eval(context,sureSyntax(context,-1,pos),L"\t");
		str +=L"\r\n}\r\n";
	}
	return str;
}


}//complier
}//trap
