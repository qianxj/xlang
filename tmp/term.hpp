#ifndef WPP_TERM_H
#define WPP_TERM_H

#include <vector>
#include <set>
#include <map>
#include <list>
#include "wpcacher.hpp"
#include "wpsourcer.hpp"
#include "wpscan.hpp"
#include "wpast.hpp"
#include "wputil.hpp"

namespace trap {
namespace complier {

struct LangContext
{
	//scan
	//Scan * scan;
	//term
	std::map<std::wstring , std::size_t>  mTerm;
	std::vector<int> aTerm;
	std::vector<coder::astNode*> cTerm;
	std::vector<int> sureTerm;

	//keyword
	std::map<std::wstring , std::size_t> mKeyword;
	std::vector<int> aKeyword;	
	//data
	int * data;
	//pc
	int pc;
	//code ctx
	coder::astContext* ctx;

	//analyse
	std::vector<int> spendingSyntaxTerms;
	int spendingSyntaxTermIndex;
	//
	std::vector<std::wstring> variants;
	std::vector<std::wstring> variantsTypes;
	std::vector<coder::astNode*> variantsInitials;
//public:
//	std::list<Token> tokens;
};

enum TermSynKind
{
	synNill				= 0x0,
	synOneOf 			= 0x201,
	synOption 			= 0x202,
	synRepeat 			= 0x203,
	synList				= 0x205,
	

	synSyntax 			= 0x208,
	synConstructor 		= 0x209,
	synState			= 0x20A,
	synError 			= 0x20B,
	synProc				= 0x20C,
	synJmp				= 0X20D,
	synSemantic			= 0x20E,
	
	synUntil	 		= 0x210,
	synSkip				= 0x211,
	
	synContainer		= 0x212,
	synCompose			= 0x213,
	synParam			= 0x214,
	synVariant			= 0x215,

	synLiteral			= 0x216,
	synAssign			= 0x217,

	synGuard			= 0x218,
	synSure				= 0x219
};

void RegisterTerm(LangContext & context, const wchar_t* term);
const std::wstring & getKeywordName(LangContext & context, int kw);
const std::wstring & getTermName(LangContext & context, int term);
void RegisterKeyword(LangContext & context, const wchar_t* kw);

void makeTerm(LangContext & context, const wchar_t* term, int  synTerms[]);
void makeSyntaxTerm(LangContext & context, const wchar_t* name);
void setSyntaxTerm(LangContext & context, const wchar_t* name, int data[],  int length);
void makeSyntaxTerm(LangContext & context, const wchar_t* name, int data[],  int length);
int getTermPc(LangContext & context, const wchar_t* name);
int getSynTermHole(LangContext & context, int term, TermSynKind synKind);
int getSynTermHole(LangContext & context, const wchar_t* term, TermSynKind synKind);
void setSynTerm(LangContext & context, const wchar_t* term, TermSynKind synKind, int  items[]);
bool isPrimaryToken(const int data[], int pos);
bool isOperatorToken(const int data[], int pos);
int nextSyntaxToken(LangContext & context,int term, int pos);
int nextSyntaxPiece(LangContext & context,int term, int pos);
int getSyntaxFirstSet(LangContext & context,int term, int pos, std::set<int>& firstSet);

}//complier
}//trap

#endif //WPP_TERM_H