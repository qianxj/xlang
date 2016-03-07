#ifndef WPP5_LEXER_H
#define WPP5_LEXER_H

#include "vlerrors.hpp"
#include "vlwerr.hpp"
#include "vllexer.hpp"
using namespace xlang;
using namespace xlang;

vlLexer::vlLexer()
{
	source = 0;
	lexerState = 0;
	memset(&token,0,sizeof(token));
	memset(&srcPosition,0,sizeof(srcPosition));
	token.cc = -1;
}


vlLexer::~vlLexer() 
{
	if(source)delete source;
	if(srcPosition.srcUrl)delete srcPosition.srcUrl;
}


wchar_t vlLexer::nextch()
{
	//this is sholud end code source
	if(token.cc == sourceLen)  return 0; 

	token.cc ++;
	if(token.ch == '\n') //if char is new row then inc row and let col to zero 
	{
		srcPosition.curRow++;
		srcPosition.curCol = 1;
	} 

	token.ch = source[token.cc];
	if(token.ch == '\r')     //if '\r\n' then mergin to '\n'
	{
		if(source[token.cc + 1] == '\n') token.cc++;   //pos shift right
		token.ch = '\n';
	}
	srcPosition.curCol ++;

	token.row = srcPosition.curRow;
	token.col = srcPosition.curCol;
	return   token.ch;
}


int vlLexer::setSource(const wstring_t pSrc,wstring_t pUrl)
{
	if(source && pSrc != source) delete source;
	if(srcPosition.srcUrl)delete srcPosition.srcUrl;
	if(pUrl)
		srcPosition.srcUrl =(const wstring_t)_wcsdup(pUrl);
	else 
		srcPosition.srcUrl = 0;
	source = pSrc;
	sourceLen = (int)wcslen(pSrc);
	token.ch = pSrc[0];

	token.cc = -1;
	srcPosition.curRow = 0;
	srcPosition.curCol = 0;

	return 1;
}


int vlLexer::insymbol(lexer_flag_t state)
{
	//mergin vlexer state
	state |= lexerState;    

	token.start.row = srcPosition.curRow;
	token.start.col = srcPosition.curCol;
	token.start.len = token.slen;

	//have meet eof?
	if(token.ch=='\0')
	{
		token.ty = 0;
		return 1;
	}

	//initial token
	token.ty = 0 ;
	token.snum = 0;
	//token.sindex = 0;       //start position index

	//read xml data
	if(state & lexerFlag::tXmltext)
	{
		token.pcc = token.cc;
		token.snum = source + (token.pcc < 0 ? 0 : token.pcc);
		token.nlen = 0;
		token.row = srcPosition.curRow;
		token.col = token.pcc;
		while(token.ch!='<' && token.ch) nextch();
		token.slen = token.cc  -  token.pcc; 
		token.ty = tokenType::tString;
		return 1;
	}

	do
	{
		//process blank letter
		if((token.ch=='\t' ||  token.ch==' ' || token.ch=='\n')){
			//when #cp.state.ln is set when meet  return #cp.token.ln
			//when #cp.state.blank is set eat all space letter then return #cp.token.ln 
			//otherwise  skip blank letter
			while((token.ch=='\t' ||  token.ch==' ' || token.ch=='\n'))
			 {
				 //meet ln and #cp.state.ln is set to state then return #cp.token.ln 
				 if(state & lexerFlag::tLn && token.ch=='\n')
				 {
					 token.ty = tokenType::tLn;
					 nextch();
					 break;
				 }

				 //if #cp.state.blank is set and meet is not blank letter then return #cp.token.blank
				 nextch();
				 if(state & lexerFlag::tBlank && !((token.ch=='\t' ||  token.ch==' ' || token.ch=='\n')))
				 {
					 token.ty = tokenType::tBlank;
					 break;
				 }
			 }
			if(token.ty != 0 ) return 1;
		};

		//process comment
		int pos = token.cc;
		if(state & lexerFlag::tXPath || state & lexerFlag::tXQuery)
		{
			if(token.ch=='(') {
				save();
				nextch();
				if(token.ch==':')
				{
					int ch = 0;
					nextch();
					while(!(ch==':' && token.ch==')') && token.ch!='\0') ch= token.ch, nextch();
					if(token.ch!='\0')
						nextch();
					else
						error(error::err_needcommenttrail);
					pop();
					if(state &lexerFlag::tComment)
					{
						token.ty = tokenType::tBlockComment;
						return 1;
					}
				}else
					restore();
			};
			if(!((token.ch=='\t' ||  token.ch==' ' || token.ch=='\n') || (token.ch=='('  && pos!=token.cc)))break;
		}else
		{
			if(token.ch=='/') {
				save();

				nextch();
				if(token.ch=='/')
				{
					nextch();
					while(token.ch!='\n' && token.ch!='\0')nextch();  
					if(token.ch!='\0' && !(state & lexerFlag::tLn)) nextch();
					pop();
					if(state &lexerFlag::tComment)
					{
						token.ty = tokenType::tComment;
						return 1;
					}
				}else if(token.ch=='*')
				{
					int ch = 0;
					nextch();
					while(!(ch=='*' && token.ch=='/') && token.ch!='\0') ch= token.ch, nextch();
					if(token.ch!='\0')
						nextch();
					else
						error(error::err_needcommenttrail);
					pop();
					if(state &lexerFlag::tComment)
					{
						token.ty = tokenType::tBlockComment;
						return 1;
					}
				}else
					restore();
			};
			if(!((token.ch=='\t' ||  token.ch==' ' || token.ch=='\n') || (token.ch=='/'  && pos!=token.cc)))break;
		}
	}while(true);

	//recorder start cc;
	token.pcc = token.cc;
	token.snum = source + (token.pcc < 0 ? 0 : token.pcc);
	token.nlen = 0;
	token.start.row = srcPosition.curRow;
	token.start.col = srcPosition.curCol;

	//process number
	if((token.ch >='0' && token.ch <='9')) {
		token.inum = token.ch - '0';
		token.ty = tokenType::tInt;

		nextch();
		if(token.ch == 'x' || token.ch=='X')     //process hex number
		{
			int cnt =0;
			nextch();
			do 
			{
				if(token.ch>='a' && token.ch<='f')
					token.inum  = (int)((unsigned int)(token.inum *16) + (unsigned int)(token.ch - 'a'+ 10));
				else if(token.ch>='A' && token.ch<='F')
					token.inum  = (int)((unsigned int)(token.inum *16) + (unsigned int)(token.ch - 'A'+ 10));
				else if(token.ch>='0' && token.ch<='9')
					token.inum  = (int)((unsigned int)(token.inum *16) + (unsigned int)( token.ch - '0'));
				else
					break;
				cnt++;
				nextch();
			} while(cnt<=8); 
		} else
		{
			//process int part
			while(token.ch>='0' && token.ch<='9')
			{
				token.inum  = token.inum *10 + token.ch - '0';
				nextch();
			}
			//process scale part
			if(token.ch=='.')
			{
				int pos =  token.cc + 1;
				while(source[pos]==' '|| source[pos]=='\t'||source[pos]=='\r'||source[pos]=='\n')pos++;
				int ch = source[pos];
				if(!(ch=='_'||(ch>='a' && ch<='z')||(ch>='A' && ch<='Z')))
				{
					int e;
					nextch();
					if(token.ch=='.')
						token.ch=':';       //this is should number range
					else
					{
						token.ty = tokenType::tDouble;
						token.dnum = (double)token.inum;
						e=0;
						while (token.ch>='0' && token.ch<='9')
						{
							e= e - 1;
							token.dnum = token.dnum*10 + token.ch - '0';
							nextch();
						}
						if(e==0) error(error::err_scannumber);
						if(token.ch=='e' ||token.ch=='E'){
							int sign = 1;
							int s = 0;
							nextch();
							if(token.ch=='+')
							{
								sign = 1;
								nextch();
							}
							else if(token.ch=='-')
							{
								sign = -1;
								nextch();
							}
							if(token.ch <'0'|| token.ch >'9')
								error(error::err_doublescale);
							do
							{
								s = s*10 + token.ch -'0';
							}while(token.ch>='0' && token.ch<='9');

							e = sign*s;
						};
						if(e !=0){
							int s = e>0?e:-e;
							if(e>0)
								for(int i=1;i<=s;i++)token.dnum *= 10;

							else
								for(int i=1;i<=s;i++)token.dnum /= 10;
						};
					}
					if(token.ch=='e'||token.ch=='E')
					{
						token.ty= tokenType::tDouble;
						token.dnum = (double)token.inum;
						e = 0;
						{
							int sign = 1;
							int s = 0;
							nextch();
							if(token.ch=='+')
							{
								sign = 1;
								nextch();
							}
							else if(token.ch=='-')
							{
								sign = -1;
								nextch();
							}
							if(token.ch <'0'|| token.ch >'9')
								error(error::err_doublescale);
							do
							{
								s = s*10 + token.ch -'0';
							}while(token.ch>='0' && token.ch<='9');

							e = sign*s;
						};
						if(e != 0){
							int s = e>0?e:-e;
							if(e>0)
								for(int i=1;i<=s;i++)token.dnum *= 10;

							else
								for(int i=1;i<=s;i++)token.dnum /= 10;
						};
					}
				}
			}
		}
		token.slen = token.cc  -  token.pcc;  //calc len
		if(token.ch=='\n' && token.cc>0 && source[token.cc - 1]=='\r')token.slen-=1;
		token.nlen = token.slen;
		return 1;
	};

	//process indent
	if(((token.ch >='a' && token.ch <='z') || (token.ch >='A' && token.ch <='Z'))  || token.ch=='$' || token.ch=='_'||token.ch=='#'||token.ch > 255)  {
		int npos = 0;
		do
		{
			//token.snum[npos++]=token.ch;
			token.nlen++;
			nextch();
		}while((token.snum[0]=='f' && token.snum[1]=='n' && token.ch==':')||
			token.ch =='_' || token.ch > 255  ||(token.ch >='0' && token.ch <='9')||  ((token.ch >='a' && token.ch <='z') || (token.ch >='A' && token.ch <='Z')) ||token.ch=='$' ||token.ch=='-'
			||(state & lexerFlag::tIdentdot && token.ch=='.') ); //fn: for xml function
		//token.snum[npos]='\0';
		//macro name
		if(token.ch=='!')
		{
			token.nlen++;
			nextch();
		}
		//_tcslwr_s(token.snum,_tcslen(token.snum) + 1);
		/*
		int j;
		int i;
		int k;

		i=1;
		j=_cp_token_key_def_end - _cp_token_key_def_start - 1;
		do
		{
			k=(i+j)/2;
			if(wcscmp(token.snum,keywords[k - 1].name)<=0)j=k - 1;
			if(wcscmp(token.snum,keywords[k - 1].name)>=0) i=k + 1;
		}while( i<=j);
		if(i - 1> j)
			token.inum = keywords[k - 1].key;
		else
			token.inum = -1;
		*/

		//process bool
		if((token.snum[0]=='t' || token.snum[0]=='T') && (token.snum[1]=='r' || token.snum[1]=='R') && (token.snum[2]=='u' || token.snum[2]=='U') && (token.snum[3]=='e' || token.snum[3]=='E') && token.cc  - token.pcc ==4/*token.snum[4]==0*/)
		{
			//true
			token.inum = 1;
			token.ty = tokenType::tBool;
		}else if((token.snum[0]=='f' || token.snum[0]=='F') && (token.snum[1]=='a' || token.snum[1]=='A') && (token.snum[2]=='l' || token.snum[2]=='L') && (token.snum[3]=='s' || token.snum[3]=='S') && (token.snum[4]=='e' || token.snum[4]=='E') && token.cc  - token.pcc ==5 /*token.snum[5]==0*/)
		{
			//false
			token.inum = 0;
			token.ty = tokenType::tBool;
		}else if(token.snum[0]=='o' && token.snum[1]=='r' && token.cc  - token.pcc ==2)
		{
			token.inum = opValue::or;
			token.ty = tokenType::tOp;
		}else if(token.snum[0]=='a' && token.snum[1]=='n' && token.snum[2]=='d' && token.cc  - token.pcc ==3)
		{
			token.inum = opValue::and;
			token.ty = tokenType::tOp;
		}else 
			token.ty = tokenType::tIdent;

		token.slen = token.cc  - token.pcc;
		if(token.ch=='\n' && token.cc>0 && source[token.cc - 1]=='\r')token.slen-=1;
		return 1;
	};

	//process string
	if(token.ch=='"') {
		int npos = 0;

		//string
		if(token.ch=='"')
		{
			token.ty= tokenType::tString;
			nextch();
			do
			{
				while(!(token.ch=='\\' || token.ch=='\0' || token.ch=='"'))
				{
					if(!(state & lexerFlag::tEscape))
					{
						//token.snum[npos++]=token.ch;	
					}
					nextch();
				}
				if(token.ch=='\0')
					error(error::err_stringtrail);
				else if(token.ch=='\\')
				{
					token.inum = 1;
					if(!(state & lexerFlag::tEscape))nextch();
					nextch();
				}
				else
				{
					nextch();
					break;
				}
			}while(true);
			//if(!(state & LEXERESCAPE)) token.snum[npos]='\0';

			token.slen = token.cc  - 2 - token.pcc;
			if(token.ch=='\n' && token.cc>0 && source[token.cc - 1]=='\r')token.slen-=1;
			return 1;
		}
	};

	//process char
	if(token.ch=='\'') {
		int npos = 0;

		//string
		//string
		if(token.ch=='\'')
		{
			token.ty = tokenType::tChar;
			nextch();
			do
			{
				while(!(token.ch=='\\' || token.ch=='\0' || token.ch=='\''))
				{
					if(!(state & lexerFlag::tEscape))
					{
						//token.snum[npos++]=token.ch;	
					}
					nextch();
				}
				if(token.ch=='\0')
					error(error::err_stringtrail);
				else if(token.ch=='\\')
				{
					if(!(state & lexerFlag::tEscape))nextch();
					nextch();
				}
				else
				{
					nextch();
					break;
				}
			}while(true);

			token.slen = token.cc  - 2 - token.pcc;
			if(token.ch=='\n' && token.cc>0 && source[token.cc - 1]=='\r')token.slen-=1;
			token.inum = token.snum[1];
			return 1;
		}
	} ;

	//process operator
	{
		if(token.ch=='%')
		{
			nextch();
			if(token.ch=='=')
			{
				token.setOperate(opValue::mod|opValue::_asn);
				nextch();
			}else
				token.setOperate(opValue::mod);
		}else if((state & lexerFlag::tXPath ||  state & lexerFlag::tXQuery) && token.ch=='/')
		{
			nextch();
			if(token.ch=='/')
			{
				token.setOperate(opValue::path2);
				nextch();
			}else
				token.setOperate(opValue::path);
		}
		else if(token.ch=='^')
		{
			nextch();
			if(token.ch=='=')
			{
				token.setOperate(opValue::xor|opValue::_asn);
				nextch();
			}else
				token.setOperate(opValue::xor);
		}else if(token.ch=='~')
		{
			nextch();
			if(token.ch=='=')
			{
				token.setOperate(opValue::bnot|opValue::_asn);
				nextch();
			}else
				token.setOperate(opValue::bnot);
		}
		else if(token.ch==':')
		{
			nextch();
			if(token.ch=='=')
			{
				token.setOperate(opValue::becomes);
				nextch();
			}else if(token.ch==':')
			{
				token.setOperate(opValue::nest);
				nextch();
			}else
			{
				//token.ty=opValue::byte;
				//token.inum = token_colon;
				token.setOperate(opValue::colon);
				//token.inum = token_colon;
			}
		}else if(token.ch=='=')
		{
			nextch();
			if(token.ch=='=')
			{
				token.setOperate(opValue::eq);
				nextch();
			}else if(token.ch=='>')
			{
				token.setOperate(opValue::arrow);
				nextch();
			}else
				token.setOperate(opValue::asn);
		}
		else if(token.ch=='|')
		{
			nextch();
			if(token.ch=='=')
			{
				token.setOperate(opValue::bor|opValue::_asn);
				nextch();
			}else if(token.ch=='|')
			{
				token.setOperate(opValue::or);
				nextch();
			}else
				token.setOperate(opValue::bor);
		}else if(token.ch=='&')
		{
			nextch();
			if(token.ch=='=')
			{
				token.setOperate(opValue::band|opValue::_asn);
				nextch();
			}else if(token.ch=='&')
			{
				token.setOperate(opValue::and);
				nextch();
			}else
				token.setOperate(opValue::band);
		}
		else if(token.ch=='<')
		{
			nextch();
			if(token.ch=='=')
			{
				token.setOperate(opValue::lte);
				nextch();
			}else if(token.ch=='>')
			{
				token.setOperate(opValue::neq);
				nextch();
			}else if(token.ch=='<')
			{
				nextch();
				if(token.ch=='=')
				{
					token.setOperate(opValue::shl|opValue::_asn);
					nextch();
				}else
					token.setOperate(opValue::shl);
			}else
				token.setOperate(opValue::lt);

		}else if(token.ch=='>')
		{
			nextch();
			if(token.ch=='=')
			{
				token.setOperate(opValue::gte);
				nextch();
			}else if(token.ch=='>')
			{
				nextch();
				if(token.ch=='=')
				{
					token.setOperate(opValue::shr|opValue::_asn);
					nextch();
				}else
					token.setOperate(opValue::shr);
			}else
				token.setOperate(opValue::gt);
		}else if(token.ch=='.')
		{
			nextch();
			if(token.ch=='.')
			{
				nextch();
				if(token.ch=='.')
				{
					token.setOperate(opValue::dot3);
					nextch();
				}
				else
				{
					token.setOperate(opValue::dot2);
				}
			}else
			{
				token.setOperate(opValue::dot);
			}
		}
		else if( token.ch=='#')
		{
			token.setOperate(opValue::hash);
			nextch();
		}else if( token.ch=='+' || token.ch=='-' || token.ch=='*' || token.ch=='/')
		{
			wchar_t ch1 = token.ch;
			nextch();
			if(token.ch=='+' && ch1=='+')
			{
				token.setOperate(opValue::inc);
				nextch();
			}else if(token.ch=='-' && ch1=='-')
			{
				token.setOperate(opValue::dec);
				nextch();
			}else if(token.ch=='>' && ch1=='-')
			{
				token.setOperate(opValue::pointer);
				nextch();
			}
			else if(token.ch=='=')
			{
				if(ch1=='+') token.setOperate(opValue::add|opValue::_asn);
				else if(ch1=='-') token.setOperate(opValue::sub|opValue::_asn);
				else if(ch1=='*') token.setOperate(opValue::mul|opValue::_asn);
				else if(ch1=='/') token.setOperate(opValue::div|opValue::_asn);
				nextch();
			}else
			{
				if(ch1=='+') token.setOperate(opValue::add);
				else if(ch1=='-') token.setOperate(opValue::sub);
				else if(ch1=='*') token.setOperate(opValue::mul);
				else if(ch1=='/') token.setOperate(opValue::div);
			}
		}
		else if(token.ch=='(' || token.ch==')' || token.ch=='[' || token.ch==']' || token.ch==';' || token.ch==',' 
			|| token.ch=='{' || token.ch=='}'  ||token.ch=='@'||token.ch=='\\')  
		{
			token.setOperate(opValue::getOperateByToken(token.ch));
			nextch();
		}
		else if(token.ch=='?')
		{
			nextch();
			token.setOperate(opValue::query);
		}
		else if(token.ch=='!')
		{
			nextch();
			if(token.ch=='=')
			{
				token.setOperate(opValue::neq);
				nextch();
			}else
			{
				token.setOperate(opValue::not);
			}
		}
		//token.inum =  token.ty;
		if(token.ty != 0)
		{
			token.slen = token.cc  - token.pcc;
			if(token.ch=='\n' && token.cc>0 && source[token.cc - 1]=='\r')token.slen-=1;
			return 1;
		}
	};

	//this is should error
	nextch();

	return -1;
}


int vlLexer::error(unsigned int errcode)
{
	return 1;
}

//get char at current pos + npos;
wchar_t vlLexer::getCharAt(int pos)
{
	if(pos + token.cc >=sourceLen) return 0;
	return source[pos + token.cc];
}

//used for save and restore token
int vlLexer::save()
{
	//save token information to token.stack
	token.row = srcPosition.curRow;
	token.col = srcPosition.curCol;

	tokenes.push_back(token);
	return 1;
}

//restore state
int vlLexer::restore()
{
	//pop token.stack to token
	token=tokenes[tokenes.size() - 1], tokenes.pop_back();
	srcPosition.curRow = token.row;
	srcPosition.curCol = token.col;

	return 1;
}

int vlLexer::consume()
{
	return pop();
}

//push state
int vlLexer::push()
{
	//save token information to token.stack
	tokenes.push_back(token);
	return 1;
}

//push state
int vlLexer::push(vlToken & tk)
{
	//save token information to token.stack
	tokenes.push_back(tk);
	return 1;
}

//pop state
int vlLexer::pop()
{
	//pop token.stack 
	if(tokenes.size())tokenes.pop_back();
	return 1;
}

//get item
vlToken& vlLexer::item(int index)
{
	//pop token.stack 
	return tokenes[tokenes.size() - 1 - index];
}

//find  and goto  token tk 
int vlLexer::find(int tk)
{
	if(token.ty==tk) return 1;

	insymbol();

	//find ']'
	if(tk==opValue::lbrackets) 
	{
		int count=1;
		do
		{
			if( token.ty== opValue::lbrackets)  count++;
			if( token.ty==opValue::rbrackets) 
			{
				count--;
				if(count==0)return 1;
			}
			insymbol();
		}while(token.ty !=opValue::eof) ;
		return 0;
	};

	//find ')'
	if(tk==opValue::rparen)
	{
		int count=1;
		do
		{
			if( token.ty== opValue::lparen)  count++;
			if( token.ty==opValue::rparen) 
			{
				count--;
				if(count==0)return 1;
			}
			insymbol();
		}while(token.ty !=opValue::eof) ;
		return 0;
	};

	//find '}'
	if(tk==opValue::rbrace)
	{
		int count=1;
		do
		{
			if( token.ty== opValue::lbrace)  count++;
			if( token.ty==opValue::rbrace) 
			{
				count--;
				if(count==0)return 1;
			}
			insymbol();
		}while(token.ty !=opValue::eof) ;
		return 0;
	};

	//find other
	{
		do
		{
			if( token.ty==tk)return 1 ;

			if(token.ty==opValue::lbrackets || token.ty==opValue::lparen || token.ty==opValue::lbrace)
			{
				if(token.ty==opValue::lbrackets)
					find(opValue::rbrackets);
				else if(token.ty==opValue::lparen)
					find(opValue::rparen);
				else if(token.ty==opValue::lbrace)
					find(opValue::rbrace);
				if(token.ty ==opValue::eof) return 0;
			}
			insymbol();
		}while(token.ty !=opValue::eof) ;

		return 0;
	};
	return 1; 
}

#endif //WPP5_LEXER_H