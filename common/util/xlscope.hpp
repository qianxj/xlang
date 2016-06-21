#ifndef XLSCOPE_H
#define XLSCOPE_H

#include <stack>
#include <vector>
#include <deque>

#include "xlstringpool.hpp"

namespace xl  {
	

class Scope
{
public:
	struct  Item
	{
		union
		{
			int nWord;
			void* ptr;
		};
		HSymbol ide;
		int next;
		int piror; //when <0 point to _bucket >0 point to _items
	};

public:
	Scope(void);
	~Scope(void);
 
#pragma warning( push )
#pragma warning( disable : 4251 )
private:
	//indexs in current scope block
	std::vector<int> currents;
	//count indexs in current scope block
	std::stack<int> countCurrents;
	//words used last
	//vector<int>	_hots;

private:
	int bucketsize;
private:
	int count;
	std::vector<int> bucket;
	std::deque<Item> items;
	//freelist
	std::stack<int> freelist;
#pragma warning( pop ) 

private:
	StringPool* stringPool;
public:
	void SetStringPool(StringPool* stringPool);
	StringPool* GetStringPool();
public:
	int GetCount();
public:
	int HashCode(const wchar_t* str);
public:
	void EnterScope();
	//leave scope should pop up words in this scope
	void LeaveScope();
public:
	int Push(int  w,HSymbol ide);
	int Push(int  w,const wchar_t* str);
	int Push(void*  w,HSymbol ide);
	int Push(void* w,const wchar_t* str);
public:
	 void* LookupCurrentScope(HSymbol ide);
	 //return index + 1 of _bucket
	 int FindToken(HSymbol ide);
	 int FindToken(const wchar_t* str);
	 //return next word's index + 1 of _bucket
	 int FindNextToken(int nIndex,HSymbol ide);
	 int FindNextToken(int nIndex,const wchar_t* str);
public:
	//get word index;
	int GetWord(int nIndex);
	void* GetPtr(int nIndex);
};

	
} //namespace xl

#endif //XLSCOPE

