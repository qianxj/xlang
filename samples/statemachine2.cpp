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

struct PrimaryExprStateContext : public StateContext
{
};

struct UnaryExprStateContext : public StateContext
{
};

struct BinaryExprStateContext : public StateContext
{
};

struct CastExprStateContext : public StateContext
{
};

struct PostfixExprStateContext : public StateContext
{
};

struct ExprStateContext : public StateContext
{
};


class Machine
{
	public:
		virtual int run(Context& context, intptr_t param = 0) = 0;
};

class StateMachine : public Machine
{
	public:
	enum State
	{
		kExpr,
		kBinaryExpr,
		kCastExpr,
		kPostfixExpr,
		kUnaryExpr,
		kPrimaryExpr,
		kExit
	};
	
	#define StateRun(state)  __asm call state
	
	#define StateTo(toState)  {\
				contexts.push_back(stateContext); \
				printf("%s <- %S\n",#toState,tk.toString().c_str());\
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
	
	#define nextToken()  tk= lexer.insymbol()
	
	public:
		Lexer<wchar_t> lexer;
	
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
			bool option = false;
			std::vector<CheckPointContext*> checkPoints;
			std::vector<StateContext *> contexts;
			//lexer.SetSource(L"a c d e f;a1+b;c+m..n+d;if (a) {a+b;e=f;}",0);
			Token<wchar_t> tk= lexer.insymbol();
			StateRun(Declare);
			
			return 1;
		
		MatchError:
			{
				printf("error: no hope  token %S \n",tk.toString().c_str());
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
				}
				StateLeave
			}
		
		MisToken_Expr:
		{
			while(!tk.IsOperate(opSemicolon) && !tk.IsEof())
				nextToken();
			nextToken();
			StateLeave
		}
		
		MisToken_IfCond:
		{
			while(!tk.IsOperate(opRparen) && !tk.IsEof() && !tk.IsOperate(opLbrace))
				nextToken();
			nextToken();
			StateLeave
		}
		
		Declare:
			{
				while(!tk.IsEof())
					StateTo(Stmt)
				StateLeave
			}
			
		Stmt:
			{
				if(tk.IsOperate(opSemicolon))
					nextToken();
				else if(tk.IsOperate(opLbrace))
					StateTo(CompStmt)
				else if(tk.IsIdent(L"if"))
				{
					StateTo(IfStmt)
				}
				else if(tk.IsIdent(L"piece"))
				{
					StateTo(PieceDefineStmt)
				}
				else
					StateTo(ExprStmt)
				StateLeave
			}
		
		CompStmt:
			{
				SureMatch(tk.IsOperate(opLbrace));
				nextToken();
				while(!tk.IsEof() && !tk.IsOperate(opRbrace))
					StateTo(Stmt);
				SureMatch(tk.IsOperate(opRbrace));
				nextToken();
				StateLeave
			}
			
		IfStmt:
			{
				if(tk.IsIdent(L"if"))
				{
					nextToken();
					SureMatch(tk.IsOperate(opLparen));
					nextToken();
					CheckPoint(MisToken_IfCond,Lable_IfBody)
					StateTo(Expr);
					SureMatch(tk.IsOperate(opRparen));
					nextToken();
				Lable_IfBody:
					StateTo(Stmt);
				}
				StateLeave
			}
			
		ExprStmt:
			{
				CheckPoint(MisToken_Expr,ExprStmt_Leave)
				StateTo(Expr);
				SureMatch(tk.IsOperate(opSemicolon));
				nextToken();
			ExprStmt_Leave: 
				RestorePoint()
				StateLeave
			}
		Expr:
			{
				auto stateContext = new BinaryExprStateContext();
				StateTo(BinaryExpr);
				StateLeave
			}
			
		BinaryExpr: 
			{
				auto stateContext = new BinaryExprStateContext();
				StateTo(CastExpr) 
				while(tk.IsOperate(opAdd) || 
					tk.IsOperate(opSub) ||
					tk.IsOperate(opMul) ||
					tk.IsOperate(opDiv) ||
					tk.IsOperate(opMod) ||
					tk.IsOperate(opBand) ||
					tk.IsOperate(opBor) ||
					tk.IsOperate(opShr) || 
					tk.IsOperate(opShl) || 
					tk.IsOperate(opAsn))
				{
					nextToken();
					StateTo(CastExpr);
				}
				StateLeave
			}
			
		CastExpr:
			{
				auto stateContext = new CastExprStateContext();
				StateTo(UnaryExpr); 
				StateLeave
			}
			
		UnaryExpr:
			{
				auto stateContext = new UnaryExprStateContext();
				while(tk.IsOperate(opInc) || tk.IsOperate(opDec))
				{
					nextToken();
					StateTo(UnaryExpr)
				}					
				StateTo(PostfixExpr)
				StateLeave
			}
			
		PostfixExpr:
			{
				auto stateContext = new PostfixExprStateContext();
				StateTo(PrimaryExpr);
				while(true)
				{
					if(tk.IsOperate(opLparen))
					{
						nextToken();
						//read  param
						while(!tk.IsOperate(opRparen))
						{
							StateTo(Expr)
							if(!tk.IsOperate(opComma))break;
							nextToken();
						}
						if(!tk.IsOperate(opRparen))
							StateTo(MatchError);
						nextToken();
					}
					else if(tk.IsOperate(opLbrackets))
					{
						nextToken();
						//read rank
						StateTo(Expr)
						SureMatch(tk.IsOperate(opRbrackets))
						nextToken();
					}
					else if(tk.IsOperate(opDot))
					{
						nextToken();
						SureMatch(tk.IsIdent());
						//read field
						nextToken();
					}
					else if(tk.IsOperate(opInc))
					{
						//read field
						nextToken();
					}else if(tk.IsOperate(opDec))
					{
						//read field
						nextToken();
					}
					else
						break;
				}
				StateLeave
			}
			
		PrimaryExpr:
			{
				auto stateContext = new PrimaryExprStateContext();
				if(tk.IsIdent(L"this"))
				{
					nextToken();
				}
				else if(tk.IsIdent())
				{
					nextToken();
				}
				else if(tk.IsLiteral())
				{
					nextToken();
				}else if(tk.IsOperate(opLparen))
				{
					nextToken();
					StateTo(Expr)
					SureMatch(tk.IsOperate(opRparen));
					nextToken();
				}
				else
				{
					SureMatch(option);
				}
				StateLeave
			}
		PieceDefineStmt:
			{
				if(!tk.IsIdent(L"piece"))
					StateLeave
				nextToken();
				SureMatch(tk.IsIdent())
				nextToken();
				SureMatch(tk.IsOperate(opLparen));
				nextToken();
				if(!tk.IsOperate(opRparen))
				{
					while(true)
					{
						StateTo(Expr)
						if(tk.IsOperate(opRparen) || tk.IsEof())break;
						SureMatch(tk.IsOperate(opComma))
						nextToken();
					}
				}
				SureMatch(tk.IsOperate(opRparen))
				nextToken();
				
				SureMatch(tk.IsOperate(opLbrackets))
				nextToken();
				if(!tk.IsOperate(opRbrackets))
				{
					while(!tk.IsEof() && !tk.IsOperate(opRbrackets))
					StateTo(Stmt);
				}
				SureMatch(tk.IsOperate(opRbrackets))
				nextToken();
				StateLeave
			}
		}
		
		
};

int main()
{
	StateMachine  machine;
	
	wchar_t* source = ReadSource(L"G:\\xlang\\samples\\data\\piece1.txt");
	printf("%S",source);
	machine.lexer.SetSource(source,0);
	machine.run(Context());
	
	//std::cout << typeid(int).name() << std::endl;
	
	printf("\n");
	
	return 1;
}