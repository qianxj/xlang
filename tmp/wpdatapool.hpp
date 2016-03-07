#ifndef WPP_DATAPOOL_H
#define WPP_DATAPOOL_H

#include <vector>
#include "wptoken.hpp"

namespace trap {
namespace complier {
	
struct DataPool
{
public:
	DataPool();
	~DataPool();
public:
	void* append(const void * ptr_data, int size);
	void* alloc( int size) ;
	int* append(int v) ;
	double* append(double v);
	wstring_t  append(wstring_t str, int len) ;
	wstring_t  append(wstring_t str);
private:
	typedef struct ptr_data_size {
		void * ptr_;
		int    size_;
		ptr_data_size(void * ptr,int size):ptr_(ptr),size_(size){}
	} ptr_data_size_t;
private:
	std::vector<ptr_data_size_t>* space_alloc_;
	std::vector<ptr_data_size_t>* space_free_;
private:
	void * ensureSpace(int size);
	void * allocSpace(int size);
private:
	//统计数据
	int data_count_; //append data count
	int data_used_;	//total data used
	int alloc_size_;	//total alloc size
	int free_size_;	//total free size 
};


}	//namespace trap
} //namespace ctrap

#endif //WPP_DATAPOOL_H