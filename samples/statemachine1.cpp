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

using namespace xl;
using namespace xl::util::lexer;
using namespace xl::util::token;


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
	//#define SureOp(x,...)  x && SuperOp1(__VA_ARGS)
	
	#define nextToken()  tk = lexer.insymbol()
	
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
			Lexer<wchar_t> lexer;
			lexer.SetSource(L"a c d e f;a1+b;c+m..n+d;if (a) {a+b;e=f;}",0);
			Token<wchar_t> tk = lexer.insymbol();
			StateRun(Declare);
			//auto x = std::make_tuple(1,2,3);
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
					tk = lexer.insymbol();
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
					tk = lexer.insymbol();
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
						tk = lexer.insymbol();
						//read  param
						while(!tk.IsOperate(opRparen))
						{
							StateTo(Expr)
							if(!tk.IsOperate(opComma))break;
							tk = lexer.insymbol();
						}
						if(!tk.IsOperate(opRparen))
							StateTo(MatchError);
						tk = lexer.insymbol();
					}
					else if(tk.IsOperate(opLbrackets))
					{
						tk = lexer.insymbol();
						//read rank
						StateTo(Expr)
						SureMatch(tk.IsOperate(opRbrackets))
						tk = lexer.insymbol();
					}
					else if(tk.IsOperate(opDot))
					{
						tk = lexer.insymbol();
						SureMatch(tk.IsIdent());
						//read field
						tk = lexer.insymbol();
					}
					else if(tk.IsOperate(opInc))
					{
						//read field
						tk = lexer.insymbol();
					}else if(tk.IsOperate(opDec))
					{
						//read field
						tk = lexer.insymbol();
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
					tk = lexer.insymbol();
				}
				else if(tk.IsIdent())
				{
					tk = lexer.insymbol();
				}
				else if(tk.IsLiteral())
				{
					tk = lexer.insymbol();
				}else if(tk.IsOperate(opLparen))
				{
					tk = lexer.insymbol();
					StateTo(Expr)
					SureMatch(tk.IsOperate(opRparen));
					tk = lexer.insymbol();
				}
				else
				{
					SureMatch(option);
				}
				StateLeave
			}
			
		}
};

int main()
{
	StateMachine  machine;
	machine.run(Context());
	
	//std::cout << typeid(int).name() << std::endl;
	
	printf("\n");
	
	return 1;
}