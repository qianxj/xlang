#ifndef WPP5_HASHSTRING_H
#define WPP5_HASHSTRING_H

#include "vlStringPool.hpp"
#include "vlang5.h"

#include <vector>
#include <deque>

#define  STR_BUCKET_SIZE 4096

namespace xlang {

using namespace std;

struct vlCHashStringItem
{
	wchar_t * hItem;
	int next;
};

class  VLANG_API vlHashString
{
public:
	vlHashString(void);
	~vlHashString(void);
private:
	vlStringPool * pool_string_;
	int _bucketsize;
private:
#pragma warning( push )
#pragma warning( disable : 4251 )
	std::deque<vlCHashStringItem> _bucket;
#pragma warning( pop )
	int _count;
public:
	int get_count();
public:
	int hash_value(const wchar_t* str);
	int hash_value(const wchar_t* str,int size);
public:
	int hash_token(const wchar_t* str,int size);
	int hash_token(const wchar_t* str);
	int find_token(const wchar_t* str,int size);
	int find_token(const wchar_t* str);
public:
	wchar_t* get_string(int nIndex);
};

}  // namespace xlang

#endif //WPP5_HASHSTRING_H
