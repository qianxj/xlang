#include "vlpreparse.hpp"

namespace trap {
namespace complier {


VToken::Kind VToken::getOperator(wstring_t op)
{
	if(!op || !op[0]) return tkUnknown;
	
	switch(op[0]) {
	case '+':
		if(op[1]=='+') return opInc;
		if(op[1]=='=') return opAddAsn;
		return opAdd;
	case '-':
		if(op[1]=='-') return opDec;
		if(op[1]=='=') return opSubAsn;
		if(op[1]=='>') return opPointer;
		return opSub;
	case '*':
		if(op[1]=='=') return opMulAsn;
		return opMul;
	case '/':
		if(op[1]=='=') return opDivAsn;
		return opDiv;
	case '%':
		if(op[1]=='=') return opModAsn;
		return opMod;
	case '^':
		if(op[1]=='=') return opXorAsn;
		return opXor;
	case '~':
		if(op[1]=='=') return opBnotAsn;
		return opBnot;
	case ':':
		if(op[1]=='=') return opBecomes;
		if(op[1]==':') return opNest;
		return opColon;
	case '<':
		if(op[1]=='=') return opLte;
		if(op[1]=='<')
		{
			if(op[2]=='=') return opShlAsn;
			return opShl;
		}
		return opLt;
	case '>':
		if(op[1]=='=') return opGte;
		if(op[1]=='>')
		{
			if(op[2]=='=') return opShrAsn;
			return opShr;
		}
		return opGt;
	case '=':
		if(op[1]=='=') return opEq;
		if(op[1]=='>') return opArrow;
		return opAsn;
	case '|':
		if(op[1]=='=') return opBorAsn;
		if(op[1]=='|')  return opOr;
		return opBor;
	case '&':
		if(op[1]=='=') return opBandAsn;
		if(op[1]=='&')  return opAnd;
		return opBand;
	case '#':
		return opHash;
	case '$':
		return opDollar;
	case '@':
		return opSign;
	case '?':
		return opQuery;
	case '\\':
		return opBackslash;
	case '!':
		if(op[1]=='=') return opNeq;
		return opNot;
	case '(':
		return opLparen;
	case ')':
		return opRparen;
	case '[':
		return opLbrackets;
	case ']':
		return opRbrackets;
	case '{':
		return opLbrace;
	case '}':
		return opRbrace;
	case ',':
		return opComma;
	case ';':
		return opSemicolon;
	case '.':
		if(op[1]=='.')
		{
			if(op[2]=='.') return opDot3;
			return opDot2;
		}
		return opDot;
	default:
			return tkUnknown;
	}
}

const wchar_t* VToken::getOperatorString(VToken::Kind kind)
{
	switch(kind)
	{
		//math operator
		case opAdd:		return L"+"; 
		case opSub:		return L"-";
		case opMul	:	return L"*";
		case opDiv:		return L"/";
		case opMod:		return L"%";
		
		//unary
		case opAsn:		return L"=";
		case opInc:		return L"++";
		case opDec:		return L"--";
		
		//bit operator
		case opShl:		return L"<<";
		case opShr:		return L">>";
		case opBand:	return L"&";
		case opBor:		return L"|";
		case opXor:		return L"^";
		case opBnot:	return L"~"; 
		
		//relation operator
		case opOr:		return L"||";
		case opAnd:		return L"&&";
		case opGt:		return L">";
		case opGte:		return L">=";
		case opLt:		return L"<";
		case opLte:		return L"<=";
		case opEq:		return L"==";
		case opNeq:		return L"!=";
		case opNot:		return L"!";
		
		
		//group operator
		case opRparen:		return L")";
		case opLparen:		return L"(";
		case opRbrace:		return L"}";
		case opLbrace:		return L"{";
		case opRbrackets:	return L"]";
		case opLbrackets:	return L"[";
		
		case opComma:		return L",";
		case opSemicolon:	return L";";
		case opSign:		 return L"@";
		case opDollar:		 return L"$";
		
		case opApos:		return L"\"";
		case opPath2:		return L"//";  
		case opSizeof:		return L"sizeof";
		
		case opNest:		return L"::";
		case opPointer:  	return L"->";
		case opDot3:	    return L"...";
		case opDot2:	    return L"..";
		case opIn:	    	return L"oneof"; 
		case opMref:		return L"::&";
		case opMpointer:	return L"::->";
		case opVarrow:  	return L"<-";
		case opBecomes: 	return L":=";
		case opQuery:		return L"?";
		case opColon:		return L":"; 
		case opHash:		return L"#";
		case opRef:			return L".";
		case opBackslash:	return L"\\";
		
		#if 0
		//only for see
		case opExclam:  	return L"!";
		case opPercent: 	return L"%";
		case opDot:			return L".";
		case opPeriod:		return L".";
		case opArrow:    	return L"->";
		case opPath:		return L"/";
		#endif
		
		//math asn operator
		case opAddAsn:		return L"+=";
		case opSubAsn:		return L"-=";
		case opMulAsn:		return L"*=";
		case opDivAsn:		return L"/=";
		case opModAsn:		return L"%=";
		
		//bit asn operator
		case opShlAsn:		return L"<<=";
		case opShrAsn:		return L">>=";
		case opBandAsn:		return L"&=";
		case opBorAsn:		return L"|=";
		case opXorAsn:		return L"^=";
		case opBnotAsn:		return L"~+";
		default:
			return L"unknown";
	}
}

const wchar_t* VToken::getOperatorString()
{
	return VToken::getOperatorString(kind);

}

vlLexer* SParser::getLexer()
{
	return lexer;
}


SParser& SParser::setLexer(vlLexer* lexer_)
{
	lexer = lexer_;
	return *this;
}


int SParser::getTokenIdent()
{
	return getTokenIdent(getToken());
}


const wstring_t SParser::getTokenString()
{
	return getTokenString(getToken());
}


vlToken& SParser::getToken()
{
	return *(vlToken*)&((vlLexer*)lexer)->token;
}

int  SParser::nextToken(vlToken &tk, lexer_flag_t flag)
{
	tk = nextTokenEx(flag);
	return 1;
}

vlToken& SParser::nextTokenEx(lexer_flag_t flag)
{
	if(/*!((*(vlToken*)&(((vlLexer*)lexer)->token)).ty) &&*/ (*(vlToken*)&(((vlLexer*)lexer)->token)).cc >= ((vlLexer*)lexer)->sourceLen && ptr_lexer_pool->size()==0)
	{
		(*(vlToken*)&(((vlLexer*)lexer)->token)).ty = -1;
		return getToken();
	}
	((vlLexer*)lexer)->insymbol(flag);
	if((*(vlToken*)&(((vlLexer*)lexer)->token)).ty)
	{
		if(pirorSoureUrl!=soureUrl) pirorSoureUrl = soureUrl; 
	}else
	{
		while(!(*(vlToken*)&(((vlLexer*)lexer)->token)).ty)
		{
			if(ptr_lexer_pool->size()>0)
			{
				//this->get_context()->get_runtime()->FreeSource((wchar_t *)lexer_->source);
				pirorSoureUrl = ((vlLexer*)lexer)->srcPosition.srcUrl?
					((vlLexer*)lexer)->srcPosition.srcUrl:L"";
				((vlLexer*)lexer)->srcPosition.srcUrl = 0;
				delete ((vlToken*)lexer);
				lexer = ptr_lexer_pool->top();
				ptr_lexer_pool->pop();
				lexer->insymbol();
				soureUrl = ((vlLexer*)lexer)->srcPosition.srcUrl?
					((vlLexer*)lexer)->srcPosition.srcUrl:L"";
			}else
				break;
		}
	}


	if(lexer->token.ty ==tokenType::tString ||
		lexer->token.ty == tokenType::tIdent)
	{
		bool drop = false;
		if(lexer->token.ty == tokenType::tString)
		{
			lexer->token.snum++; //skip "
			if(lexer->token.inum==1) //escape
			{
				wchar_t* str = this->token_str;
				if(lexer->token.slen >= 255)
				{
					str = new wchar_t[lexer->token.slen + 1];
					drop = true;
				}
				const wchar_t* pc = lexer->token.snum;
				const wchar_t* end = pc + lexer->token.slen;
				wchar_t* pc1 = str;
				while(pc < end)
				{
					if(*pc=='\\')
					{
						pc++;
						if(*pc=='\'') 
							*pc1++='\'';
						else if(*pc=='"')
							*pc1++='\"';
						else if(*pc=='r')
							*pc1++='\r';
						else if(*pc=='n')
							*pc1++='\n';
						else if(*pc=='t')
							*pc1++='\t';
						else if(*pc=='\\')
							*pc1++='\\';
						else
						{
							*pc1++ ='\\';	
							pc--;
						}
						pc++;
					}else
						*pc1++ = *pc++;
				}
				lexer->token.snum = str;
				lexer->token.slen = (int)(pc1 - str);
			}
		}
		if(lexer->token.slen)
			lexer->token.inum = util::hashString((const wstring_t)lexer->token.snum,lexer->token.slen);
		else
			lexer->token.inum = util::hashString(L"");
		if(drop) delete lexer->token.snum;
		lexer->token.snum = util::getHashString(lexer->token.inum);
	}
	return getToken();
}


bool SParser::skipLine()
{
	while(!matchEof(getToken()) && !matchOperate(getToken(),opValue::semicolon))
		nextTokenEx();
	if(matchOperate(getToken(),opValue::semicolon))
		nextTokenEx();
	return true;
}

bool SParser::skipLine(vlToken & tk)
{
	while(!matchEof(getToken()) && !matchOperate(getToken(),opValue::semicolon))
		nextToken(tk);
	if(matchOperate(getToken(),opValue::semicolon))
		nextToken(tk);
	return true;
}



int  SParser::output(wstring_t str)
{
	setlocale( LC_ALL, "" );
	printf("%S",str);
	return 1;
}


int  SParser::format_output(wstring_t format,...)
{
	wchar_t str[40960];
	va_list args;
	va_start(args, format);
	int len = vswprintf_s(str,40960,format,args);
	va_end(args);

	if(get_err_ptr())
		get_err_ptr()->output(0,str);
	else
		os_t::print(L"%s",str);

	return 1;
}


xlang::wErr * SParser::get_err_ptr()
{
	return werr;
}


void SParser::set_err_ptr(xlang::wErr * err)
{
	werr = err;
}


void SParser::error(error::err nError, const wstring_t str, const vlToken& tk)
{
	const wchar_t * txt = error::geterrtext(nError);
	if(get_err_ptr())
	{
		setlocale( LC_ALL, "" );
		wchar_t buf[255];
		swprintf_s(buf,255,txt,str);
		get_err_ptr()->adderror(((vlLexer*)lexer)->srcPosition.srcUrl,(*(vlToken*)&tk).start.row + 1,
			(*(vlToken*)&tk).start.col + 1,nError,buf,0,0);
	}else
	{
		setlocale( LC_ALL, "" );
		wprintf(L"\n");
		wprintf(L"%s(%d %d):error C%4d:",((vlLexer*)lexer)->srcPosition.srcUrl,
			(*(vlToken*)&tk).start.row + 1,(*(vlToken*)&tk).start.col + 1,nError);
		wprintf(txt,str);
	}
}


int	SParser::getTokenIdent(vlToken& tk)
{
	return (*(vlToken*)&tk).inum;
}


 const wstring_t	SParser::getTokenString(vlToken& tk)
{
	return (const wstring_t)(*(vlToken*)&tk).snum;
}

//util

bool SParser::ensurseToken(wstring_t kw,error::err  err,wchar_t* tk,bool skip_line)
{
	if(!matchKeyword(getToken(),kw))
	{
		error(err,tk,getToken());
		return false;
	}
	return true;
}

bool SParser::ensurseToken(wstring_t kw,error::err  err,bool skip_line)
{
	if(!matchKeyword(getToken(),kw))
	{
		error(err,0,getToken());
		return false;
	}
	return true;
}

bool SParser::ensurseOperator(int op,error::err  err,bool skip_line)
{
	if(!matchOperate(getToken(),op))
	{
		error(err,0,getToken());
		return false;
	}
	return true;
}

bool SParser::ensurseOperator(int op,error::err  err,wchar_t* tk, bool skip_line)
{
	if(!matchOperate(getToken(),op))
	{
		error(err,tk,getToken());
		return false;
	}
	return true;
}

bool SParser::ensurseIdent(error::err  err,bool skip_line)
{
	if(!matchIdent(getToken()))
	{
		error(err,0,getToken());
		return false;
	}
	return true;
}

bool SParser::ensurseString(error::err  err,bool skip_line)
{
	if(!matchString(getToken()))
	{
		error(err,0,getToken());
		return false;
	}
	return true;
}

bool SParser::ensurseNumber(error::err  err,bool skip_line)
{
	if(!matchNumber(getToken()))
	{
		error(err,0,getToken());
		return false;
	}
	return true;
}


bool SParser::matchChar(wchar_t ch)
{
	return matchChar(getToken(),ch);
}


bool SParser::matchBool()
{
	return matchBool(getToken());
}	


bool SParser::matchChar(vlToken &tk,wchar_t ch)
{
	if(!match(tk,tokenType::tChar)) return false;
	return (*(vlToken*)&tk).snum[0]== ch;
}


bool SParser::match(int ty)
{
	return match(getToken(),ty);
}


bool SParser::matchEof()
{
	return matchEof(getToken());
}


bool SParser::matchIdent()
{
	return matchIdent(getToken());
}


bool SParser::matchString()
{
	return matchString(getToken());
}


bool SParser::matchChar()
{
	return matchChar(getToken());
}


bool SParser::matchNumber(int v)
{
	return matchNumber(getToken()) && (*(vlToken*)&getToken()).inum == v;
}


bool SParser::matchNumber(double v)
{
	return matchNumber(getToken()) && (*(vlToken*)&getToken()).dnum == v;
}


bool SParser::matchInteger()
{
	return matchInteger(getToken());
}


bool SParser::matchNumber()
{
	return matchNumber(getToken());
}


bool SParser::matchOperate()
{
	return matchOperate(getToken());
}


bool SParser::matchOperate(int op)
{
	return matchOperate(getToken(),op);
}


bool SParser::matchKeyword()
{
	return matchKeyword(getToken());
}


bool SParser::matchKeyword(const wchar_t* kw)
{
	return matchKeyword(getToken(),kw);
}


bool SParser::matchiKeyword(const wchar_t* kw)
{
	return matchiKeyword(getToken(),kw);
}



bool SParser::match(vlToken &tk, int ty)
{
	return (*(vlToken*)&tk).ty ==ty ? true : false;
}

bool SParser::matchEof(vlToken &tk)
{
	 return (*(vlToken*)&tk).ty && (*(vlToken*)&tk).ty!=-1? false:true; 
}

bool SParser::matchIdent(vlToken &tk)
{
	/*if(tk.ty==tokenType::tIdent)
	{
		if(wcsncmp(tk.snum,L"return",tk.slen)==0) return false;
		if(wcsncmp(tk.snum,L"new",tk.slen)==0) return false;
		if(wcsncmp(tk.snum,L"delete",tk.slen)==0) return false;
	}*/
	 return match(tk,tokenType::tIdent);
}

bool SParser::matchString(vlToken &tk)
{
	 return match(tk,tokenType::tString);
}


bool SParser::matchChar(vlToken &tk)
{
	return match(tk,tokenType::tChar);
}


bool SParser::matchBool(vlToken &tk)
{
	return match(tk,tokenType::tBool);
}


bool SParser::matchNumber(vlToken &tk)
{
	 return match(tk,tokenType::tNumber)||
		   match(tk,tokenType::tInt)||
		   match(tk,tokenType::tDouble);
}


bool SParser::matchInteger(vlToken &tk)
{
	 return  match(tk,tokenType::tInt);
}


bool SParser::matchOperate(vlToken &tk)
{
 	return match(tk,tokenType::tOp);
}

bool SParser::matchOperate(vlToken &tk,int op)
{
	if(matchOperate(tk) && (*(vlToken*)&tk).inum==op)return true;
	return false;
}

bool SParser::matchKeyword(vlToken &tk)
{
	return match(tk,tokenType::tIdent);
}

bool SParser::matchKeyword(vlToken &tk,const wchar_t* kw)
{
	int nlen = (int)wcslen(kw);
	if((*(vlToken*)&tk).ty == tokenType::tIdent && 
		nlen == (*(vlToken*)&tk).slen &&
		wcsncmp((*(vlToken*)&tk).snum,kw,nlen)==0)return true;
	return false;
}

bool SParser::matchiKeyword(vlToken &tk,const wchar_t* kw)
{
	int nlen = (int)wcslen(kw);
	if((*(vlToken*)&tk).ty == tokenType::tIdent && 
		nlen == (*(vlToken*)&tk).slen &&
		_wcsnicmp((*(vlToken*)&tk).snum,kw,nlen)==0)return true;
	return false;
}


}} //namespace trap::complier