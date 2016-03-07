#include <assert.h>
#include "term.hpp"

namespace trap {
namespace complier {

void RegisterTerm(LangContext & context, const wchar_t* term)
{
	context.aTerm.push_back(0);	
	context.cTerm.push_back(0);	
	context.sureTerm.push_back(0);
	context.mTerm.insert(std::pair<std::wstring ,  std::size_t>(term,context.aTerm.size() - 1));
}

const std::wstring & getKeywordName(LangContext & context, int kw)
{
	std::map<std::wstring , std::size_t>::const_iterator iter = context.mKeyword.begin();
	while(iter != context.mKeyword.end())
	{
		if(iter->second==kw) break;
		iter++;
	}
	return iter->first;
}

const std::wstring & getTermName(LangContext & context, int term)
{
	std::map<std::wstring , std::size_t>::const_iterator iter = context.mTerm.begin();
	while(iter != context.mTerm.end())
	{
		if(iter->second==term) break;
		iter++;
	}
	return iter->first;
}

void RegisterKeyword(LangContext & context, const wchar_t* kw)
{
	if(context.mKeyword.find(kw)!=context.mKeyword.end()) return;

	int kbase = 0x100;
	if(!context.aKeyword.size())
		context.aKeyword.push_back(kbase);
	else
		context.aKeyword.push_back(context.aKeyword.back() + 1);
	context.mKeyword.insert(std::pair<std::wstring ,  std::size_t>(kw,context.aKeyword.size() - 1));
}

void makeTerm(LangContext & context, const wchar_t* term, int  synTerms[])
{
	context.aTerm[context.mTerm[term]] = context.pc;
	int i = 0;
	while(synTerms[i] != synNill)
	{
		context.data[context.pc++] = synTerms[i];
		context.data[context.pc++] = 0; //place hole
		i++;
	}
	context.data[context.pc++] = synNill;
}

void makeSyntaxTerm(LangContext & context, const wchar_t* name)
{
	context.aTerm.push_back(0);
	context.mTerm.insert(std::pair<std::wstring ,  std::size_t>(name,context.aTerm.size() - 1));
	context.cTerm.push_back(0);
	context.sureTerm.push_back(0);
}

void setSyntaxTerm(LangContext & context, const wchar_t* name, int data[],  int length)
{
	context.aTerm[context.mTerm[name]] = context.pc;
	if(data[0]!=synVariant || wcscmp((const wchar_t*)data[1],L"$result")!=0)
	{
		context.data[context.pc++] = synVariant;
		context.data[context.pc++] = (int)L"$result";
		context.data[context.pc++] = (int)L"TNode*";
		context.data[context.pc++] = synNill;
	}
	int i = 0;
	while(i < length)
	{
		context.data[context.pc++] = data[i];
		i++;
	}
}

void makeSyntaxTerm(LangContext & context, const wchar_t* name, int data[],  int length)
{
	context.aTerm.push_back(context.pc);
	context.mTerm.insert(std::pair<std::wstring ,  std::size_t>(name,context.aTerm.size() - 1));
	context.cTerm.push_back(0);
	context.sureTerm.push_back(0);
	if(data[0]!=synVariant || wcscmp((const wchar_t*)data[1],L"$result")!=0)
	{
		context.data[context.pc++] = synVariant;
		context.data[context.pc++] = (int)L"$result";
		context.data[context.pc++] = (int)L"TNode*";
		context.data[context.pc++] = synNill;
	}
	int i = 0;
	while(i < length)
	{
		context.data[context.pc++] = data[i];
		i++;
	}
}

int getTermPc(LangContext & context, const wchar_t* name)
{
	return context.aTerm[context.mTerm[name]];
}

int getSynTermHole(LangContext & context, int term, TermSynKind synKind)
{
	int term_pc = context.aTerm[term];
	int pos = term_pc;
	while(context.data[pos] != synNill)
	{
		if(context.data[pos] == synKind) break;
		pos += 2;
	}
	if(context.data[pos] != synKind) return 0; //not found
	return pos + 1;
}

int getSynTermHole(LangContext & context, const wchar_t* term, TermSynKind synKind)
{
	return getSynTermHole(context,(int)context.mTerm[term],synKind);
}

void setSynTerm(LangContext & context, const wchar_t* term, TermSynKind synKind, int  items[])
{
	if(synKind != synNill)
	{
		int hole_pos = getSynTermHole(context, term, synKind);
		assert(hole_pos);
		context.data[hole_pos] = context.pc;
	}
	
	int i = 0;
	while(items[i] != synNill) 
	{
		context.data[context.pc++] = items[i];
		i++;
	}
	context.data[context.pc++] = synNill;
}

bool isPrimaryToken(const int data[], int pos)
{
	if(data[pos] < 255) return true;
	return false;
}

bool isOperatorToken(const int data[], int pos)
{
	if(data[pos] >= 0x10 && data[pos] <0xff) return true;
	return false;
}

int nextSyntaxToken(LangContext & context,int term, int pos)
{
	if(isOperatorToken(context.data,pos)) return pos+1;
	else if(isPrimaryToken(context.data,pos)) return pos+2;
	else if(context.data[pos]==synParam) return pos+2;
	else if(context.data[pos]==synSyntax) return pos+2;
	else if(context.data[pos]==synAssign) return pos+2;
	else if(context.data[pos]==synGuard) return pos+1;
	else if(context.data[pos]==synConstructor)
	{
		while(context.data[pos])pos++;
		return pos+1;
	}
	else if(context.data[pos]==synVariant)
	{
		while(context.data[pos])pos++;
		return pos+1;
	}
	else if(pos && context.data[pos] == synList)
	{
		pos++;
		//eat body
		while(context.data[pos] != synNill)pos = nextSyntaxToken(context, term, pos);
		pos++;
		//eat tail
		while(context.data[pos] != synNill)pos = nextSyntaxToken(context, term, pos);
		return pos + 1;
	}
	else if(pos && context.data[pos] == synOneOf)
	{
		pos++;
		while(true)
		{
			while(context.data[pos] != synNill)pos = nextSyntaxToken(context, term, pos);
			pos++;
			if(context.data[pos] == synNill)break;
		}
		assert(context.data[pos] == synNill);
		return pos + 1;
	}
	else if(pos && context.data[pos] == synOption)
	{
		pos++;
		while(context.data[pos] != synNill)pos = nextSyntaxToken(context, term, pos);
		return pos + 1; 
	}
	return pos+1;
}

int nextSyntaxPiece(LangContext & context,int term, int pos)
{
	while(context.data[pos] != synNill)
		pos = nextSyntaxToken(context, term, pos);
	return pos+1;
}

int getSyntaxFirstSet(LangContext & context,int term, int pos, std::set<int>& firstSet)
{
	if(pos && context.data[pos] == synList)
	{
		return getSyntaxFirstSet(context,term,pos+1, firstSet);
	}
	else if(pos && context.data[pos] == synParam)
	{
		int paramIndex = context.data[pos+1];
		int paramPos = context.data[context.data[getSynTermHole(context,term,synParam)] + paramIndex];
		if(pos == paramPos) return 1;//dead loop
		return getSyntaxFirstSet(context,term,paramPos, firstSet);
	}
	else if(pos && context.data[pos] == synOneOf)
	{
		pos++;
		while(true)
		{
			getSyntaxFirstSet(context,term,pos, firstSet);
			pos = nextSyntaxPiece(context,term,pos);
			if(context.data[pos] == synNill) break;
		}
		return 1;
	}
	else if(pos && context.data[pos] == synOption)
	{
		getSyntaxFirstSet(context,term,pos+1, firstSet);
		pos = nextSyntaxPiece(context,term,pos + 1);
		if(!context.data[pos] == synNill)
			getSyntaxFirstSet(context,term,pos, firstSet);
		return 1; 
	}
	else if(pos && context.data[pos] == synAssign)
	{
		getSyntaxFirstSet(context,term,pos+2, firstSet);
		return 1; 
	}
	else if(pos && context.data[pos] == synConstructor)
	{
		pos = nextSyntaxToken(context,term,pos);
		getSyntaxFirstSet(context,term,pos, firstSet);
		return 1; 
	}
	else if(pos && context.data[pos] == synVariant)
	{
		pos = nextSyntaxToken(context,term,pos);
		getSyntaxFirstSet(context,term,pos, firstSet);
		return 1; 
	}
	else if(pos && context.data[pos] == synGuard)
	{
		pos = nextSyntaxToken(context,term,pos);
		getSyntaxFirstSet(context,term,pos, firstSet);
		return 1; 
	}
	else if(pos && context.data[pos] == synSyntax)
	{
		pos = context.aTerm[context.data[pos +1]];
		getSyntaxFirstSet(context,term,pos, firstSet);
		return 1;
	}
	else
		firstSet.insert(pos);
	return 1;
}

}//complier
}//trap

