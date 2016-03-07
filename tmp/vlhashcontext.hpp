#ifndef WPP5_HASHCONTEXT_H
#define WPP5_HASHCONTEXT_H

#include "vlang5.h"
#include "vlhashstring.hpp"
#include "vlglobal.hpp"

#include <stack>
#include <vector>
#include <deque>
using namespace std;


namespace xlang  {

struct VLANG_API vlCHashContextItem
{
	union
	{
		int_t nWord;
		ptr_t ptr;
	};
	int_t ide;
	int_t next;
	int_t piror; //when <0 point_t to _bucket >0 point_t to _items
};

class VLANG_API vlHashContext
{
public:
	vlHashContext(void);
	~vlHashContext(void);
 
#pragma warning( push )
#pragma warning( disable : 4251 )
private:

	//indexs in current scope block
	vector<int_t> _currents;
	//count indexs in current scope block
	stack<int_t> _countCurrents;
	//words used last
	//vector<int_t>	_hots;

private:
	int_t _bucketsize;
private:
	int_t _count;
	vector<int_t> _bucket;
	deque<vlCHashContextItem> _items;
	//freelist
	stack<int_t> _freelist;
#pragma warning( pop ) 

private:
	vlHashString* _hashString;
public:
	void set_hash_string(vlHashString* hashString);
	vlHashString* get_hash_string();
public:
	int_t get_count();
public:
	int_t hash_value(const wchar_t* str);
public:
	void enter_scope();
	//leave scope should pop up words in this scope
	void leave_scope();
public:
	int_t push_word(int_t  w,int_t ide);
	int_t push_word(int_t  w,const wstring_t str);
	int_t push_word(ptr_t  w,int_t ide);
	int_t push_word(ptr_t  w,const wstring_t str);
public:
	 ptr_t lookup_currentscope(int ide);
	 //return index + 1 of _bucket
	 int_t find_token(int ide);
	 int_t find_token(const wstring_t str);
	 //return next word's index + 1 of _bucket
	 int_t find_next_token(int_t nIndex,int_t ide);
	 int_t find_next_token(int_t nIndex,const wstring_t str);
public:
	//get word index;
	int_t get_word(int_t nIndex);
	ptr_t get_ptr(int_t nIndex);
};

}// namespace xlang


#endif //WPP5_HASHCONTEXT_H