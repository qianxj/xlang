
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "v8.h"
#define V8_TARGET_ARCH_IA32

#include "disasm.h"
#include "vlplatform.hpp"
#include "vlcoder.hpp"
#include "vlCoder.hpp"

using namespace xlang;

vlCoder::vlCoder():register_(0),register_count_(0)
{
	hCodeSpace = allocCodeSpace();
}

vlCoder::~vlCoder()
{
	freeCodeSpace(hCodeSpace);
}


vlCoder::CodeSpce * vlCoder::allocCodeSpace(int len )
{
	int prot = PAGE_EXECUTE_READWRITE ;
	int msize = len;

	LPVOID mbase = VirtualAlloc(NULL,
		msize,
		MEM_COMMIT | MEM_RESERVE,
		prot);
	if (mbase == NULL)
	{
		printf("\r\n%s","error call VirtualAlloc.");
		return 0;
	}
	CodeSpce * p = new CodeSpce();
	p->code = (unsigned char*)mbase;
	p->pc = p->code;
	p->length = msize;

	return p;
}

void vlCoder::freeCodeSpace(CodeSpce * p)
{
	VirtualFree(p->code,p->length,MEM_RELEASE);
}



int vlCoder::curRegister()
{
	return register_ - 1;
}

int vlCoder::askRegister()
{
	if(register_count_< register_) register_count_ = register_;
	return register_++;
}

void vlCoder::freeRegister(){
	
	if(register_ > 0) register_--;
}

void vlCoder::clearRegister(bool clearRegisterCount)
{
	register_ = 0;
	if(clearRegisterCount) register_count_ = 0;
}

int vlCoder::getRegisterCount()
{
	return register_count_;
}

int vlCoder::emit(unsigned char *data, int len)
{
	return emit(hCodeSpace,data,len);
}

int vlCoder::emit(unsigned char data)
{
	return emit(hCodeSpace,data);
}

int vlCoder::emit(short data)
{
	return emit(hCodeSpace,data);
}

int vlCoder::emit(int data)
{
	return emit(hCodeSpace,data);
}

int vlCoder::emit(void* data)
{
	return emit(hCodeSpace,data);
}

int vlCoder::emit(CodeSpce * p ,unsigned char *data, int len)
{
	if(p->pc - p->code + len >= p->length) return -1;
	memcpy_s(p->pc,len,data,len);
	p->pc += len;
	return 1;
}
int vlCoder::emit(CodeSpce * p ,unsigned char data)
{
	if(p->pc - p->code + 32 >= p->length) return -1;
	*p->pc++ = data;
	return 1;
}

int vlCoder::emit(CodeSpce * p ,short data)
{
	if(p->pc - p->code + 32 >= p->length) return -1;
	*(short *)p->pc = data;
	p->pc += 2; 
	return 1;
}

int vlCoder::emit(CodeSpce * p ,int data)
{
	if(p->pc - p->code + 32 >= p->length) return -1;
	*(int*)p->pc = data;
	p->pc+=4;
	return 1;
}

int vlCoder::emit(CodeSpce * p ,void* data)
{
	if(p->pc - p->code + 32 >= p->length) return -1;
	*(void **)p = data;
	p->pc+=4;
	return 1;
}

unsigned char *  vlCoder::getCodePc()
{
	return hCodeSpace->pc;
}

void vlCoder::toLabel(int hlabel,bool addr )
{
	Label* l = labels[hlabel - 1];
	if(!addr)
	{
		if(l->initial)
			*(int*)hCodeSpace->pc = l->dest - (hCodeSpace->pc - hCodeSpace->code + 4);
		else
			l->links.push_back(hCodeSpace->pc - hCodeSpace->code + 4);
	}else
	{
		if(l->initial)
			*(unsigned char **)hCodeSpace->pc = hCodeSpace->code + l->dest ;
		else
			l->links.push_back(hCodeSpace->pc - hCodeSpace->code + 4);
	}
	hCodeSpace->pc+=4;
}


int  vlCoder::askLabel()
{
	if(freeLabels.size())
	{
		int label= freeLabels.back();
		freeLabels.pop_back();
		return  label;
	}else
	{
		labels.push_back(new Label);
		return (int)labels.size();
	}
}

void vlCoder::freeLabel(int hlabel)
{
	labels[hlabel -1]->Reset();
	freeLabels.push_back(hlabel);
}

void vlCoder::label(int hlabel,bool addr)
{
	return label(this->hCodeSpace,hlabel,addr);
}

void vlCoder::labelas(int hlabel,int hlabelas)
{
	return labelas(this->hCodeSpace,hlabel,hlabelas);
}

void vlCoder::label(CodeSpce * p, int hlabel,bool addr)
{
	Label* l = labels[hlabel - 1];
	l->initial = true;
	l->dest = p->pc - p->code;
	for(int i=0;i<(int)l->links.size();i++)
	{
		if(addr)
			*((unsigned char **)(p->code + l->links[i] - 4)) = p->code + l->dest;
		else
			*(int *)(p->code + l->links[i] - 4) = l->dest - l->links[i];
	}
	l->links.clear();
}

void vlCoder::labelas(CodeSpce * p, int hlabel,int hlabelas)
{
	Label* l  = labels[hlabel - 1];
	Label* sl = labels[hlabelas - 1];
	l->dest = sl->dest;
	for(int i=0;i<(int)l->links.size();i++)
	{
		*(int *)(p->code + l->links[i]) = l->dest - l->links[i];
	}
	l->links.clear();
}

bool vlCoder::Disassemble(unsigned char * addr ,std::wstring & decodestr)
{
	v8::internal::EmbeddedVector<char,64> buffer;
	unsigned char * instruction = (unsigned char*)addr;
	if(!instruction)
	{
		decodestr = L"";
		return false;
	}

	disasm::NameConverter converter;
	disasm::Disassembler decoder(converter);

	std::wstring str;
	while(true)
	{
		int len = decoder.InstructionDecode(buffer,instruction);
		if(!len) break;
		wchar_t buf[512];

		swprintf_s(buf,512,L"\n\t\t0x%x %S;",instruction,buffer.start());
		decodestr +=buf;  
		if(instruction[0]==0xC3||instruction[0]==0xC2) break;
		instruction += len;
	}

	return true;
}
