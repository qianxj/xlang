#ifndef WPP5_VLENTRYNODE_H
#define WPP5_VLENTRYNODE_H

#include <vector>
#include <map>
using namespace std;
#include <wtypes.h>
#include "vlwppstdint.hpp"

namespace xlang{

struct EntryNode;
enum TyOfValue
{
	ty_int		= VT_INT,
	ty_str		= VT_LPWSTR,
	ty_astr		= VT_LPSTR,
	ty_nill		= VT_NULL,
	ty_empty	= VT_EMPTY,
	ty_bool		= VT_BOOL,
	ty_ptr		= VT_PTR,
	ty_uint		= VT_UINT,
	ty_int64	= VT_I8,
	ty_uint64	= VT_UI8,
	ty_double	= VT_R8,
	ty_byte		= VT_I1,
	ty_char		= VT_I2,
};

struct TValue
{
	char ty;
	char unused[3];
	union
	{
		bool		bval;
		uint_t		uval;
		int_t		val;
		double_t	dval;
		ptr_t		ptr;
		wstring_t	sval;
		int64_t		val64;
		uint64_t	uval64;
		struct
		{
			int_t	hval;
			int_t	lval;
		};
	};

	TValue():ty(ty_empty),hval(0),lval(0){}
	TValue(int_t v):ty(ty_int),val(v){}
	TValue(uint_t v):ty(ty_uint),uval(v){}
	TValue(int64_t v):ty(ty_int64),val64(v){}
	TValue(uint64_t v):ty(ty_uint64),uval64(v){}
	TValue(double_t v):ty(ty_double),dval(v){}
	TValue(float_t v):ty(ty_double),dval(v){}
	TValue(ptr_t v):ty(ty_ptr),ptr(v){}
	TValue(int lval,int hval) : ty(ty_int64),lval(lval),hval(hval){}
	TValue(wchar_t* v):ty(ty_str),sval(v){}
	TValue(const wchar_t* v):ty(ty_str),sval((wstring_t)v){}
};

enum Test_kind
{
  undeftest = 0,
  nametest = 1,
  literaltest =2,
  counttest = 3,
  condtest = 4,
  contexttest = 5,
  fnametest = 6,
  anytest =2,
  matchtest = 7
};

class OneOfList
{
public:
	virtual int addItem(TValue, EntryNode* node) = 0;
	virtual EntryNode * findItem(TValue) = 0;
};

class OneOfNameList : public OneOfList
{
public:
	map<int,EntryNode *> items;
public:
	int addItem(TValue v, EntryNode* node);
	EntryNode * findItem(TValue v);
};

class OneOfMatchList : public OneOfList
{
public:
	vector<pair<TValue,EntryNode *>> items;
public:
	int addItem(TValue v, EntryNode* node);
	EntryNode * findItem(TValue v);
};

struct OneOfCountList : public OneOfList
{
public:
	vector<pair<int,EntryNode *>> items;
public:
	int addItem(TValue v, EntryNode* node);
	EntryNode * findItem(TValue v);
};

struct EntryNode
{
	char test_kind;
	OneOfList * items;
};

}

#endif VLENTRYNODE_H