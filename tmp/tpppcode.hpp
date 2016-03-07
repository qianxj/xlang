#ifndef STPP_PCODE_H
#define STPP_PCODE_H

#include <vector>
#include "vltoken.hpp"
#include "vlutils.hpp"
#include "vlsym.hpp"
#include "tppsymnode.hpp"

using namespace xlang;

namespace trap {
namespace complier {

namespace PCode
{
	enum Code
	{
		cJCC		= 0x01,
		cJMP		= 0x02,
		cLDA		= 0x03,
		cLD		= 0x04,
		cST		= 0x05,
		cADD		= 0x06,
		cSUB		= 0x07,
		cMUL		= 0x08,
		cDIV		= 0x09,
		cSHR		= 0x0A,
		cSHL		= 0x0B,
		cXOR		= 0x0C,
		cSADD	= 0x0D,
		cNOP		= 0x11,
		cBR		= 0x12,

		cRET		= 0x13,
		cLET		= 0x14,
		cCALL	= 0x18,
		cVCALL	= 0x19,
		cNCALL	= 0x1A,
		cPUSH	= 0x1B,
		cPOP		= 0x1C,

		cBIND	= 0x15,
		cUNBIND	= 0x16,
		cSPILL	= 0x17,


		cNEQ		= 0x20,
		cEQ		= 0x21,
		cLT		= 0x22,
		cLTE		= 0x23,
		cGT		= 0x24,
		cGTE		= 0x25,
		cNOT		= 0x26,
		cCAST	= 0x28,

		cFIELD	= 0x30,
		cELEMENT = 0x31,

		//PARAM	= 0x32,
		//VAR		= 0x33,
		//TMP		= 0x34,

		cBNOT	= 0x40,
		cBOR	= 0x41,
		cBAND	= 0x42,
		cBLOCK	= 0x43,

		cINC	= 0x44,
		cDEC	= 0x45,

		cNEW	= 0x46,
		cDEL	= 0x47,
		cNEG	= 0x48,

		cUINC	= 0x49,
		cUDEC	= 0x4A,
		cVRET	= 0x4B,

		cThis	= 0x80,
		cNill	= 0x81,
		cLiteral= 0x82,
		cItem	= 0x83,
		cImm	= 0x84,
		cBlock	= 0x85

	};

	struct Oprand;

	struct Oprand
	{
		Code cd;
		Oprand(Code cd):cd(cd){}
	};

	struct Block : public Oprand
	{
		std::vector<Oprand*> stmts;
		std::vector<int> jmpSrc;
		int startpc;
		inline Block & operator +=(Oprand* node){ stmts.push_back(node); return *this;}
		Block():Oprand(cBlock),startpc(0){}
	};

	/*
	struct Address : public Oprand
	{
	};

	struct Register :  public Oprand
	{
	};
	*/


	struct Imm : public Oprand
	{
		enum ValueType
		{
			I64 = 1,
			I32 = 2,
			I16 = 3,
			I8	= 4,
			UI64 = 5,
			UI32 = 6,
			UI16 = 7,
			UI8  = 8,
			D64 = 9,
			D32 = 10,
			STR = 11,
			ASTR = 12,
			BOOL = 13,
			PTR  = 14,
			WCHAR = 15
		};

		ValueType vt;
		union 
		{
			__int64				val_i64;
			int					val_i32;
			short				val_i16;
			char				val_i8;
			unsigned __int64	val_ui64;
			unsigned int		val_ui32;
			unsigned short		val_ui16;
			unsigned char		val_ui8;

			double				val_d64;
			float				val_d32;

			char *				val_str;
			wchar_t*			val_wstr;

			bool				val_bool;
			void *				val_ptr;
		};
		Imm():Oprand(cImm){}
	};

	struct Item : public Oprand
	{
		int tempId;
		xlang::Handle hTerm;
		Item(xlang::Handle & hTerm,int tempId=0):Oprand(cItem),hTerm(hTerm),tempId(tempId){}
	};

	struct Param	 : public Item
	{
		Param(HParam hParam) : Item(hParam){}
	};
	struct Var	 : public Item
	{
		Var(HVariant hVar) : Item(hVar){}
	};
	struct Tmp		 : public Item
	{
		Tmp(int tempId) : Item(Handle(),tempId){}
	};

	struct This : public Oprand
	{
		This():Oprand(cThis){}
	};

	struct Nill : public Oprand
	{
		Nill():Oprand(cNill){}
	};

	struct Ins : public Oprand
	{
		Ins(Code cd) : Oprand(cd){}
	};

	struct INC : public Ins
	{
		Oprand* opr;
		INC(Oprand * opr):Ins(cINC),opr(opr){}
	};

	struct DEC : public Ins
	{
		Oprand* opr;
		DEC(Oprand * opr):Ins(cDEC),opr(opr){}
	};
	struct UINC : public Ins
	{
		Oprand* opr;
		UINC(Oprand * opr):Ins(cUINC),opr(opr){}
	};

	struct UDEC : public Ins
	{
		Oprand* opr;
		UDEC(Oprand * opr):Ins(cUDEC),opr(opr){}
	};

	struct VRET : public Ins
	{
		Oprand* opr;
		VRET(Oprand * opr):Ins(cVRET),opr(opr){}
	};

	struct NEW : public Ins
	{
		HType type;
		Oprand * rank;
		NEW(HType type, Oprand * rank):Ins(cNEW),type(type),rank(rank){}
	};
	struct DEL : public Ins
	{
		HType type;
		Oprand * opr;
		DEL(HType type, Oprand * opr):Ins(cDEL),type(type),opr(opr){}
	};


	struct JMP : public Ins
	{
		Block * dest;
		JMP(Block * dest):Ins(cJMP),dest(dest){}
	};


	struct JCC : public Ins
	{
		Oprand* cond;
		Block* fblock;
		Block* tblock;
		JCC(Oprand* cond,Block* tblock,Block* fblock): Ins(cJCC),cond(cond),tblock(tblock),fblock(fblock){}
	};

	struct LDA : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		LDA(Oprand * lhs,Oprand * rhs):Ins(cLDA),lhs(lhs),rhs(rhs){}
	};


	struct LD : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		LD(Oprand * lhs,Oprand * rhs):Ins(cLD),lhs(lhs),rhs(rhs){}
	};

	struct ST : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		ST(Oprand * lhs,Oprand * rhs):Ins(cST),lhs(lhs),rhs(rhs){}
	};

	struct ADD		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		ADD(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cADD),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct SUB		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		SUB(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cSUB),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct MUL		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		MUL(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cMUL),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct DIV		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		DIV(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cDIV),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct SHR		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		SHR(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cSHR),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct SHL		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		SHL(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cSHL),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct XOR		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		XOR(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cXOR),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct SADD		: public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		SADD(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cSADD),dest(dest), lhs(lhs),rhs(rhs){}
	};

	struct NOP		 : public Ins
	{
		NOP():Ins(cNOP){}
	};
	struct BR		 : public Ins
	{
		BR():Ins(cBR){}
	};

	struct RET		 : public Ins
	{
		Oprand* lhs;
		RET(Oprand* lhs):Ins(cRET),lhs(lhs){}
	};
	struct LET		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		OpBinary::Op op;
		LET(Oprand * dest, OpBinary::Op op, Oprand * lhs,Oprand * rhs):Ins(cLET),dest(dest), lhs(lhs),rhs(rhs),op(op){}
		LET(Oprand * dest, Oprand * lhs):Ins(cLET),dest(dest), lhs(lhs),rhs(0),op(OpBinary::nil){}
	};
	struct CALL		: public Ins
	{
		Oprand * base;
		Oprand * opr;
		Block  * args;
		int	   argCount;
		xlang::Handle hTerm;
		CALL(Oprand * base,Oprand * opr,xlang::Handle hTerm,int argCount,Block  * args):Ins(cCALL),opr(opr),base(base),hTerm(hTerm),argCount(argCount),args(args){}
	};
	struct VCALL	: public Ins
	{
		Oprand * opr;
		VCALL(Oprand * opr):Ins(cVCALL),opr(opr){}
	};
	struct NCALL	 : public Ins
	{
		Oprand * opr;
		NCALL(Oprand * opr):Ins(cNCALL),opr(opr){}
	};
	struct PUSH	 : public Ins
	{
		HParam hParam;
		Oprand * opr;
		PUSH(Oprand * opr,HParam hParam):Ins(cPUSH),opr(opr),hParam(hParam){}
	};
	struct POP		 : public Ins
	{
		Oprand * opr;
		POP(Oprand * opr):Ins(cPOP),opr(opr){}
	};

	struct BIND	 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		BIND(Oprand * lhs,Oprand * rhs):Ins(cBIND), lhs(lhs),rhs(rhs){}
	};
	struct UNBIND	 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		UNBIND(Oprand * lhs,Oprand * rhs):Ins(cUNBIND), lhs(lhs),rhs(rhs){}
	};
	struct SPILL	 : public Ins
	{
		Oprand * opr;
		SPILL(Oprand * opr):Ins(cSPILL),opr(opr){}
	};

	struct NEQ		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		NEQ(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cNEQ),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct EQ		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		EQ(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cEQ),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct LT		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		LT(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cLT),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct LTE		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		LTE(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cLTE),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct GT		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		GT(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cGT),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct GTE		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		GTE(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cGTE),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct NOT		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		NOT( Oprand * lhs,Oprand * rhs):Ins(cNOT),lhs(lhs),rhs(rhs){}
	};


	struct FIELD	 : public Ins
	{
		Oprand * base;
		HField hField;
		//bool bStruct;
		FIELD(Oprand * base,HField hField):Ins(cFIELD), base(base),hField(hField){}
	};
	struct ELEMENT  : public Ins
	{
		Oprand * base;
		Oprand * rank;
		int eleSize;
		ELEMENT(Oprand * base,Oprand * rank,int eleSize):Ins(cELEMENT), base(base),rank(rank),eleSize(eleSize){}
	};

	struct BNOT	 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		BNOT(Oprand * lhs,Oprand * rhs):Ins(cBNOT), lhs(lhs),rhs(rhs){}
	};
	struct BOR		 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		BOR(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cBOR),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct BAND	 : public Ins
	{
		Oprand * lhs;
		Oprand * rhs;
		Oprand * dest;
		BAND(Oprand * dest, Oprand * lhs,Oprand * rhs):Ins(cBAND),dest(dest), lhs(lhs),rhs(rhs){}
	};
	struct CAST		 : public Ins
	{
		Oprand * opr;
		HType srcType;
		HType destType;
		CAST(HType srcType,HType destType, Oprand * opr):Ins(cCAST),srcType(srcType),destType(destType),opr(opr){}
	};

	struct NEG : public Ins
	{
		Oprand * opr;
		NEG(Oprand* opr) : Ins(cNEG),opr(opr){}
	};

	inline JMP * Jmp(Block* dest){ return new JMP(dest);}
	inline JCC * Jcc(Oprand* cond,Block* tblock,Block* fblock){ return new JCC(cond,tblock,fblock);}
	inline LDA * LoadA(Oprand * lhs,Oprand * rhs) { return new LDA(lhs,rhs);}
	inline LD  * Load(Oprand * lhs,Oprand * rhs) { return new LD(lhs,rhs);}
	inline ST  * Store(Oprand * lhs,Oprand * rhs) { return new ST(lhs,rhs);}
	inline ADD * Add(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new ADD(dest,lhs,rhs);}
	inline SUB * Sub(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new SUB(dest,lhs,rhs);}
	inline MUL * Mul(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new MUL(dest,lhs,rhs);}
	inline DIV * Div(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new DIV(dest,lhs,rhs);}
	inline SHR * Shr(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new SHR(dest,lhs,rhs);}
	inline SHL * Shl(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new SHL(dest,lhs,rhs);}
	inline XOR * Xor(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new XOR(dest,lhs,rhs);}
	inline SADD * SAdd(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new SADD(dest,lhs,rhs);}
	inline NOP  * Nop() { return new NOP();}
	inline BR	 * Br()  { return new BR();}
	inline RET  * Ret(Oprand * lhs)  { return new RET(lhs);}
	inline LET  * Let(Oprand * dest,OpBinary::Op op, Oprand * lhs,Oprand * rhs) { return new LET(dest,op,lhs,rhs);}
	inline LET  * Let(Oprand * dest,Oprand * lhs) { return new LET(dest,lhs);}
	//function pointer
	inline CALL * Call(Oprand * opr,int argCount,Block* args) { return new CALL(0,opr,0,argCount,args);}
	//member method
	inline CALL * Call(Oprand * base, Oprand * opr,int argCount,Block* args) { return new CALL(base,opr,0,argCount,args);}
	//static call
	inline CALL * Call(Oprand * base, Oprand * opr,xlang::Handle hTerm,int argCount,Block* args) { return new CALL(base,opr,hTerm,argCount,args);}
	
	inline VCALL* VCall(Oprand * opr) { return new VCALL(opr);}
	inline NCALL* NCall(Oprand * opr) { return new NCALL(opr);}
	inline PUSH * Push(Oprand * opr,HParam hParam) { return new PUSH(opr,hParam);}
	inline POP  * Pop(Oprand * opr) { return new POP(opr);}

	inline BIND  * Bind(Oprand * lhs,Oprand * rhs) { return new BIND(lhs,rhs);}
	inline UNBIND  * UnBind(Oprand * lhs,Oprand * rhs) { return new UNBIND(lhs,rhs);}
	inline SPILL  * Spill(Oprand * opr) { return new SPILL(opr);}

	inline EQ  * Eq(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new EQ(dest,lhs,rhs);}
	inline NEQ * Neq(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new NEQ(dest,lhs,rhs);}
	inline GT  * Gt(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new GT(dest,lhs,rhs);}
	inline GTE * Gte(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new GTE(dest,lhs,rhs);}
	inline LT  * Lt(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new LT(dest,lhs,rhs);}
	inline LTE * Lte(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new LTE(dest,lhs,rhs);}
	
	inline BOR  * Bor(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new BOR(dest,lhs,rhs);}
	inline BAND * Band(Oprand * dest, Oprand * lhs,Oprand * rhs) { return new BAND(dest,lhs,rhs);}
	inline BNOT * Bnot(Oprand * lhs,Oprand * rhs) { return new BNOT(lhs,rhs);}
	inline CAST * Cast(HType  destType,HType  srcType,Oprand * opr) { return new CAST(srcType,destType,opr);}
	
	inline ELEMENT * Element(Oprand * base,Oprand * rank,int eleSize) { return new ELEMENT(base,rank,eleSize);}
	inline FIELD * Field(Oprand * base,HField hField) { return new FIELD(base,hField);}
	inline NOT * Not(Oprand * lhs,Oprand * rhs) { return new NOT(lhs,rhs);}
	
	inline INC * Inc(Oprand * opr) { return new INC(opr);}
	inline DEC * Dec(Oprand * opr) { return new DEC(opr);}
	inline NEW * New(HType type,Oprand * rank) { return new NEW(type,rank);}
	inline DEL * Del(HType type,Oprand * opr) { return new DEL(type,opr);}
	inline NEG * Neg(Oprand * opr) { return new NEG(opr);}

	inline UINC * UInc(Oprand * opr) { return new UINC(opr);}
	inline UDEC * UDec(Oprand * opr) { return new UDEC(opr);}

	inline Imm* tImm(OpValue * value){
		Imm* imm = new Imm();
		imm->vt = (Imm::ValueType)value->vt;
		imm->val_d64 = value->val_d64;
		return imm;
	}
	inline Imm* tImm(int val){
		Imm* imm = new Imm();
		imm->vt = Imm::I32;
		imm->val_i32 = val;
		return imm;
	}
	inline Imm* tImm(double val){
		Imm* imm = new Imm();
		imm->vt = Imm::D64;
		imm->val_d64 = val;
		return imm;
	}
	inline Imm* tImm(wchar_t* val){
		Imm* imm = new Imm();
		imm->vt = Imm::STR;
		imm->val_wstr = val;
		return imm;
	}
	inline Imm* tImm(bool val){
		Imm* imm = new Imm();
		imm->vt = Imm::BOOL;
		imm->val_d64 = val;
		return imm;
	}

	inline Item* tItem(xlang::Handle hItem){ return new Item(hItem);}
	inline Item* tItem(OpItem* item) 
	{
		return new Item(item->hTerm);
	};

	inline Param* tParam(HParam hParam){ return new Param(hParam);}
	inline Var* tVar(HVariant hVariant){return new Var(hVariant);}
	inline Tmp* tTmp(int tmp){return new Tmp(tmp);}
	inline This* tThis() {return new This();}
	inline Nill* tNill() {return new Nill();}
	inline Block* tBlock() {return new Block();}
	inline VRET* VRet(Oprand * opr) { return new VRET(opr);}
}

} // namespace complier
} // namespace trap


#endif //STPP_PCODE_H