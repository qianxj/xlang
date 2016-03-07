#ifndef STPP_SNODE_H
#define STPP_SNODE_H

#include <vector>
#include "vltoken.hpp"
#include "vlutils.hpp"
#include "vlsym.hpp"

using namespace xlang;

namespace trap {
namespace complier {

	enum KindSNode : unsigned char
	{
		kIf = 1,
		kFor = 2,
		kWhile = 3,
		kUntil = 4,
		kReturn = 5,
		kContinue = 6,
		kBreak = 7,
		kCompStmt = 8,
		kExprStmt = 9,
		kItem =10,
		kThis=11,
		kSuper = 12,
		kValue = 13,
		kNill = 14,
		kField = 15,
		kElement = 16,
		kMethod = 17,
		kCall = 18,
		kArgList = 19,
		kType = 20,
		kInc  = 21,
		kDec = 22,
		kUInc = 23,
		kUDec = 24,
		kNew = 25,
		kDelete = 26,
		kNot = 27,
		kBNot = 28,
		kNeg = 29,
		kCast = 30,
		kBinary = 31,
		kList = 32,
		kParen = 33,
		kEmpty = 34,
		kSizeof= 35,

		kVar = 40,
		kParam = 41,

		kToken = 42,

		kBlock = 43,
		kJmp   = 44,
		kJcc   = 45
	};

	struct SNode
	{
		KindSNode kind;
		HType hType;
		int srcRow;
		int srcCol;
		bool isLeftVaule;
		bool isArg;
		bool isRet;
		SNode(KindSNode k):kind(k),hType(0),srcRow(0),srcCol(0),isLeftVaule(false),isArg(false),isRet(false){};
		SNode(KindSNode k,int srcRow,int srcCol):kind(k),hType(0),srcRow(srcRow),srcCol(srcCol),isLeftVaule(false),isArg(false),isRet(false){};
	};

	struct OpList : public SNode
	{
		OpList* next;
		SNode * node;
		OpList():SNode(kList),next(0),node(0){}
		OpList(SNode * node):SNode(kList),next(0),node(node){}
		OpList(OpList* next, SNode * node):SNode(kList),next(next),node(node){}
		inline OpList * last()
		{
			OpList* term = this;
			while(term->next) term = term->next;
			return term;
		}
	};

	struct OpToken : public SNode
	{
		vlToken tk;
		OpToken(vlToken &tk):SNode(kToken,tk.row,tk.col),tk(tk){}
	};

	struct OpIf : public SNode
	{
		SNode* cond;
		SNode* tbody;
		SNode* fbody;
		OpIf(SNode* cond, SNode* tbody, SNode* fbody):SNode(kIf),
			cond(cond),tbody(tbody),fbody(fbody){};
	};

	struct OpBreak : public SNode
	{
		OpBreak():SNode(kBreak){};
	};

	struct OpContinue : public SNode
	{
		OpContinue():SNode(kContinue){};
	};

	struct OpExprStmt : public SNode
	{
		SNode* expr;
		OpExprStmt(SNode* expr):SNode(kExprStmt),expr(expr){}
	};

	struct OpCompStmt : public SNode
	{
		//std::vector<SNode *> stmts;
		SNode* stmts;
		OpCompStmt(SNode* stmts):SNode(kCompStmt),stmts(stmts){}
	};

	struct OpFor : public SNode
	{
		SNode * init;
		SNode * cond;
		SNode * inc;
		SNode * body;

		OpFor(SNode* init, SNode* cond, SNode* inc, SNode* body):SNode(kFor),
		init(init),cond(cond),inc(inc),body(body){}
	};

	struct OpWhile : public SNode
	{
		SNode * cond;
		SNode * body;
		OpWhile(SNode * cond, SNode * body):SNode(kWhile),
		cond(cond),body(body){}
	};

	struct OpReturn : public SNode
	{
		SNode * expr;
		OpReturn():SNode(kReturn){};
		OpReturn(SNode* expr):SNode(kReturn,expr?expr->srcRow:0,expr?expr->srcCol:0),expr(expr){}
	};


	struct OpUntil : public SNode
	{
		SNode * cond;
		SNode * body;
		OpUntil(SNode * cond, SNode * body):SNode(kUntil),
		cond(cond),body(body){}
	};

	struct OpValue : public SNode
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
		OpValue(int val):SNode(kValue),vt(I32),val_i32(val){}
		OpValue(unsigned int val):SNode(kValue),vt(UI32),val_ui32(val){}
		OpValue(short val):SNode(kValue),vt(I16),val_i16(val){}
		OpValue(unsigned short val):SNode(kValue),vt(UI16),val_ui16(val){}
		OpValue(char val):SNode(kValue),vt(I8),val_i8(val){}
		OpValue(unsigned char val):SNode(kValue),vt(UI8),val_ui8(val){}
		OpValue(__int64 val):SNode(kValue),vt(I64),val_i64(val){}
		OpValue(unsigned __int64 val):SNode(kValue),vt(UI64),val_ui64(val){}
		OpValue(double val):SNode(kValue),vt(D64),val_d64(val){}
		OpValue(float val):SNode(kValue),vt(D32),val_d32(val){}

		OpValue(char* val):SNode(kValue),vt(ASTR),val_str(val){}
		OpValue(wchar_t* val):SNode(kValue),vt(STR),val_wstr(val){}

		OpValue(void* val):SNode(kValue),vt(PTR),val_ptr(val){}
		OpValue(bool val):SNode(kValue),vt(BOOL),val_bool(val){}
	};



	struct OpItem : public SNode
	{
		HVariant hTerm;
		int tempId; //¡Ÿ ±±‰¡øID
		unsigned int spec;
		OpItem* nest;
		int ident;
		SNode* param;
		SNode* rank;

		OpItem(int tempId,unsigned int spec=0):SNode(kItem),rank(0),spec(spec),tempId(tempId){}
		OpItem(HVariant hTerm):SNode(kItem),tempId(0),rank(0),hTerm(hTerm),spec(0){}
		OpItem(OpItem* nest,int ident,SNode* param):SNode(kItem),ident(ident),param(param),rank(0),tempId(0),nest(nest),spec(0){}
	};

	struct	OpThis : public SNode
	{
		OpThis(): SNode(kThis){}
	};

	struct	OpSuper : public SNode
	{
		OpSuper(): SNode(kSuper){}
	};

	struct	OpNill : public SNode
	{
		OpNill(): SNode(kNill){}
	};

	struct OpMethodPtr : public SNode
	{
		SNode* typ;
		SNode* args;
		SNode* base;
		OpMethodPtr(SNode* typ, SNode* base) : SNode(kMethod),
			typ(typ),base(base){}
	};

	struct OpCall  : public SNode
	{
		SNode * typ;
		SNode * mt;
		SNode * args;
		OpCall(SNode* typ, SNode* mt, SNode* args):SNode(kCall),
			typ(typ),mt(mt),args(args){}
	};

	struct OpArgList : public SNode
	{
		std::vector<SNode*> args;
		OpArgList():SNode(kArgList){}
	};

	struct OpField : public SNode
	{
		SNode * typ;
		SNode * base;
		SNode * hitem;
		HField hField;
		OpField(SNode* typ, SNode* base, SNode* hitem) :SNode(kField,
				hitem?hitem->srcRow:0,hitem?hitem->srcCol:0),
			typ(typ),base(base),hitem(hitem),hField(0){}
	};

	struct OpElement : public SNode
	{
		SNode * typ;
		SNode * rank;
		SNode * exp;
		OpElement(SNode* typ, SNode* exp,SNode* rank ) : SNode(kElement),
			typ(typ),rank(rank),exp(exp){}
	};

	struct OpInc : public SNode
	{
		SNode * typ;
		SNode * expr;
		OpInc(SNode* typ, SNode* expr) : SNode(kInc),
			typ(typ),expr(expr){}
	};
	
	struct OpDec : public SNode
	{
		SNode * typ;
		SNode * expr;
		OpDec(SNode* typ, SNode* expr) : SNode(kDec),
			typ(typ),expr(expr){}
	};

	struct OpUInc : public SNode
	{
		SNode * typ;
		SNode * expr;
		OpUInc(SNode* typ, SNode* expr) : SNode(kUInc),
			typ(typ),expr(expr){}
	};

	struct OpUDec : public SNode
	{
		SNode * typ;
		SNode * expr;
		OpUDec(SNode* typ, SNode* expr) : SNode(kUDec),
			typ(typ),expr(expr){}
	};

	struct OpNeg : public SNode
	{
		SNode * typ;
		SNode * expr;
		OpNeg(SNode* typ, SNode* expr) : SNode(kNeg),
			typ(typ),expr(expr){}
	};

	struct OpNew : public SNode
	{
		SNode * typ;
		SNode * rank;
		OpNew(SNode* typ, SNode* rank) : SNode(kNew),
			typ(typ),rank(rank){}
	};

	struct OpDelete : public SNode
	{
		SNode * exp;
		OpDelete(SNode* exp) :SNode(kDelete),exp(exp){}
	};

	struct OpSizeof : public SNode
	{
		SNode * expr;
		OpSizeof(SNode* exp) :SNode(kSizeof),expr(exp){}
	};

	struct OpNot : public SNode
	{
		SNode * typ;
		SNode * expr;
		OpNot(SNode* typ, SNode* expr) : SNode(kNot),
			typ(typ),expr(expr){}
	};

	struct OpBNot : public SNode
	{
		SNode * typ;
		SNode * expr;
		OpBNot(SNode* typ, SNode* expr) : SNode(kBNot),
			typ(typ),expr(expr){}
	};
	
	struct OpCast : public SNode
	{
		SNode * typ;
		SNode * expr;
		OpCast(SNode* typ, SNode* expr) :SNode(kCast),
			typ(typ),expr(expr){}
	};

	struct OpBinary : public SNode
	{
		enum Op
		{
			nil		= 0x0,
			add		= 0x1,
			sub		= 0x2,
			mul		= 0x3,
			div		= 0x4,
			shl		= 0x5,
			shr		= 0x6,
			asn		= 0x7,
			mod		= 0x8,
			or		= 0x12,
			and		= 0x13,
			band	= 0x15,
			bor		= 0x16,
			xor		= 0x18,
			eq		= 0x20,
			neq		= 0x21,
			gt		= 0x22,
			gte		= 0x23,
			lt		= 0x24,
			lte		= 0x25,

			ref		 = 0x2e,
			pointer	 = 0x3A,
			mref	 = 0x3E,
			mpointer = 0x3F,
			varrow	 = 0x40,
			arrow	 = 0x42
		};

		Op op;
		SNode* typ;
		SNode* lhs;
		SNode* rhs;
		std::vector<Op> ops;
		std::vector<SNode*> terms;
		bool expand; 

		inline static bool canSwap(Op op) {return op==add||op==mul||op==and||op==or||op==xor;}
		OpBinary(SNode* typ,Op op,  SNode* lhs, SNode* rhs) :SNode(kBinary),
			op(op),typ(typ),lhs(lhs),rhs(rhs),expand(true){}
	};

	struct OpType : public SNode
	{
		unsigned int htype;
		OpList * spec;
		int		ident;
		OpList * param;
		OpList * rank;
		OpType * axis;
		OpType(int htype) :SNode(kType),htype(htype){}
		OpType(OpList * spec,int ident,OpList * param, OpList* rank,OpType* axis)
			:SNode(kType),htype(0),spec(spec),ident(ident),param(param),rank(rank),axis(axis){}
	};

	struct OpVar : public SNode
	{
		int ident;
		SNode * typ;
		SNode * expr;
		HVariant hVariant;
		OpVar(int ident, SNode* typ, SNode * expr) : SNode(kVar),
			ident(ident),typ(typ),expr(expr),hVariant(0){}
	};

	struct OpParam : public SNode
	{
		int ident;
		SNode * typ;
		SNode * expr;
		SNode* terms;
		HParam hParam;
		OpParam (int ident, SNode* typ, SNode * expr,SNode* terms) : SNode(kVar),
			ident(ident),typ(typ),expr(expr),hParam(0),terms(terms){}
	};

	struct OpBlock : public SNode
	{
		OpList * stmts;
		std::vector<int> jmpSrc;
		int startpc;
		OpBlock & operator +=(SNode* node);
		OpBlock():SNode(kBlock),stmts(new OpList()),startpc(0){}
	};

	struct OpJmp : public SNode
	{
		OpBlock * dest;
		OpJmp():SNode(kJmp),dest(0){}
		OpJmp(OpBlock * dest):SNode(kJmp),dest(dest){}
	};

	struct OpJcc : public SNode
	{
		enum Cond
		{
			cOverflow     	=  0, 
			cNoOverflow  	=  1, 
			cBelow        	=  2, 
			cAboveEqual		=  3, 
			cEqual        	=  4, 
			cNotEqual     	=  5, 
			cBelowEqual 	=  6,
			cAbove        	=  7,
			cNegative     	=  8,
			cPositive     	=  9,
			cParityEven   	= 10,
			cParityOdd    	= 11,
			cLess         	= 12,
			cGreaterEqual	= 13,
			cLessEqual   	= 14,
			cGreater      	= 15,

			cCarry        	= cBelow,
			cNotCarry    	= cAboveEqual,
			cZero         	= cEqual,
			cNotZero     	= cNotEqual,
			cSign         	= cNegative,
			cNotSign     	= cPositive
		};

		SNode* expr;
		OpBlock * tblock;
		OpBlock * fblock;
		OpJcc(SNode* expr,OpBlock* tblock,OpBlock* fblock):SNode(kJcc),expr(expr),tblock(tblock),fblock(fblock){}
	};

	inline SNode* setNodePosition(SNode* node, SNode* pos)
	{
		if(node && pos)
		{
			node->srcRow = pos->srcRow;
			node->srcCol = pos->srcCol;
		}
		return node;
	}

	inline OpIf* opIf(SNode* cond, SNode* tbody, SNode* fbody)
	{
		return new OpIf(cond,tbody,fbody); 
	}

	inline OpFor* opFor(SNode* init, SNode* cond, SNode* inc, SNode* body)
	{
		return new OpFor(init,cond,inc,body);
	}

	inline OpWhile* opWhile(SNode* cond, SNode* body)
	{
		return new OpWhile(cond,body);
	};

	inline OpUntil* opUntil(SNode* cond, SNode* body)
	{
		return new OpUntil(cond,body);
	};

	inline OpReturn* opReturn(SNode* exp)
	{
		return new OpReturn(exp);
	};

	inline OpContinue* opContinue()
	{
		return new OpContinue();
	};

	inline OpBreak* opBreak()
	{
		return new OpBreak();
	};

	inline OpCompStmt* opCompStmt(SNode * stmts)
	{
		return new OpCompStmt(stmts);
	};

	inline OpExprStmt* opExprStmt(SNode* exp)
	{
		return new OpExprStmt(exp);
	}
	
	inline OpValue* opValue(int val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(unsigned int val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(short val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(unsigned short val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(char val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(unsigned char val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(__int64 val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(unsigned __int64 val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(double val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(float val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(char* val)
	{
		return new OpValue(val);
	}
	inline OpValue* opValue(wchar_t* val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(void* val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(bool val)
	{
		return new OpValue(val);
	}

	inline OpValue* opValue(SNode* typ, SNode* lt)
	{
		if(lt->kind!=kToken) return 0;
		vlToken & tk = ((OpToken*)lt)->tk;
		if(tk.ty==tokenType::tInt)
			return opValue(tk.inum);
		else if(tk.ty==tokenType::tDouble)
			return opValue(tk.dnum);
		else if(tk.ty==tokenType::tBool)
			return opValue(tk.inum?true:false);
		else if(tk.ty==tokenType::tChar)
			return opValue((wchar_t)tk.inum);
		else if(tk.ty==tokenType::tString)
		{
			return opValue(util::getHashString(util::hashString((const wstring_t)tk.snum,tk.slen)));
		}
		return 0;
	};

	inline OpItem* opItem(OpItem* nest,int ident,SNode* param)
	{
		return new OpItem(nest,ident,param);
	};

	inline OpItem* opItem(HVariant hTerm)
	{
		return new OpItem(hTerm);
	};

	inline OpItem* opItem(int tempId)
	{
		return new OpItem(tempId);
	};

	inline OpThis* opThis()
	{
		return new OpThis();
	};

	inline OpSuper* opSuper()
	{
		return new OpSuper();
	};

	inline OpNill* opNill()
	{
		return new OpNill();
	};

	inline OpMethodPtr* opMethodPtr(SNode* typ, SNode* base)
	{
		return new OpMethodPtr(typ,base);
	};

	inline OpCall* opCall(SNode* typ, SNode* mt, SNode* args)
	{
		return new OpCall(typ,mt,args);
	};

	inline OpArgList* opArgList()
	{
		return new OpArgList();
	};

	inline OpField* opField(SNode* typ, SNode* base, SNode* hitem)
	{
		return new OpField(typ,base,hitem);
	};

	inline OpElement* opElement(SNode* typ,  SNode* exp,SNode* rank)
	{
		return new OpElement(typ,exp,rank);
	};

	inline OpInc* opInc(SNode* typ, SNode* exp)
	{
		return new OpInc(typ,exp);
	};
	inline OpDec* opDec(SNode* typ, SNode* exp)
	{
		return new OpDec(typ,exp);
	};

	inline OpUInc* opUInc(SNode* typ, SNode* exp)
	{
		return new OpUInc(typ,exp);
	};

	inline OpUDec* opUDec(SNode* typ, SNode* exp)
	{
		return new OpUDec(typ,exp);
	};

	inline OpNeg* opNeg(SNode* typ, SNode* exp)
	{
		return new OpNeg(typ,exp);
	};

	inline OpNew* opNew(SNode* typ, SNode* rank)
	{
		return new OpNew(typ,rank);
	};

	inline OpDelete* opDelete(SNode* exp)
	{
		return new OpDelete(exp);
	};

	inline OpSizeof* opSizeof(SNode* exp)
	{
		return new OpSizeof(exp);
	};

	inline OpNot* opNot(SNode* typ, SNode* exp)
	{
		return new OpNot(typ,exp);
	};

	inline OpBNot* opBNot(SNode* typ, SNode* exp)
	{
		return new OpBNot(typ,exp);
	};
	
	inline OpCast* opCast(SNode* typ, SNode* exp)
	{
		return new OpCast(typ,exp);
	};

	inline OpVar* opVar(int ident,SNode * typ, SNode*  expr)
	{
		return new OpVar(ident,typ,expr);
	}

	inline OpParam* opParam(int ident,SNode * typ, SNode*  expr, SNode* terms)
	{
		return new OpParam(ident,typ,expr,terms);
	}
	inline OpParam* opParam(int ident,SNode * typ, SNode*  expr)
	{
		return new OpParam(ident,typ,expr,0);
	}

	inline OpBinary* opBinary(SNode* typ,OpBinary::Op op,  SNode* lhs, SNode* rhs,bool normalize=false)
	{
		if(normalize)
			return new OpBinary(typ,op,lhs,rhs);
		if(lhs->kind==kBinary && ((OpBinary*)lhs)->expand)
		{
			((OpBinary*)lhs)->ops.push_back(op);
			((OpBinary*)lhs)->terms.push_back(rhs);
			return (OpBinary*)lhs;
		}else
			return new OpBinary(typ,op,lhs,rhs);
	};

	inline OpToken* opToken(vlToken &tk)
	{
		return new OpToken(tk);
	}

	inline OpType* opType(int htype)
	{
		return new OpType(htype);
	}

	inline OpType*  opType(OpList * spec,int ident,OpList * param = 0, OpList* rank = 0,OpType* axis = 0)
	{
		return new OpType(spec,ident,param,rank,axis);
	}

	inline OpList*  opList()
	{
		return new OpList();
	}
	
	inline OpList*  opList(SNode* term)
	{
		return new OpList(term);
	}

	inline OpList*  opList(OpList* head , SNode* term)
	{
		if(!term) term = new SNode(kEmpty);
		if(!head) return opList(term);
		if(!head->node && !head->next)
		{
			head->node = term;
			return head;
		}
		OpList * t = head;
		while(t->next) t = t->next;
		t->next = new OpList(term);
		return head;
	}

	inline OpBlock * opBlock()
	{
		return new OpBlock();
	}

	inline OpBlock & OpBlock::operator +=(SNode* node)
	{
		if(!stmts)
			stmts = opList(node);
		else
			stmts = opList(stmts,node);
		return *this;
	}


	inline OpJmp * opJmp(OpBlock * dest)
	{
		return new OpJmp(dest);
	}

	inline OpJcc * opJcc(SNode* expr,OpBlock* tbody,OpBlock* fbody)
	{
		return new OpJcc(expr,tbody,fbody);
	}

} // namespace complier
} // namespace trap


#endif //STPP_SNODE_H