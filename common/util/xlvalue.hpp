#ifndef XLVALUE_H
#define XLVALUE_H

#include <locale.h>
#include <string>

namespace xl {
namespace util{ namespace misc {

//value with type	
struct TValue
{
	enum  : char
	{
		Unknown	=0,
		UInt64		=1,
		Int64			=2,
		Int32			=3,
		UInt32		=4,
		Int16			=5,
		UInt16		=6,
		Int8			=7,
		UInt8			=8,
		Double80	=9,
		Double64	=10,
		Double32	=11, 
		WString		=12,
		String		=13,
		Bool			=14,
		Ptr			=15,
		
		WChar		=16,
		Char			=18,
		UChar		=19,
		MULChar		=20
	} vt;

	union 
	{
		long long int					val_i64;
		unsigned long long int	val_ui64;
		int								val_i32;
		unsigned int					val_ui32;
		short							val_i16;
		unsigned short				val_ui16;
		double							val_d64;
		float								val_d32;
		const wchar_t*				val_wstr;
		const char*					val_str;
		wchar_t						val_wchar;
		char								val_char;
		unsigned char				val_uchar;
		char								val_i8;
		unsigned char				val_ui8;
		bool								val_bool;
		void *							val_ptr;
	};
	
	inline TValue(long long int val) : 				val_i64(val), 		vt(Int64){}
	inline TValue(unsigned long long int val) : val_ui64(val), 	vt(UInt64){}
	inline TValue(int val) :  							val_i32(val), 		vt(Int32){}
	inline TValue(unsigned int val) : 				val_ui32(val),		vt(UInt32){}
	inline TValue(short val) : 						val_i16(val), 		vt(Int16){}
	inline TValue(unsigned short val) :  			val_ui16(val), 		vt(UInt16){}
	inline TValue(double val) : 						val_d64(val), 		vt(Double64){}
	inline TValue(float val) : 							val_d32(val), 		vt(Double32){}
	inline TValue(const wchar_t* val) : 			val_wstr(val), 	vt(WString){}
	inline TValue(const char* val) :				val_str(val), 		vt(String){}
	inline TValue(wchar_t val) :						val_wchar(val), 	vt(WChar){}
	inline TValue(char val) :							val_char(val), 	vt(Char){}
	inline TValue(unsigned char val) : 			val_char(val), 	vt(UChar){}
	inline TValue(bool val) :							val_bool(val), 		vt(Bool){}
	inline TValue(void * val) :						val_ptr(val), 		vt(Ptr){}
	inline TValue(wchar_t val[], int size) :		val_wstr(val), 	vt(MULChar){}
	
	std::wstring toString();
	//char or string with quote
	std::wstring toStringEx();
};


}}	//namespace util::misc

} //namespace xl

#endif //XLVALUE_H