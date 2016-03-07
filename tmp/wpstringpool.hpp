#ifndef WPP_STRINGPOOL_H
#define WPP_STRINGPOOL_H

#include <vector>
#include "wpdatapool.hpp"

namespace trap {
namespace complier {
	
typedef struct StringEntry{
	wchar* str_;
	StringEntry* next_;
} * HStringEntry;

typedef HStringEntry HSymbol;

class StringPool
{
public:
	StringPool() ;
	~StringPool();
public:
	HStringEntry insert(wstring_t str) ;
	HStringEntry insert(wstring_t str, int size);

public:
	wstring_t getString(HStringEntry  h);
private:
	int hashCode(wstring_t str);
	int hashCode(wstring_t str, int size) ;
private:
	DataPool * data_pool_;
	int bucketsize_;
	HStringEntry*  buckets_;
};


}	//namespace trap
} //namespace ctrap

#endif //WPP_STRINGPOOL_H