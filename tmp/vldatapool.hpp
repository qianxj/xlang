#ifndef WPP5_vlDataPool_H
#define WPP5_vlDataPool_H

#include <vector>

namespace xlang  {

class vlDataPool
{
public:
	vlDataPool();
	~vlDataPool();
public:
	void* append_data(const void * ptr_data, int size);

private:
	typedef struct ptr_data_size
	{
		void * ptr_;
		int    size_;
		ptr_data_size(void * ptr,int size):ptr_(ptr),size_(size){}
	} ptr_data_size_t;
private:
	std::vector<ptr_data_size_t>* space_alloc_;
	std::vector<ptr_data_size_t>* space_free_;
private:
	void * ensure_space(int size);
	void * alloc_space(int size);
private:
	//统计数据
	int data_count_; //append data count
	int data_used_;	//total data used
	int alloc_size_;	//total alloc size
	int free_size_;	//total free size 
};

} //namespace xlang

#endif //WPP5_vlDataPool_H