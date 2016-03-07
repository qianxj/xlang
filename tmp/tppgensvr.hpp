#ifndef TPP_GENSVR_H
#define TPP_GENSVR_H

#include <vector>
#include <map>
#include "tppsymnode.hpp"
#include "tpppcode.hpp"
#include "vlcontext.hpp"
#include "vlcoder.hpp"

namespace trap {
namespace complier {

class GenSvr
{
public:
	struct VarIndex
	{
		enum Kind
		{
			kUndef  = 0x00,
			kTmpVar = 0x01,
			kVar	= 0x02,
			kField  = 0x03,
			kParam  = 0x04
		};
		unsigned int  kind	: 8;
		unsigned int  val	: 24;
		inline operator bool ()
		{
			return kind != kUndef;
		}
		inline bool operator ==(const VarIndex & rhs)
		{
			return kind == rhs.kind && val == rhs.val;
		}
		inline bool operator !=(const VarIndex & rhs)
		{
			return kind != rhs.kind || val != rhs.val;
		}
		bool operator <(const VarIndex & rhs) const
		{
			if(kind < rhs.kind) return true;
			if(kind == rhs.kind && val < rhs.val) return true;
			return false;
		}
		VarIndex(int kind,int val):kind(kind),val(val){}
		VarIndex():kind(kUndef),val(0){}
	};

	//for reg use
	struct RegUse
	{
		int count;
		VarIndex varIndex[7];

		RegUse();
	};

	struct RegItem
	{
		//machine register no
		int	realReg;
		//register used  by variant
		RegUse used;
		//register machine term 
		void*	regTerm;
		//caller register
		bool	caller;

		std::vector<int> uses;
		int currentUse;

		//constructor
		RegItem(int reg,bool caller = false);
	};

	struct VarItemBase
	{
		//offset by ebp or base address.
		int	offset;

		//-1: not use register
		//>=0: [0,...] index of regsvr vars
		int reg;

		//true: memory data is valid.
		//false:memory data is not valid.
		bool addr;

		//variant address term
		void * addrTerm;

	};

	//temp variant
	struct VarTmpItem : public VarItemBase
	{
		//type handle of temp variant 
		HType ty;
	};

	//variant , param, field,array but not temp variant
	struct VarItem : public VarItemBase
	{
		xlang::Handle hTerm;
	};

	//field,array but not temp variant
	struct FieldItem : public VarItem
	{
		VarIndex baseIndex;
	};

	class RegSvr
	{
	public:
		RegSvr(GenSvr & genSvr):tmpOffset(0x8),varOffset(0x4),paramOffset(-0x8),genSvr(genSvr){genSvr.regSvr =  this;}
	public:
		//variants
		std::vector<VarItem>		vars;
		std::vector<VarItem>		params;
		std::vector<FieldItem>		fields;
		std::map<VarIndex,std::map<HField,VarIndex>> mapFields;
		std::vector<VarTmpItem>		tmps; //temp variant
		//registers
		std::vector<RegItem>		regs;

	public:
		/*
		  get a reister for use
		  wreg: [0,...] index regs and flag use bit 
		*/
		int GetReg(int mreg = 0); 

		void SpillCallerReg(int mreg = 0);

		/* 
			spill register when no other register for use or store register before call .
		*/
		void SpillReg(int reg);

		/*
			free  register which use with var.  
		*/
		void UnUsedReg(VarIndex varIndex);

		void GenSvr::RegSvr::SpillRegs(int wreg = 0);

		/*
			var use register
		*/
		void UseReg(int reg, VarIndex varIndex);

		void UseRegReplace(int reg, VarIndex srcIndex,VarIndex destIndex);
		void SwapReg(int r1,int r2);

		/**/
		bool isShared(int reg);
		bool isUsed(int reg);
		bool isTmpVar(VarIndex & varIndex);
		bool isValid(int reg);
	public:
		VarIndex  SureFieldIndex(vlContext & context,VarIndex & baseIndex, OpItem * item);
		VarIndex  GetVarIndex(vlContext & context,OpItem * item);
		VarItemBase & GetVarItem(vlContext & context,OpItem * item);
		VarItemBase & GetVarItem(VarIndex &varIndex);

		VarIndex  SureFieldIndex(vlContext & context,VarIndex & baseIndex, HField term);
		VarIndex  GetVarIndex(vlContext & context,PCode::Item * item);
		VarItemBase & GetVarItem(vlContext & context,PCode::Item * item);
		int LookupReg(void* term);

	public:
		bool LoadData(int reg, VarIndex &index);
		bool StoreData(int reg, VarIndex &index);

	public:
		//variant frame offset base
		int   tmpOffset;
		int   varOffset;
		int   paramOffset;
		
		GenSvr & genSvr;
	};
public:
	GenSvr(vlContext& context);
public:
	vlContext& context;
	RegSvr*	regSvr;
public:
	inline bool isShared(int reg){return regSvr->isShared(reg);}
	inline bool isUsed(int reg){return regSvr->isUsed(reg);}
	inline bool isTmpVar(VarIndex & varIndex){return regSvr->isTmpVar(varIndex);}
	inline bool isValid(int reg){return regSvr->isValid(reg);}
public:
	vlCoder * coder;
};

} // namespace complier
} // namespace trap


#endif //TPP_GENSVR_H