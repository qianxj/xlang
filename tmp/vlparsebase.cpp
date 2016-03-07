#include "vlParser.hpp"
#include "vlUtils.hpp"
#include "vlPlatform.hpp"
#include "vlAstNode.hpp"
#include "vlAstAxis.hpp"
#include "vlAstFactory.hpp"
#include <vector>

//using namespace vord;
using namespace xlang;

vlParser::vlParser():ptr_lexer_pool(new std::stack<vlLexer *>),
		lexer(new vlLexer),werr(0)
{
}


vlParser::~vlParser()
{
	delete lexer;
	delete ptr_lexer_pool;
}

void * vlParser::parse(vlContext* context,wstring_t src,wstring_t url)
{
	lexer->setSource(src,url);
	lexer->nextch();
	nextToken();
	parse_xlang_translation_unit(context);
	return 0;
}


vlLexer* vlParser::getLexer()
{
	return lexer;
}


vlParser& vlParser::setLexer(vlLexer* lexer_)
{
	lexer = lexer_;
	return *this;
}


int vlParser::getTokenIdent()
{
	return getTokenIdent(getToken());
}


const wstring_t vlParser::getTokenString()
{
	return getTokenString(getToken());
}


vlToken& vlParser::getToken()
{
	return *(vlToken*)&((vlLexer*)lexer)->token;
}


vlToken& vlParser::nextToken(lexer_flag_t flag)
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


bool vlParser::skipLine()
{
	while(!matchEof(getToken()) && !matchOperate(getToken(),opValue::semicolon))
		nextToken();
	if(matchOperate(getToken(),opValue::semicolon))
		nextToken();
	return true;
}


int  vlParser::output(wstring_t str)
{
	setlocale( LC_ALL, "" );
	printf("%S",str);
	return 1;
}


int  vlParser::format_output(wstring_t format,...)
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


xlang::wErr * vlParser::get_err_ptr()
{
	return werr;
}


void vlParser::set_err_ptr(xlang::wErr * err)
{
	werr = err;
}


void vlParser::error(error::err nError, const wstring_t str, const vlToken& tk)
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


int	vlParser::getTokenIdent(vlToken& tk)
{
	return (*(vlToken*)&tk).inum;
}


 const wstring_t	vlParser::getTokenString(vlToken& tk)
{
	return (const wstring_t)(*(vlToken*)&tk).snum;
}

//util

bool vlParser::ensurseToken(wstring_t kw,error::err  err,wchar_t* tk,bool skip_line)
{
	if(!matchKeyword(getToken(),kw))
	{
		error(err,tk,getToken());
		return false;
	}
	return true;
}

bool vlParser::ensurseToken(wstring_t kw,error::err  err,bool skip_line)
{
	if(!matchKeyword(getToken(),kw))
	{
		error(err,0,getToken());
		return false;
	}
	return true;
}

bool vlParser::ensurseOperator(int op,error::err  err,bool skip_line)
{
	if(!matchOperate(getToken(),op))
	{
		error(err,0,getToken());
		return false;
	}
	return true;
}

bool vlParser::ensurseOperator(int op,error::err  err,wchar_t* tk, bool skip_line)
{
	if(!matchOperate(getToken(),op))
	{
		error(err,tk,getToken());
		return false;
	}
	return true;
}

bool vlParser::ensurseIdent(error::err  err,bool skip_line)
{
	if(!matchIdent(getToken()))
	{
		error(err,0,getToken());
		return false;
	}
	return true;
}

bool vlParser::ensurseString(error::err  err,bool skip_line)
{
	if(!matchString(getToken()))
	{
		error(err,0,getToken());
		return false;
	}
	return true;
}

bool vlParser::ensurseNumber(error::err  err,bool skip_line)
{
	if(!matchNumber(getToken()))
	{
		error(err,0,getToken());
		return false;
	}
	return true;
}


bool vlParser::matchChar(wchar_t ch)
{
	return matchChar(getToken(),ch);
}


bool vlParser::matchBool()
{
	return matchBool(getToken());
}	


bool vlParser::matchChar(vlToken &tk,wchar_t ch)
{
	if(!match(tk,tokenType::tChar)) return false;
	return (*(vlToken*)&tk).snum[0]== ch;
}


bool vlParser::match(int ty)
{
	return match(getToken(),ty);
}


bool vlParser::matchEof()
{
	return matchEof(getToken());
}


bool vlParser::matchIdent()
{
	return matchIdent(getToken());
}


bool vlParser::matchString()
{
	return matchString(getToken());
}


bool vlParser::matchChar()
{
	return matchChar(getToken());
}


bool vlParser::matchNumber(int v)
{
	return matchNumber(getToken()) && (*(vlToken*)&getToken()).inum == v;
}


bool vlParser::matchNumber(double v)
{
	return matchNumber(getToken()) && (*(vlToken*)&getToken()).dnum == v;
}


bool vlParser::matchInteger()
{
	return matchInteger(getToken());
}


bool vlParser::matchNumber()
{
	return matchNumber(getToken());
}


bool vlParser::matchOperate()
{
	return matchOperate(getToken());
}


bool vlParser::matchOperate(int op)
{
	return matchOperate(getToken(),op);
}


bool vlParser::matchKeyword()
{
	return matchKeyword(getToken());
}


bool vlParser::matchKeyword(wchar_t* kw)
{
	return matchKeyword(getToken(),kw);
}


bool vlParser::matchiKeyword(wchar_t* kw)
{
	return matchiKeyword(getToken(),kw);
}



bool vlParser::match(vlToken &tk, int ty)
{
	return (*(vlToken*)&tk).ty ==ty ? true : false;
}

bool vlParser::matchEof(vlToken &tk)
{
	 return (*(vlToken*)&tk).ty && (*(vlToken*)&tk).ty!=-1? false:true; 
}

bool vlParser::matchIdent(vlToken &tk)
{
	 return match(tk,tokenType::tIdent);
}

bool vlParser::matchString(vlToken &tk)
{
	 return match(tk,tokenType::tString);
}


bool vlParser::matchChar(vlToken &tk)
{
	return match(tk,tokenType::tChar);
}


bool vlParser::matchBool(vlToken &tk)
{
	return match(tk,tokenType::tBool);
}


bool vlParser::matchNumber(vlToken &tk)
{
	 return match(tk,tokenType::tNumber)||
		   match(tk,tokenType::tInt)||
		   match(tk,tokenType::tDouble);
}


bool vlParser::matchInteger(vlToken &tk)
{
	 return  match(tk,tokenType::tInt);
}


bool vlParser::matchOperate(vlToken &tk)
{
 	return match(tk,tokenType::tOp);
}

bool vlParser::matchOperate(vlToken &tk,int op)
{
	if(matchOperate(tk) && (*(vlToken*)&tk).inum==op)return true;
	return false;
}

bool vlParser::matchKeyword(vlToken &tk)
{
	return match(tk,tokenType::tIdent);
}

bool vlParser::matchKeyword(vlToken &tk,wchar_t* kw)
{
	int nlen = (int)wcslen(kw);
	if((*(vlToken*)&tk).ty == tokenType::tIdent && 
		nlen == (*(vlToken*)&tk).slen &&
		wcsncmp((*(vlToken*)&tk).snum,kw,nlen)==0)return true;
	return false;
}

bool vlParser::matchiKeyword(vlToken &tk,wchar_t* kw)
{
	int nlen = (int)wcslen(kw);
	if((*(vlToken*)&tk).ty == tokenType::tIdent && 
		nlen == (*(vlToken*)&tk).slen &&
		_wcsnicmp((*(vlToken*)&tk).snum,kw,nlen)==0)return true;
	return false;
}
