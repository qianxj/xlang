#ifndef XLELEMENTBITVECTOR_H
#define XLELEMENTBITVECTOR_H

#include "xlbitvector.hpp"

namespace xl {

class BitElementVector
{
private:
	int* data;
	int length;
	int dataLength;
	int elementLength;
	int elementDataLength;
public:
	explicit BitElementVector(int elementLength,int length);
	
	BitElementVector(const BitElementVector& other);

	~BitElementVector();

	static int SizeFor(int elementLength,int length);

	BitElementVector& operator=(const BitElementVector& rhs);

	void CopyFrom(const BitElementVector& other);

	inline bool Get(int element, int i) const ;
	inline void Add(int element, int i,bool b) ;
	void Remove(int element, int i);
	void Union(int element1,int element2);
	void Union(int element1,BitElementVector & other, int element2) ;
	void UnionTo(int element1,BitVector & other) ;
	bool UnionIsChanged(int element1,int element2);

	void Intersect(int element1,int element2);
	void Subtract(int element1,int element2);
	void Clear();

	bool IsEmpty() const ;

	bool IsEmpty(int element) const ;

	bool Equals(int element1,int element2);

	void TransmitClosure(int pSorts[],int &count);
	
	inline int GetLength() const { return length; }
	inline int GetElementLength() const {return elementLength;}

};

} //namespace xl

#endif //XLELEMENTBITVECTOR_H
	