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
#include "../tmp/machine_x86.hpp"

#include "windows.h"

using namespace xl;
using namespace xl::util::lexer;
using namespace xl::util::token;
using namespace XM;


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
	intptr_t result;
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
				printf("%s <- %S\n",#toState,tk.toString().c_str());\
				__asm {call toState }	\
				stateContext = (decltype(stateContext))contexts.back();\
				contexts.pop_back(); }\
	
	#define StateToEx(toState,retVal)  {\
				contexts.push_back(stateContext); \
				printf("%s <- %S\n",#toState,tk.toString().c_str());\
				__asm {call toState }	 \
				__asm {mov retVal, eax} \
				stateContext = (decltype(stateContext))contexts.back();\
				contexts.pop_back(); }\
	
	#define StateReturn( retVal )  { intptr_t result = (intptr_t)retVal;	\
				__asm { __asm mov eax, result \
							__asm ret \
						  }\
				} 
	
	#define StateLeave  { intptr_t result  =  stateContext->result; \
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
	#define SureEat(cond) if(!cond) StateTo(MatchError) else nextToken();
	
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
		
		/*bnf_term = one of bnf_rhs | option bnf_rhs | list bnf_rhs*/
		bnf_term:
		{
			if(tk.IsIdent(L"var"))
				StateTo(variant_define)
			else if(tk.IsIdent(L"one"))
			{
				nextToken();
				SureEat(tk.IsIdent(L"of"));
				SureEat(tk.IsOperate(opLparen));
				while(true)
				{
					StateTo(bnf_rhs)
					if(!tk.IsOperate(opComma))break;
					nextToken();
				}
				SureEat(tk.IsOperate(opRparen))
			}
			else if(tk.IsIdent(L"option"))
			{
				nextToken();
				if(tk.IsOperate(opLparen))
				{
					nextToken();
					StateTo(bnf_rhs)
					SureEat(tk.IsOperate(opRparen))
				}else
					StateTo(bnf_term)
			}
			else if(tk.IsIdent(L"list"))
			{
				nextToken();
				if(tk.IsOperate(opLparen))
				{
					nextToken();
					StateTo(bnf_rhs)
					if(tk.IsOperate(opComma))
					{
						nextToken();
						StateTo(bnf_rhs)
					}
					SureEat(tk.IsOperate(opRparen))
				}else
					StateTo(bnf_term)
			}
			else if(tk.IsIdent())
			{
				nextToken();
			}
			else if(tk.IsChar())
			{
				nextToken();
			}
			else if(tk.IsString())
			{
				nextToken();
			}
			else if(tk.IsOperate(opSub))
			{//guard
				nextToken();
			}
			else if(tk.IsOperate(opNot))
			{//action
				nextToken();
				SureEat(tk.IsIdent())
				SureEat(tk.IsOperate(opLparen))
				//read param
				while(!tk.IsOperate(opRparen))
				{
					StateTo(bnf_term)
					if(!tk.IsOperate(opComma) || tk.IsEof()) break;
					nextToken();
				}
				SureEat(tk.IsOperate(opRparen))
			}else
			{
				StateReturn(0)
			}
			StateReturn(1)
		}
	
		//bnf-rhs = ( bnf-term | bnf-option | bnf-group | bnf-repeat) { ['|'] bnf-rhs };
		bnf_rhs:
		{
			while(true)
			{
				intptr_t retVal;
				StateToEx(bnf_term, retVal)
				if(!retVal) break;
			}
			if(tk.IsOperate(opBor))
				StateTo(bnf_rhs);
			StateLeave	
		}
		
		variant_define:
		{
			nextToken(); //eat var
			while(true)
			{
				if(tk.IsIdent(L"list"))
					nextToken();
				SureEat(tk.IsIdent())
				if(!tk.IsOperate(opComma) || tk.IsEof()) break;
				nextToken();
			}
			StateLeave	
		}
		
		bnf_ruler:
		{
			SureEat(tk.IsIdent(L"syntax"))
			SureEat(tk.IsIdent())
			SureEat(tk.IsOperate(opColon))
			StateTo(bnf_rhs)
			SureEat(tk.IsOperate(opSemicolon))
			
			StateLeave	
		}
		
		Declare:
		{
			while(!tk.IsEof())
				StateTo(bnf_ruler)
			StateLeave
		}
		
	}
};

int main1()
{
	StateMachine  machine;
	
	wchar_t* source = ReadSource(L"G:\\xlang\\samples\\data\\bnf1.vlang");
	printf("%S",source);
	machine.lexer.SetSource(source,0);
	machine.run(Context());
	
	//std::cout << typeid(int).name() << std::endl;
	
	printf("\n");
	
	return 1;
}

int  EnterFrame(unsigned char* & pc)
{
	pc = X86::PUSH(pc,X86::ebp);
	pc = X86::MOV(pc,X86::ebp,X86::esp);
	//pc = X86::PUSH(pc,X86::esp);
	pc = X86::SUB(pc,X86::esp,Imm<B32>(0x128));
	pc = X86::PUSH(pc,X86::ebx);
	pc = X86::PUSH(pc,X86::esi);
	pc = X86::PUSH(pc,X86::edi);
	//save this
	//pc = X86::MOV(pc,X86::AddressX86<B32>(X86::ebp, Disp<B32>(-4)),X86::ecx);

	return 1;
}


int LeaveFrame(unsigned char* & pc, int ret = 0)
{
	//MOVD(xmm0,eax);
	//MOVD(eax,xmm0);
	
	pc = X86::POP(pc,X86::edi);
	pc = X86::POP(pc,X86::esi);
	pc = X86::POP(pc,X86::ebx);
	//pc = ADD(X86::esp,X86::Imm(0x128));
	pc = X86::MOV(pc,X86::esp,X86::ebp); 
	pc = X86::POP(pc,X86::ebp);
	pc = X86::RET(pc,Imm<B16>(ret));	
	return 0;
}

typedef  int (* FuncMatchEval)(Token<wchar_t> & tk) ;

struct TMatchNode
{
	Token<wchar_t> tk;
	int state;
	
	TMatchNode(const wchar_t* keyword, int state)
	{
		tk.kind = tkString;
		tk.strVal = keyword;
		tk.strLen = wcslen(keyword);
		tk.tokenLen = tk.strLen;
		this->state = state;
	}
	
	TMatchNode(int state,const wchar_t* ident )
	{
		tk.kind = tkIdent;
		tk.strVal = ident;
		tk.strLen = wcslen(ident);
		tk.tokenLen = tk.strLen;
		this->state = state;
	}
	
	TMatchNode(TokenKind kind, int state)
	{
		tk.kind = kind;
		this->state = state;
	}
	
	TMatchNode(Operate op, int state)
	{
		tk.kind = tkOperate;
		tk.opVal = op;
		this->state = state;
	}
	
	bool match(Token<wchar_t> &tk)
	{
		return false;
	}
};

struct TStateNode
{
	int state;
	int matchNodeIndex;
	int matchNodeCount;
	FuncMatchEval funcMatchEval;
};

#define Offset(Ty, Field) ((int)(&((Ty*)((void*)0))->Field))
int genState(unsigned char* code, unsigned char* & pc, int state, std::vector<TMatchNode>  & matchNodes, std::vector<TStateNode> & stateNodes,
		std::map<wstring,int> &syns)
{
	std::vector<int> labels;
	std::vector<int> olabels;
	std::vector<int> rlabels;
	
	stateNodes[state].funcMatchEval = (FuncMatchEval)(void*)pc;
	EnterFrame(pc);

	//load tk to ebx
	//fast call
	pc = X86::MOV(pc,X86::ebx,X86::AddressX86<B32>(X86::ebp,Disp<B8>(8))); //mov ebx, dword ptr[ebp+8]
	//pc = X86::MOV(pc,X86::ebx,X86::ecx); 
	//load kind to al
	pc = X86::MOV(pc,X86::al,X86::AddressX86<B8>(X86::ebx,Disp<B8>(Offset(Token<wchar_t>,kind))));
	//load strlen to ecx, load strVal to esi, load val to edx
	pc = X86::MOV(pc,X86::ecx,X86::AddressX86<B32>(X86::ebx,Disp<B32>(Offset(Token<wchar_t>,strLen))));
	pc = X86::MOV(pc,X86::esi,X86::AddressX86<B32>(X86::ebx,Disp<B8>(Offset(Token<wchar_t>,strVal))));
	pc = X86::MOV(pc,X86::edx,X86::AddressX86<B32>(X86::ebx,Disp<B8>(Offset(Token<wchar_t>,intVal))));
	
	TStateNode& stateNode = stateNodes[state];
	int i = 0;
	for(i= stateNode.matchNodeIndex;i< stateNode.matchNodeIndex+ stateNode.matchNodeCount;i++)
	{
			Token<wchar_t> & tk = matchNodes[i].tk;
			int destState =  matchNodes[i].state;
		
			//keyword
			if(tk.kind == tkString)
			{
				//comp kind
				pc = X86::CMP(pc,X86::al,Imm<B8>(tkIdent));
				pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
				labels.push_back(pc - code -1);
				
				//comp strlen
				pc = X86::CMP(pc,X86::ecx,Imm<B8>(tk.strLen));
				pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
				labels.push_back(pc - code -1);

				//comp keyword
				int nlen = tk.strLen;
				int pos = 0;
				while(nlen - pos > 0)
				{
					if(nlen - pos > 1)
					{
						pc = X86::CMP(pc,X86::AddressX86<B32>(X86::esi,Disp<B8>(pos*2) ),Imm<B32>(*(int *)(tk.strVal + pos)));
					}else
					{
						pc = X86::CMP(pc,X86::AddressX86<B16>(X86::esi,Disp<B8>(pos*2) ),Imm<B16>(tk.strVal[pos]));
					}
					pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
					labels.push_back(pc - code -1);
					pos += 2;
				}
			}
			else if(tk.kind == tkOperate)
			{
				//comp kind
				pc = X86::CMP(pc,X86::al,Imm<B8>(tkOperate));
				pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
				labels.push_back(pc - code -1);
				
				//comp operate
				pc = X86::CMP(pc,X86::dl,Imm<B8>(tk.opVal));
				pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
				labels.push_back(pc - code -1);
				
			}
			else if(tk.kind == tkIdent)
			{
				if(wcscmp(tk.strVal,L"identifier")==0)
				{
					//comp kind
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkIdent));
					pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
					labels.push_back(pc - code -1);
					
				}
				else if(wcscmp(tk.strVal,L"literal")==0)
				{
					//comp kind
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkLiteral));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkString));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkInt));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkUInt));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkDouble));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkFloat));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkChar));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkBool));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkHex));
					pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
					labels.push_back(pc - code -1);
				}
				else if(wcscmp(tk.strVal,L"string-literal")==0)
				{
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkString));
					pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
					labels.push_back(pc - code -1);
				}
				else if(wcscmp(tk.strVal,L"integer-literal")==0)
				{
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkInt));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkUInt));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkHex));
					pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
					labels.push_back(pc - code -1);
				}
				else if(wcscmp(tk.strVal,L"double-literal")==0)
				{
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkInt));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkUInt));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkHex));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkDouble));
					pc = X86::JCC(pc,TCond::cEqual,Imm<B8>(0));
					olabels.push_back(pc - code -1);
					
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkFloat));
					pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
					labels.push_back(pc - code -1);
				}
				else if(wcscmp(tk.strVal,L"bool-literal")==0)
				{
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkBool));
					pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
					labels.push_back(pc - code -1);
				}
				else if(wcscmp(tk.strVal,L"char-literal")==0)
				{
					pc = X86::CMP(pc,X86::al,Imm<B8>(tkChar));
					pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
					labels.push_back(pc - code -1);
				}
				else
				{
					if(syns.find(tk.strVal)==syns.end())
						//syntax
						assert(false);
					else
						
				}
			}
			else if(tk.kind != tkAny)
				assert(false);
			
			//label here
			for(int i=0;i<(int)olabels.size();i++)
			{
				*(code + olabels[i]) = (pc - code ) - olabels[i] - 1;
			}
			olabels.clear();
			
			pc = X86::MOV(pc,X86::eax, Imm<B32>(destState));
			pc = X86::JMP(pc,Imm<B8>(0));
			rlabels.push_back(pc - code -1);

			//label here
			for(int i=0;i<(int)labels.size();i++)
			{
				*(code + labels[i]) = (pc - code ) - labels[i] - 1;
			}
			labels.clear();
	}
	
	if(i>= stateNode.matchNodeIndex+ stateNode.matchNodeCount)
	{
		X86::MOV(pc,X86::eax, Imm<B32>(-1));
	}
		
	//rlael here
	for(int i=0;i<(int)rlabels.size();i++)
	{
		*(code + rlabels[i]) = (pc - code) - rlabels[i] - 1;
	}
	rlabels.clear();
	LeaveFrame(pc,0);
	return 1;
}


int main2()
{

	
	std::vector<TMatchNode> matchNodes;
	std::vector<TStateNode> stateNodes;
	std::map<std::wsting /*sym*/, int /*state*/> syns;
	
	
	printf("\token.kind %d\n", Offset(Token<wchar_t>,strVal));
	
	int prot = PAGE_EXECUTE_READWRITE ;
	int msize = 0x1000;

	LPVOID mbase = VirtualAlloc(NULL,
		msize,
		MEM_COMMIT | MEM_RESERVE,
		prot);
	if (mbase == NULL)
	{
		printf("\r\n%s","error call VirtualAlloc.");
		return 0;
	}
	
	unsigned char* code = (unsigned char*)mbase;
	unsigned char* pc = code;
	
	std::vector<int> labels;
	std::vector<int> rlabels;
	std::vector<int> flabels;
	
	EnterFrame(pc);
	//arg1
	pc = X86::MOV(pc,X86::ebx,X86::AddressX86<B32>(X86::ebp,Disp<B8>(8))); //mov ebx, dword ptr[ebp+8]
	
	//tk.kind==tkIdent?
	pc = X86::MOV(pc,X86::eax,X86::AddressX86<B32>(X86::ebx,Disp<B8>(Offset(Token<wchar_t>,kind)))); //ebx <- tk.kind
	pc = X86::CMP(pc,X86::al,Imm<B8>(tkIdent));
	pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
	flabels.push_back(pc - code -1);
		
	//ecx for tk.strLen
	//ebx for tk.strVal
	pc = X86::MOV(pc,X86::ecx,X86::AddressX86<B32>(X86::ebx,Disp<B32>(Offset(Token<wchar_t>,strLen))));
	pc = X86::MOV(pc,X86::ebx,X86::AddressX86<B32>(X86::ebx,Disp<B8>(Offset(Token<wchar_t>,strVal))));
	
	//tk.strLem==2?
	pc = X86::CMP(pc,X86::ecx,Imm<B8>(2));
	pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
	labels.push_back(pc - code -1);

	//tk.strVal="if"
	pc = X86::MOV(pc,X86::esi,X86::ebx);
	pc = X86::CMP(pc,X86::AddressX86<B32>(X86::esi),Imm<B32>((((unsigned short)'f')<<16) + (unsigned short)'i'));
	pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
	labels.push_back(pc - code -1);

	//state <1
	pc = X86::MOV(pc,X86::eax,Imm<B32>(1));
	pc = X86::JMP(pc,Imm<B8>(0));
	rlabels.push_back(pc - code -1);
	
	//lael here
	for(int i=0;i<(int)labels.size();i++)
	{
		*(code + labels[i]) = (pc - code ) - labels[i] - 1;
	}
	labels.clear();

	//tk.strLem==3?
	//pc = X86::MOV(pc,X86::ecx,X86::AddressX86<B32>(X86::ebx,Disp<B32>(Offset(Token<wchar_t>,strLen))));
	pc = X86::CMP(pc,X86::ecx,Imm<B8>(3));
	pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
	labels.push_back(pc - code -1);
	
	//tk.strVal = "for"
	pc = X86::MOV(pc,X86::esi,X86::ebx);
	pc = X86::CMP(pc,X86::AddressX86<B32>(X86::esi),Imm<B32>((((unsigned short)'o')<<16) + (unsigned short)'f'));
	pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
	labels.push_back(pc - code -1);
	pc = X86::CMP(pc,X86::AddressX86<B16>(X86::esi,Disp<B8>(4) ),Imm<B16>('r'));
	pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
	labels.push_back(pc - code -1);
	
	//state <2
	pc = X86::MOV(pc,X86::eax,Imm<B32>(2));
	pc = X86::JMP(pc,Imm<B8>(0));
	rlabels.push_back(pc - code -1);
	
	//lael here
	for(int i=0;i<(int)labels.size();i++)
	{
		*(code + labels[i]) = (pc - code) - labels[i] - 1;
	}
	labels.clear();
	
	//tk.strLem==5?
	//pc = X86::MOV(pc,X86::ecx,X86::AddressX86<B32>(X86::ebx,Disp<B32>(Offset(Token<wchar_t>,strLen))));
	pc = X86::CMP(pc,X86::ecx,Imm<B8>(5));
	pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
	labels.push_back(pc - code -1);

	//tk.strVal = "while"
	pc = X86::MOV(pc,X86::esi,X86::ebx);
	pc = X86::CMP(pc,X86::AddressX86<B32>(X86::esi),Imm<B32>((((unsigned short)'h')<<16) + (unsigned short)'w'));
	pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
	labels.push_back(pc - code -1);
	pc = X86::CMP(pc,X86::AddressX86<B32>(X86::esi,Disp<B8>(4)),Imm<B32>((((unsigned short)'l')<<16) + (unsigned short)'i'));
	pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
	labels.push_back(pc - code -1);
	pc = X86::CMP(pc,X86::AddressX86<B32>(X86::esi,Disp<B8>(8) ),Imm<B16>('e'));
	pc = X86::JCC(pc,TCond::cNotEqual,Imm<B8>(0));
	labels.push_back(pc - code -1);
	
	//state <3
	pc = X86::MOV(pc,X86::eax,Imm<B32>(3));
	pc = X86::JMP(pc,Imm<B8>(0));
	rlabels.push_back(pc - code -1);
	
	//lael here
	for(int i=0;i<(int)labels.size();i++)
	{
		*(code + labels[i]) = (pc - code) - labels[i] - 1;
	}
	labels.clear();

	//false match
	for(int i=0;i<(int)flabels.size();i++)
	{
		*(code + flabels[i]) = (pc - code) - flabels[i] - 1;
	}
	flabels.clear();	
	pc = X86::MOV(pc,X86::eax,Imm<B32>(0));
	
	//rlael here
	for(int i=0;i<(int)rlabels.size();i++)
	{
		*(code + rlabels[i]) = (pc - code) - rlabels[i] - 1;
	}
	rlabels.clear();
	
	LeaveFrame(pc,0);
	
	typedef int (* Eval)(Token<wchar_t> & tk) ;
	Eval eval = (Eval)(void *)code;
	
	Token<wchar_t> tk;
	tk.kind=tkIdent;
	tk.strVal = L"while";
	tk.strLen = 5;
	printf("\nresult %d\n", eval(tk));
	
	VirtualFree(mbase,msize,MEM_RELEASE);
	
	return 1;
}

int main()
{
	std::vector<TMatchNode> matchNodes;
	std::vector<TStateNode> stateNodes;
	
	int prot = PAGE_EXECUTE_READWRITE ;
	int msize = 0x1000;

	LPVOID mbase = VirtualAlloc(NULL,
		msize,
		MEM_COMMIT | MEM_RESERVE,
		prot);
	if (mbase == NULL)
	{
		printf("\r\n%s","error call VirtualAlloc.");
		return 0;
	}
	

	unsigned char* code = (unsigned char*)mbase;
	unsigned char* pc = code;
	
	stateNodes.push_back(TStateNode());
	
	Token<wchar_t> tk;
	//tk.kind=tkIdent;
	//tk.strVal = L"for";
	//tk.strLen = wcslen(tk.strVal);
	//tk.kind = tkOperate;
	//tk.opVal = opLparen;
	tk.kind = tkInt;
	tk.intVal = 3;

	int matchNodeIndex = (int)matchNodes.size();
	matchNodes.push_back(TMatchNode(L"while",1));
	matchNodes.push_back(TMatchNode(L"for",2));
	matchNodes.push_back(TMatchNode(L"do",3));
	matchNodes.push_back(TMatchNode(L"if",4));
	
	TStateNode stateNode;
	stateNode.state = 1;
	stateNode.matchNodeIndex = matchNodeIndex;
	stateNode.matchNodeCount =  (int)matchNodes.size() - matchNodeIndex;
	stateNode.funcMatchEval = 0;
	stateNodes.push_back(stateNode);
	
	matchNodeIndex = (int)matchNodes.size();
	matchNodes.push_back(TMatchNode(opLparen,5));
	stateNode.state = 2;
	stateNode.matchNodeIndex = matchNodeIndex;
	stateNode.matchNodeCount =  (int)matchNodes.size() - matchNodeIndex;
	stateNode.funcMatchEval = 0;
	stateNodes.push_back(stateNode);

	matchNodeIndex = (int)matchNodes.size();
	matchNodes.push_back(TMatchNode(12,L"integer-literal"));
	matchNodes.push_back(TMatchNode(L"this",3));
	matchNodes.push_back(TMatchNode(opLparen,5));
	matchNodes.push_back(TMatchNode(4,L"identifier"));
	stateNode.state = 3;
	stateNode.matchNodeIndex = matchNodeIndex;
	stateNode.matchNodeCount =  (int)matchNodes.size() - matchNodeIndex;
	stateNode.funcMatchEval = 0;
	stateNodes.push_back(stateNode);

	int state = 3;
	while(true)
	{
		if(!stateNodes[state].funcMatchEval)
			genState(code,pc,state,matchNodes,stateNodes);
			
		state = stateNodes[state].funcMatchEval(tk);
		printf("\nresult %d\n", state);
		break;
	}
	//FuncMatchEval eval = (FuncMatchEval)(void *)code;
	/*Token<wchar_t> tk;
	tk.kind=tkIdent;
	tk.strVal = L"while";
	tk.strLen = 5;
	printf("\nresult %d\n", eval(tk));*/

	VirtualFree(mbase,msize,MEM_RELEASE);
}