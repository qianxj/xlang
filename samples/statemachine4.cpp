#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <string>
#include <stdlib.h>

#include <vector>
#include <tuple>
#include <ostream>

#include "xllexer.hpp"
#include "xltokenutil.cpp"
#include "windows.h"

using namespace xl;
using namespace xl::util::lexer;
using namespace xl::util::token;


wchar_t * ReadSource(wchar_t* srcFile)
{
	wchar_t* f = srcFile;
	HANDLE hFile = ::CreateFile(f,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	DWORD nLen = GetFileSize(hFile,0);

	LPBYTE pData = new BYTE[nLen + 2];
	::ReadFile(hFile,pData,nLen,&nLen,NULL);
	pData[nLen] = 0;
	pData[nLen+1] = 0;
	CloseHandle(hFile);

	wchar_t * pStr;
	int ch=0;
	bool bDelete = false;
	if((pData[0]==0xff &&  pData[1]==0xfe)||(pData[0]==0xfe &&  pData[1]==0xff))
	{
		pStr = (LPTSTR)(pData + 2);
	}else if((pData[0]==0xEF &&  pData[1]==0xBB && pData[2]==0xBF))
	{
		int _convert = MultiByteToWideChar(CP_UTF8, 0, (LPSTR)pData+3, nLen, NULL, 0);
		TCHAR * psz = new TCHAR[_convert + 1];
		int nRet = MultiByteToWideChar(CP_UTF8, 0,(LPSTR)pData+3, nLen, psz, _convert);
		psz[_convert]='\0';
		pStr = psz;
		bDelete = true; 
	}
	else if(pData[0] !=0 && pData[1]!=0) //ansi should gb2312
	{
		int _convert = MultiByteToWideChar(CP_ACP, 0, (LPSTR)pData, nLen, NULL, 0);
		TCHAR * psz = new TCHAR[_convert + 1];
		int nRet = MultiByteToWideChar(CP_ACP, 0,(LPSTR)pData, nLen, psz, _convert);
		psz[_convert]='\0';
		pStr = psz;
		bDelete = true;
	}else //should utf-16
	{
		pStr = (LPTSTR)pData;
	}
	if(bDelete) delete pData;

	return pStr;
}

struct Context
{
};

struct StateContext
{
	void * result;
};

class Machine
{
	public:
		virtual int run(Context& context, intptr_t param = 0) = 0;
};

/*
	bnf-lhs = identifier;
	bnf-term = string-literal | char-literal | double-literal| identifier;
	bnf-option = '['  bnf-rhs ']';
	bnf-group = '(' bnf-rhs ')';
	bnf-repeat = '{' bnf-rhs '}';
	bnf-rhs = ( bnf-term | bnf-option | bnf-group | bnf-repeat) { ['|'] bnf-rhs };
	bnf-ruler = bnf-lhs '=' bnf-rhs ';' ;
	bnf-gramer = { bnf-ruler };
*/

class StateMachine : public Machine
{
	public:
	enum State
	{
	};
	
	#define StateRun(state)  __asm call state
	
	#define StateTo(toState)  {\
				contexts.push_back(stateContext); \
				__asm {call toState }	\
				stateContext = (decltype(stateContext))contexts.back();\
				contexts.pop_back(); }\
				
	
	#define StateLeave  { void *result  =  stateContext->result; \
				__asm { __asm mov eax, result \
							__asm ret \
						  }\
				} 
	
	#define CheckPoint(st,dest)  { CheckPointContext* cpContext = new CheckPointContext(); \
							  intptr_t xp; \
							  __asm { mov xp,esp }\
							  cpContext->esp = xp; \
							  cpContext->contextsSize = (int)contexts.size(); \
							  checkPoints.push_back(cpContext);\
							  intptr_t addr;\
							  intptr_t jmpdest;\
							  __asm {\
							  __asm mov eax, st \
							  __asm mov addr,eax \
							  __asm mov eax,dest \
							  __asm mov jmpdest,eax }\
							  checkPoints.back()->state = addr;\
							  checkPoints.back()->jmpdest = jmpdest; }
							  
	#define RestorePoint() checkPoints.pop_back();
	
	#define SureMatch(cond) if(!cond) StateTo(MatchError)
	
	public:
		virtual int run(Context& context,intptr_t param =0 )
		{
			struct CheckPointContext
			{
				intptr_t esp;
				intptr_t jmpdest;
				int contextsSize;
				intptr_t state;
			};
			StateContext* stateContext  = new StateContext();
			std::vector<CheckPointContext*> checkPoints;
			std::vector<StateContext *> contexts;
			
			wchar_t*  pSrc 	= 0;
			int 			pos 	= -1;
			wchar_t 	ch 	= 0;
			
			StateTo(NextCh)
			StateRun(InSymbol);
			
			return 1;
		
		NextCh:
			{
				pos++;
				ch = pSrc[pos];
				if(ch=='\r')
				{
					if(pSrc[pos+1]=='\n')pos++;
					ch='\n';
				}
				else if(ch=='\n')
				{
					if(pSrc[pos+1]=='\r')pos++;
				}
				StateLeave
			}
		SkipSpace:
			{
				while(ch==' '||ch=='\t'||ch=='\n')StateTo(NextCh)
				StateLeave
			}
		InSymbol:
			{
				StateTo(SkipSpace)
				while(true)
				{
					StateTo(SkipSpace)
					if(ch=='/' && pSrc[pos+1]=='/')
					{
						StateTo(LineComment)
						continue;
					}
					else if(ch=='/' && pSrc[pos+1]=='*')
					{
						StateTo(BlockComment)
							continue;
					}
					break;
				}
				else if(ch=='"')
				{
					StateTo(EatString)
				}
				else if(ch=='\'')
				{
					StateTo(EatChar)
				}
				else if(ch>='0' && ch<='9')
				{
					StateTo(EatNumber)
				}
				else if((ch>='a' && ch <='z') || (ch>='A' && ch<='Z') || ch=='$' || ch=='_' || ch > 255)
				{
					StateTo(EatIdent)
				}
				else 
				{
					StateTo(EatOperate)
				}
				StateLeave
			}
			
		LineComment:
			{
				StateTo(NextCh)
				while(ch && ch !='\n') StateTo(NextCh)
				StateLeave
			}
		BlockComment:
			{
				StateTo(NextCh),
				StateTo(NextCh)
				StateLeave
			}
		EatNumber:
			StateLeave
		EatString:
			StateLeave
		EatChar:
			StateLeave
		EatIdent:
			StateLeave
		EatOperate:
			StateLeave
		
		MatchError:
			{
				/*printf("error: no hope  token %S \n",tk.toString().c_str());
				if(checkPoints.size())
				{
					CheckPointContext* checkpoint = checkPoints.back();
					if(checkpoint->state)
					{
						intptr_t state = checkpoint->state;
						__asm mov eax, state
						__asm call eax
						if( checkpoint->esp)
						{
							intptr_t xesp = checkpoint->esp;
							__asm mov esp, xesp
						}
						if(checkpoint->jmpdest)
						{
							intptr_t jmpdest = checkpoint->jmpdest;
							__asm jmp jmpdest;
						}
					}
				}*/
				StateLeave
			}
		}
};

int main()
{
	StateMachine  machine;
	
	wchar_t* source = ReadSource(L"G:\\xlang\\samples\\data\\bnf1.vlang");
	printf("%S",source);
	machine.lexer.SetSource(source,0);
	machine.run(Context());
	
	printf("\n");
	
	return 1;
}