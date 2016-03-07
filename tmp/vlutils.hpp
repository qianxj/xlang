#ifndef WPP5_WPPUTIL_H
#define WPP5_WPPUTIL_H

#include "vlwppstdint.hpp"
#include <string.h>
#include "vlhashstring.hpp"

using namespace xlang ;

namespace util
{
	
	inline int round_up(int x, int m) 
	{  return (x + m - 1) & ~(m - 1); }

	inline int round_down(int x, int m) 
	{  return x & ~(m - 1); }

	template <typename T>
	static inline bool IsPowerOf2(T x)
	{
	  return (((x) & ((x) - 1)) == 0)
	}

	template <typename T>
	static inline int /*intptr_t*/ OffsetFrom(T x)
	{
	  return x - static_cast<T>(0);
	}

	template <typename T>
	static inline T AddressFrom(int/*intptr_t*/ x)
	{
	  return static_cast<T>(static_cast<T>(0) + x);
	}

	template <typename T>
	static inline T RoundDown(T x, int m)
	{
	  return AddressFrom<T>(OffsetFrom(x) & -m);
	}


	template <typename T>
	static inline T RoundUp(T x, int m)
	{
	  return RoundDown(x + m - 1, m);
	}

	wstring_t UnXMLText(const wstring_t str);
	wstring_t ToXmlText(const wstring_t str);
	void FreeXmlText(wstring_t str);

	bool TrimString(wstring_t lpsz,wstring_t lpFilter);


	vlHashString* getGlobalHashString();
	int hashString(const wstring_t str,int size = 0);
	const wstring_t getHashString(int h);

}

class vlDynVector
{
public:
	vlDynVector();
	~vlDynVector();
public:
	unsigned char * getElement(int index);
	unsigned char * newElemnt();
	unsigned char * newElemnt(int count);
	void pop();
public:
	inline int	getLength() {return length;}
	inline void setLength(int nlength) {length = nlength;};
	inline void setEleSize(int size) {eleSize = size;}
	inline int	getEleSize() { return eleSize;}
public:
	unsigned char * getData(){return data;} 
private:
	unsigned char * data; 
	int length;
	int eleSize;
private:
	int growth_factor;
	int capacity;
};

class vlTapeChar
{
public:
	vlTapeChar();
	~vlTapeChar();
public:
	void Enter(int v);
	void Enter(double v);
	void Enter(wchar_t v);
	void Enter(short v);
	void Enter(char v);
	void Enter(unsigned char data[],int len);
	void Enter(vlTapeChar * tp);
public:
	inline unsigned char * getData(){return data? data->getData(): 0;}
	inline int getLength(){ return length;}
private:
	int length;
	vlDynVector * data;
};


#endif //WPP5_WPPUTIL_H