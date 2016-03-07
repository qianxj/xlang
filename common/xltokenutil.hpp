#ifndef XLTOKENUTIL_H
#define XLTOKENUTIL_H

namespace xl {
namespace util { namespace token {

enum TokenKind : char
{
	tkEof = 0,
	tkUnknown = 1,
	tkIdent =2,
	tkKeyword =3,
	tkInt	=4,
	tkUInt	=5,
	tkDouble = 6,
	tkFloat = 7,
	tkString = 8,
	tkChar = 9,
	tkBool = 10,
	tkOperate = 11,
	tkHex = 12,
	
	tkComment = 32,
	tkBlockComment = 33,
	tkBlank = 34,
	tkLn	 = 35
};
	
enum Operate : char
{
	//unknown
	opUnknown		= 0x0,
	
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
	
inline bool isOperate(TokenKind kind)		{	return kind == tkOperate; }
inline bool isIdent(TokenKind kind)				{	return kind == tkIdent;	}
inline bool isString(TokenKind kind)				{	return kind == tkString;	}
inline bool isBool(TokenKind kind)				{	return kind == tkBool;		}
inline bool isChar(TokenKind kind)				{	return kind == tkChar;	}
inline bool isInt(TokenKind kind)				{	return kind == tkInt;	}
inline bool isDouble(TokenKind kind)				{	return kind == tkDouble || kind == tkFloat;	}
inline bool isFloat(TokenKind kind)				{	return kind == tkFloat;	}
inline bool isNumber(TokenKind kind)			
	{	return kind == tkDouble|| kind == tkFloat || kind == tkChar || kind == tkInt || kind == tkHex; }
inline bool isLiteral(TokenKind kind)			{	return kind == tkString || kind == tkBool || isNumber(kind);	}
inline bool isEof(TokenKind kind)				{	return kind == tkEof;	}
inline bool isComment(TokenKind kind)			{	return kind == tkComment || kind == tkBlockComment;	}
inline bool isLn(TokenKind kind)				{	return kind == tkLn;	}
inline bool isBlank(TokenKind kind)				{	return kind == tkBlank;	}

	
bool isAsnOperate(Operate op );
bool isMathOperator(Operate op) ;
bool isBitOperator(Operate op) ;
bool isRelationOperator(Operate op);
bool isGroupOperator(Operate op);

bool isSeparateOperator(Operate op);
bool isUnaryOperator(Operate op);
bool isPostfixOperator(Operate op);
Operate unAsnOperator(Operate op);

Operate  GetOperate(const wchar_t* op);
const wchar_t* getOperateText(Operate op);

}} //util::token

} //namespace xl

#endif  //XLTOKENUTIL_H
