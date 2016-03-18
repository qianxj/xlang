#ifndef XLDATAPOOL_H
#define XLDATAPOOL_H

#include <vector>

namespace xl {
	
struct DataPool
{
public:
	DataPool();
	~DataPool();
public:
	void* 		Append(const void * ptrData, int size);
	void* 		Alloc( int size) ;
	int* 			Append(int v) ;
	double* 	Append(double v);
	wchar_t*  Append(wchar_t* str, int len) ;
	wchar_t*  Append(wchar_t* str);
private:
	struct PtrDataSize {
		void * ptr;
		int    size;
		PtrDataSize(void * ptr,int size):ptr(ptr),size(size){}
	};
private:
	std::vector<PtrDataSize>* spaceAlloced;
	std::vector<PtrDataSize>* spaceFree;
private:
	void * ensureSpace(int size);
	void * allocSpace(int size);
private:
	//统计数据
	int dataCount; //append data count
	int dataUsed;	//total data used
	int allocSize;	//total alloc size
	int freeSize;	//total free size 
};


}	//namespace xl


#endif //XLDATAPOOL_H