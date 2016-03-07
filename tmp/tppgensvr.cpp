#include "tppgensvr.hpp"
#include "machine_x86.hpp"

using namespace trap::complier;
using namespace XM;


GenSvr::RegUse::RegUse():count(0)
{
}

GenSvr::GenSvr(vlContext& context):context(context),coder(context.trap->getCoder())
{
}

GenSvr::RegItem::RegItem(int reg,bool caller):realReg(reg),
	regTerm((void*)X86::GetPoolRegister(reg)),caller(caller)
{
}

int GenSvr::RegSvr::GetReg(int wreg)
{
	int start = 1;//skip ecx
	//have a free register then return it.
	for(int i=start;i<(int)regs.size();i++)
	{
		if(wreg & (1<<i)) continue;
		if(!regs[i].used.count) return i;
	}

	//评估寄存器的价值
	int rv = 1000;
	int reg = -1;
	for(int i=start;i<(int)regs.size();i++)
	{
		int crv = 0;
		if(wreg & (1<<i)) continue;
		if(regs[i].used.count > 1) crv += 80;//多个变量在使用
		if(regs[i].used.varIndex[0].kind==VarIndex::kTmpVar) crv += 100;//临时变量
		if(crv < rv)
		{
			rv = crv;
			reg = i;
		}
	}
	if(reg <0) return reg; //寄存器不够用

	SpillReg(reg);

	return reg;
}

void GenSvr::RegSvr::SpillCallerReg(int wreg)
{
	for(int i=0;i<(int)regs.size();i++)
	{
		if(wreg & (1<<i)) continue;
		if(regs[i].caller && regs[i].used.count) SpillReg(i);
	}
}

void GenSvr::RegSvr::SpillRegs(int wreg)
{
	for(int i=0;i<(int)regs.size();i++)
	{
		if(wreg & (1<<i)) continue;
		if(regs[i].used.count)
		{
			RegUse & use =  regs[i].used;
			for(int  k=0;k< use.count;k++)
			{
				if(use.varIndex->kind != VarIndex::kTmpVar)
				{
					VarItemBase * item = &GetVarItem(use.varIndex[k]);
					if(item->addr==false)
					{
						StoreData(i,use.varIndex[k]);
						item->addr = true;
					}
					item->reg = -1;
				}
			}
		}
		regs[i].used.count = 0;
	}
}

GenSvr::VarIndex  GenSvr::RegSvr::SureFieldIndex(vlContext & context,VarIndex & baseIndex, HField hTerm)
{
	VarIndex varIndex = baseIndex;
	VarItemBase * term = &GetVarItem(varIndex);
	HType hType = context.trap->getParent(hTerm);
	/*if(varIndex.kind==VarIndex::kTmpVar)
		hType = ((VarTmpItem*)term)->ty;
	else
		hType =context.trap->getTermType(((VarItem*)term)->hTerm);
	*/
	
	if(mapFields.find(varIndex)==mapFields.end())
	{
		mapFields.insert(std::pair<VarIndex,std::map<HField,VarIndex>>(varIndex,std::map<HField,VarIndex>()));
	}
	if(mapFields[varIndex].find(hTerm)==mapFields[varIndex].end())
	{
		fields.push_back(GenSvr::FieldItem());
		GenSvr::FieldItem & it = fields.back();
		it.hTerm = hTerm;
		it.reg = -1;
		it.addr = true;
		it.offset = hType?context.trap->getFieldOffset(hType,it.hTerm):-1;
		it.addrTerm = 0;
		it.baseIndex = baseIndex;
		mapFields[varIndex].insert(
			std::pair<HField,VarIndex>(
				hTerm,VarIndex(VarIndex::kField,(int)fields.size() - 1)));
	}
	return mapFields[varIndex][hTerm];
}

GenSvr::VarIndex  GenSvr::RegSvr::SureFieldIndex(vlContext & context,VarIndex & baseIndex, OpItem * item)
{
	VarIndex varIndex = baseIndex;
	VarItemBase * term = &GetVarItem(varIndex);
	HType hType;
	if(varIndex.kind==VarIndex::kTmpVar)
		hType = ((VarTmpItem*)term)->ty;
	else
		hType =context.trap->getTermType(((VarItem*)term)->hTerm);

	if(mapFields.find(varIndex)==mapFields.end())
	{
		mapFields.insert(std::pair<VarIndex,std::map<HField,VarIndex>>(varIndex,std::map<HField,VarIndex>()));
	}
	if(mapFields[varIndex].find((HField)item->hTerm)==mapFields[varIndex].end())
	{
		fields.push_back(GenSvr::FieldItem());
		GenSvr::FieldItem & it = fields.back();
		it.hTerm = item->hTerm;
		it.reg = -1;
		it.addr = true;
		it.offset = hType?context.trap->getFieldOffset(hType,it.hTerm):-1;
		it.addrTerm = 0;
		it.baseIndex = baseIndex;
		mapFields[varIndex].insert(
			std::pair<HField,VarIndex>(
				(HField)item->hTerm,VarIndex(VarIndex::kField,(int)fields.size() - 1)));
	}
	return mapFields[varIndex][(HField)item->hTerm];
}


GenSvr::VarIndex  GenSvr::RegSvr::GetVarIndex(vlContext & context,PCode::Item * item)
{
	if(item->tempId)
		return VarIndex(VarIndex::kTmpVar,item->tempId - 1);
	if(item->hTerm.kind==skVariant)
	{
		int index = item->hTerm.index - vars[1].hTerm.index;
		return VarIndex(VarIndex::kVar,index + 1/*this*/);
	}
	else if(item->hTerm.kind==skParam)
	{
		int index = item->hTerm.index - params[0].hTerm.index;
		return VarIndex(VarIndex::kParam,index);
	}
	else if(item->hTerm.kind==skField)
	{
		VarIndex  thisIndex(VarIndex::kVar,0);
		return SureFieldIndex(context,thisIndex,item->hTerm);
	}
	else
	{
		return VarIndex(VarIndex::kField,0);
	}

}

GenSvr::VarIndex  GenSvr::RegSvr::GetVarIndex(vlContext & context,OpItem * item)
{
	if(item->tempId)
		return VarIndex(VarIndex::kTmpVar,item->tempId - 1);
	if(item->hTerm.kind==skVariant)
	{
		int index = item->hTerm.index - vars[1].hTerm.index;
		return VarIndex(VarIndex::kVar,index + 1/*this*/);
	}
	else if(item->hTerm.kind==skParam)
	{
		int index = item->hTerm.index - params[0].hTerm.index;
		return VarIndex(VarIndex::kParam,index);
	}
	else if(item->hTerm.kind==skField)
	{
		VarIndex  thisIndex(VarIndex::kVar,0);
		return SureFieldIndex(context,thisIndex,item);
	}
	else
	{
		return VarIndex(VarIndex::kField,0);
	}

}

int GenSvr::RegSvr::LookupReg(void* term)
{
	for(int i=0;i<(int)regs.size();i++)
	{
		if(regs[i].regTerm == term) return i;
	}
	return -1;
}

GenSvr::VarItemBase & GenSvr::RegSvr::GetVarItem(vlContext & context,PCode::Item * item)
{
	if(item->tempId)
		return tmps[item->tempId - 1];
	if(item->hTerm.kind==skVariant)
	{
		int index = item->hTerm.index - vars[1].hTerm.index;
		return vars[index + 1];
	}
	else if(item->hTerm.kind==skParam)
	{
		int index = item->hTerm.index - params[0].hTerm.index;
		return params[index];
	}
	else if(item->hTerm.kind==skField)
	{
		VarIndex index = GetVarIndex(context,item);
		return fields[index.val];
	}
	else
	{
		return fields[0];
	}
}

GenSvr::VarItemBase & GenSvr::RegSvr::GetVarItem(vlContext & context,OpItem * item)
{
	if(item->tempId)
		return tmps[item->tempId - 1];
	if(item->hTerm.kind==skVariant)
	{
		int index = item->hTerm.index - vars[1].hTerm.index;
		return vars[index + 1];
	}
	else if(item->hTerm.kind==skParam)
	{
		int index = item->hTerm.index - params[0].hTerm.index;
		return params[index];
	}
	else if(item->hTerm.kind==skField)
	{
		VarIndex index = GetVarIndex(context,item);
		return fields[index.val];
	}
	else
	{
		return fields[0];
	}
}

GenSvr::VarItemBase & GenSvr::RegSvr::GetVarItem(VarIndex & varIndex)
{
	if(varIndex.kind==VarIndex::kTmpVar)
		return tmps[varIndex.val];
	else if(varIndex.kind==VarIndex::kVar)
		return vars[varIndex.val];
	else if(varIndex.kind==VarIndex::kParam)
		return params[varIndex.val];
	else if(varIndex.kind==VarIndex::kField)
		return fields[varIndex.val];

	assert(varIndex.kind==VarIndex::kField);
	return fields[varIndex.val];
}

bool GenSvr::RegSvr::LoadData(int reg,GenSvr::VarIndex &index)
{
	unsigned char* & pc = genSvr.coder->hCodeSpace->pc;
	if(index.kind==VarIndex::kVar || index.kind==VarIndex::kParam || index.kind==VarIndex::kTmpVar)
	{
		GenSvr::VarItemBase & item = GetVarItem(index);
		pc = X86::MOV(pc,*(Register<B32>*)regs[reg].regTerm,*(X86::AddressX86<B32>*)item.addrTerm);
	}else if(index.kind ==VarIndex::kField)
	{
		FieldItem * term = (FieldItem *)&GetVarItem(index);
		GenSvr::VarItemBase & item = GetVarItem(term->baseIndex);
		
		HType hClass = genSvr.context.trap->getParent(term->hTerm);
		HType hMyType = genSvr.context.trap->getField(term->hTerm)->hType;
		bool bstruct = genSvr.context.trap->getTypeKind(hClass)==TypeKind::tkStruct?true:false;
        bool tstruct = genSvr.context.trap->getTypeKind(hMyType)==TypeKind::tkStruct?true:false;
		if(genSvr.context.trap->getTermSpec(term->hTerm) & ParamSpec::byref_)tstruct = false;
		if(bstruct)
		{
			if(item.reg==-1)
			{
				pc = X86::LEA(pc,X86::edx,*(X86::AddressX86<B32>*)item.addrTerm);
				if(tstruct)
				{
					pc = X86::LEA(pc,*(Register<B32>*)regs[reg].regTerm,
								X86::AddressX86<B32>(X86::edx,Disp<B32>(term->offset)));
				}else
				{
					pc = X86::MOV(pc,*(Register<B32>*)regs[reg].regTerm,
								X86::AddressX86<B32>(X86::edx,Disp<B32>(term->offset)));
				}
			}else
			{
				//pc = X86::MOV(pc,*(Register<B32>*)regs[reg].regTerm,
				//	X86::AddressX86<B32>(X86::ecx,Disp<B32>(term->offset)));
				if(tstruct)
				{
					pc = X86::LEA(pc,*(Register<B32>*)regs[reg].regTerm,
						X86::AddressX86<B32>(*(Register<B32>*)regs[item.reg].regTerm,Disp<B32>(term->offset)));
				}else
				{
					pc = X86::MOV(pc,*(Register<B32>*)regs[reg].regTerm,
						X86::AddressX86<B32>(*(Register<B32>*)regs[item.reg].regTerm,Disp<B32>(term->offset)));
				}
			}
		}else
		{
			if(item.reg==-1)
			{
				pc = X86::MOV(pc,X86::edx,*(X86::AddressX86<B32>*)item.addrTerm);
				if(tstruct)
				{
					pc = X86::LEA(pc,*(Register<B32>*)regs[reg].regTerm,
								X86::AddressX86<B32>(X86::edx,Disp<B32>(term->offset)));
				}else
				{
					pc = X86::MOV(pc,*(Register<B32>*)regs[reg].regTerm,
								X86::AddressX86<B32>(X86::edx,Disp<B32>(term->offset)));
				}
			}else
			{
				//pc = X86::MOV(pc,*(Register<B32>*)regs[reg].regTerm,
				//	X86::AddressX86<B32>(X86::ecx,Disp<B32>(term->offset)));
				if(tstruct)
				{
					pc = X86::LEA(pc,*(Register<B32>*)regs[reg].regTerm,
								X86::AddressX86<B32>(X86::edx,Disp<B32>(term->offset)));
				}else
				{
					pc = X86::MOV(pc,*(Register<B32>*)regs[reg].regTerm,
								X86::AddressX86<B32>(X86::edx,Disp<B32>(term->offset)));
				}
			}
		}
	}
	return true;
}

bool GenSvr::RegSvr::StoreData(int reg,GenSvr::VarIndex &index)
{
	unsigned char* & pc = genSvr.coder->hCodeSpace->pc;
	if(index.kind==VarIndex::kVar || index.kind==VarIndex::kParam || index.kind==VarIndex::kTmpVar)
	{
		GenSvr::VarItemBase & item = GetVarItem(index);
		pc = X86::MOV(pc,*(X86::AddressX86<B32>*)item.addrTerm,
										*(Register<B32>*)regs[reg].regTerm);
	}
	else if(index.kind ==VarIndex::kField)
	{
		FieldItem * term = (FieldItem *)&GetVarItem(index);
		GenSvr::VarItemBase & item = GetVarItem(term->baseIndex);
		HType hClass = genSvr.context.trap->getParent(term->hTerm);
		HType hMyType = genSvr.context.trap->getField(term->hTerm)->hType;
		bool bstruct = genSvr.context.trap->getTypeKind(hClass)==TypeKind::tkStruct?true:false;
        bool tstruct = genSvr.context.trap->getTypeKind(hMyType)==TypeKind::tkStruct?true:false;
		if(genSvr.context.trap->getTermSpec(term->hTerm) & ParamSpec::byref_)tstruct = false;
		//如果是结构的话，需要复制数据，现在暂时不考虑
		
		if(tstruct) return true;

		if(bstruct)
		{
			if(item.reg==-1)
			{
				pc = X86::LEA(pc,X86::edx,*(X86::AddressX86<B32>*)item.addrTerm);
				pc = X86::MOV(pc,X86::AddressX86<B32>(X86::edx,Disp<B32>(term->offset)),
						*(Register<B32>*)regs[term->reg].regTerm);
			}else
			{
				//pc = X86::MOV(pc,X86::AddressX86<B32>(X86::ecx,Disp<B32>(term->offset)),
				//	*(Register<B32>*)regs[term->reg].regTerm);
				pc = X86::MOV(pc,X86::AddressX86<B32>(*(Register<B32>*)regs[item.reg].regTerm,Disp<B32>(term->offset)),
					*(Register<B32>*)regs[term->reg].regTerm);
			}

		}else
		{
			if(item.reg==-1)
			{
				pc = X86::MOV(pc,X86::edx,*(X86::AddressX86<B32>*)item.addrTerm);
				pc = X86::MOV(pc,X86::AddressX86<B32>(X86::edx,Disp<B32>(term->offset)),
						*(Register<B32>*)regs[term->reg].regTerm);
			}else
			{
				//pc = X86::MOV(pc,X86::AddressX86<B32>(X86::ecx,Disp<B32>(term->offset)),
				//	*(Register<B32>*)regs[term->reg].regTerm);
				pc = X86::MOV(pc,X86::AddressX86<B32>(*(Register<B32>*)regs[item.reg].regTerm,Disp<B32>(term->offset)),
					*(Register<B32>*)regs[term->reg].regTerm);
			}
		}
	}
	return true;
}


void GenSvr::RegSvr::UnUsedReg(VarIndex varIndex)
{
	GenSvr::VarItemBase & item = GetVarItem(varIndex);
	int reg = item.reg;
	if(reg < 0) return;
	item.reg = -1;

	RegUse & use =  regs[reg].used;
	if(use.count < 1) return;
	bool found = false;
	for(int  i=0;i< use.count;i++)
	{
		if(found)
		{
			use.varIndex[i - 1] = use.varIndex[i];
			continue;
		}
		if(use.varIndex[i] == varIndex)
		{
			found=true;
		}
	}
	use.count--;
}

void GenSvr::RegSvr::UseReg(int reg, VarIndex varIndex)
{
	if(reg < 0) return;
	RegUse & use =  regs[reg].used;
	
	assert(use.count<=6);
	use.varIndex[(int)use.count++] = varIndex;

	VarItemBase &item = GetVarItem(varIndex);
	item.addr = false;
	item.reg = reg;
}

bool GenSvr::RegSvr::isShared(int reg)
{
	assert(reg>0);
	return regs[reg].used.count > 1? true:false;
}
bool GenSvr::RegSvr::isUsed(int reg)
{
	assert(reg>0);
	return regs[reg].used.count > 0? true:false;
}

bool GenSvr::RegSvr::isTmpVar(VarIndex & varIndex)
{
	return varIndex.kind==GenSvr::VarIndex::kTmpVar? true:false;
}

bool GenSvr::RegSvr::isValid(int reg)
{
	return reg ==-1?false:true;
}

void GenSvr::RegSvr::SwapReg(int r1,int r2)
{
	RegUse * use1 =  &regs[r1].used;
	RegUse * use2 =  &regs[r2].used;
	for(int  i=0;i< use1->count;i++)
	{
		VarItemBase * item = (VarItemBase *)&GetVarItem(use1->varIndex[i]);
		item->reg = r2;
	}
	for(int  i=0;i< use2->count;i++)
	{
		VarItemBase * item = (VarItemBase *)&GetVarItem(use2->varIndex[i]);
		item->reg = r1;
	}

	genSvr.coder->hCodeSpace->pc=X86::XCHG(genSvr.coder->hCodeSpace->pc,
		Register<B32>(regs[r1].realReg),Register<B32>(regs[r2].realReg));
}
void GenSvr::RegSvr::UseRegReplace(int reg, VarIndex srcIndex,VarIndex destIndex)
{
	RegUse & use =  regs[reg].used;
	for(int  i=0;i< use.count;i++)
	{
		if(use.varIndex[i] == srcIndex)
		{
			use.varIndex[i] = destIndex;
			break;
		}
	}
}

void GenSvr::RegSvr::SpillReg(int reg)
{
	RegUse & use =  regs[reg].used;
	for(int  i=0;i< use.count;i++)
	{
		VarItemBase * item = &GetVarItem(use.varIndex[i]);
		if(item->addr==false)
		{
			StoreData(reg,use.varIndex[i]);
			item->addr = true;
		}
		item->reg = -1;
		regs[reg].used.count = 0;
	}
}
