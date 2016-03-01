#ifndef XLTOKEN_H
#define XLTOKEN_H

namespace XL
{

struct SrcPos
{
	int row;
	int col;
};

template<class CharType >
struct Token
{
	enum Kind : char
	{
		kEof = 0,
		kIdent =1
		kKeyword =2,
		kInt	=3,
		kUInt	=4,
		kDouble = 5,
		kFloat = 6,
		kString = 7,
		kChar = 8,
		kBool = 9,
		kOperate = 10,
		kHex = 11,
		
		kCommnt = 32,
		kBlockComment = 33,
		kBlank = 34,
		kLn	 = 35
	};
	
	enum Operate : char
	{
		//math operator
		opAdd			= 0x21,
		opSub			= 0x22,
		opMul			= 0x23,
		opDiv			= 0x24,
		opMod			= 0x25,
		
		//unary
		opAsn			= 0x26,
		opInc			= 0x27,
		opDec			= 0x28,
		
		//bit operator
		opShl			= 0x2A,
		opShr			= 0x2B,
		opBand			= 0x2C,
		opBor			= 0x2D,
		opXor			= 0x2E,
		opBnot			= 0x2F,
		
		//logic operator
		opOr			= 0x30,
		opAnd			= 0x31,
		opGt			= 0x32,
		opGte			= 0x33,
		opLt			= 0x34,
		opLte			= 0x35,
		opEq			= 0x36,
		opNeq			= 0x37,
		opNot			= 0x38,
		
		opThis			= 0x3A,
		opSuper			= 0x3B,

		//group operator
		opRparen		= 0x40,
		opLparen		= 0x41,
		opRbrace		= 0x42,
		opLbrace		= 0x43,
		opRbrackets		= 0x44,
		opLbrackets		= 0x45,
		opComma			= 0x46,
		opSemicolon		= 0x47,
		opSign		 	= 0x48,
		opDollar		= 0x49,
		
		opApos			= 0x4A,
		opPath2			= 0x4B,
		opSizeof		= 0x4C,
		
		opNest			= 0x50,
		opPointer  		= 0x51,
		opDot3	    		= 0x52,
		opDot2	    		= 0x53,
		opIn	    		= 0x54,
		opMref			= 0x55,
		opMpointer 		= 0x56,
		opVarrow   		= 0x57,
		opBecomes 		= 0x58,
		opQuery			= 0x59,
		opColon			= 0x5A,
		opHash			= 0x5B,
		opRef			= 0x5C,
		opBackslash		= 0x5D,
		
		//math asn operator
		opAddAsn		= 0x71,
		opSubAsn		= 0x72,
		opMulAsn		= 0x73,
		opDivAsn		= 0x74,
		opModAsn		= 0x75,
		
		//bit asn operator
		opShlAsn		= 0x7A,
		opShrAsn		= 0x7B,
		opBandAsn		= 0x7C,
		opBorAsn		= 0x7D,
		opXorAsn		= 0x7E,
		opBnotAsn		= 0x7F,
		
		opExclam   		= opNot,
		opPercent  		= opMod,
		opDot			= opRef,
		opPeriod		= opRef,
		opArrow	    		= opPointer,
		opPath			= opDiv		
	};
	
	//token kind
	Kind kind;
	
	//source index
	int cc;
	//prev source index
	int prevcc;
	//token start index
	int startcc;
	
	//token len
	int tokenLen; 
	//string token len;
	int strLen;
	
	union  
	{
		int 		intVal;
		double 		doubleVal;
		bool 		boolVal;
		CharType	charVal;
		Operate		opVal;
	};
	
	CharType* 	strValue;
	
	//method
	bool IsOperate(Operate op)		{	return kind==kOperate &&  opVal == op;	}
	bool IsIdent()				{	return kind==kIdent;	}
	bool IsString()				{	return kind==kString;	}
	bool IsBool()				{	return kind==kBool;		}
	bool IsChar(CharType ch)		{	return kind==kChar && strVal[0] == ch;	}
	bool IsChar()				{	return kind==kChar;	}
	bool IsInt()				{	return kind==kInt;	}
	bool IsDouble()				{	return kind==kDouble || kind ==kFloat;	}
	bool IsFloat()				{	return kFloat;	}
	bool IsNumber()				{	return kind==kDouble|| kind==kFloat || kind==kChar || kind==kInt || kind==kHex; }
	bool IsLiteral()			{	return kind==kString|| kind==kBool|| IsNumber();	}
	bool IsEof()				{	return kind ==kEof;	}
	bool IsComment()			{	return kind==kComment||kind==kBlockComment;	}
	bool IsLn()				{	return kind==kLn;	}
	bool IsBlank()				{	return kind==kBlank;	}

	bool IsIdent(CharType* Ident)
	{
		if(kind != kIdent) return false;
		int pos = 0;
		while(Ident[pos] == strVal[pos] && pos<=strLen)pos++; 
		return !Ident[pos] && pos == strLen + 1;
	}
	
	bool IsString(CharType* str)
	{
		if(kind != kString) return false;
		int pos = 0;
		while(Ident[pos] == strVal[pos] && pos<=strLen)pos++; 
		return !Ident[pos] && pos == strLen + 1;
	}
	
	bool IsAsnOperate()
	{
		return Kind==kOperate && (
			opVal==kAsn 			|| 
			opVal==opAddAsn	||
			opVal==opSubAsn	||
			opVal==opMulAsn	||
			opVal==opDivAsn	||
			opVal==opModAsn	||
			//bit asn operator
			opVal==opShlAsn	||
			opVal==opShrAsn	||
			opVal==opBandAsn  ||
			opVal==opBorAsn	||
			opVal==opXorAsn	||
			opVal==opBnotAsn  ) ;
	}
	
	static GetOperateByToken(CharType ch);
	static GetOperateByString(const CharType* str);
	static CharType* getOperateText(Operate op);
};


} //namespace XL

#endif  //XLTOKEN_H
