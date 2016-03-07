#ifndef WPP5_VLMASM_H
#define WPP5_VLMASM_H

namespace xlang{

namespace X86
{
	struct Register
	{
		int r_;
		bool is(Register r){return r.r_==r_;}
		operator const unsigned char () const {return (const unsigned char)r_;}
		operator const int () const {return r_;}
	};

	const Register 
		no_reg = { -1 },
		eax = { 0 },
		ecx = { 1 },
		edx = { 2 },
		ebx = { 3 },
		esp = { 4 },
		ebp = { 5 },
		esi = { 6 },
		edi = { 7 };

	struct XMMRegister 
	{
		int r_;
		bool is(XMMRegister r){return r.r_==r_;}
		operator const unsigned char () const {return (const unsigned char)r_;}
		operator const int () const {return r_;}
	};

	const XMMRegister
		xmm0 = {0},
		xmm1 = {0},
		xmm2 = {0},
		xmm3 = {0},
		xmm4 = {0},
		xmm5 = {0},
		xmm6 = {0},
		xmm7 = {0};

	struct ModRM
	{
		unsigned char rm_:3;
		unsigned char reg_:3;
		unsigned char mod_:2;

		ModRM():rm_(0),reg_(0),mod_(0){}
		ModRM(const unsigned char mod,const unsigned char reg,const unsigned char rm):
			rm_(rm),reg_(reg),mod_(mod){}
	};

	struct Scaled
	{
		const static unsigned char time_1 = 0;
		const static unsigned char time_2 = 1;
		const static unsigned char time_4 = 2;
		const static unsigned char time_8 = 3;
	};

	struct SIB
	{
		unsigned char base_:3;
		unsigned char index_:3;
		unsigned char scale_:2;

		SIB():base_(0),index_(0),scale_(0){}
		SIB(const unsigned char scale,const unsigned char index,const unsigned char base):
			base_(base),index_(index),scale_(scale){}
	};

	struct Imm32
	{
		int v_;

		inline bool is_zero() const { return v_ == 0; }
		inline bool is_int8() const { return -128 <= v_ && v_ < 128; }
		inline bool is_int16()const { return -32768 <= v_ && v_ < 32768;}
		operator int () {return v_;}
		operator char () {return (char)v_;}
		operator short () {return (short)v_;}
		Imm32():v_(0){}
		Imm32(int v):v_(v){}
	};

	struct Displacement
	{
		union 
		{
			char	d8_;
			short	d16_;
			int		d32_;
		};
	};

	struct Addr
	{
	public:
		inline explicit Addr(Register r)
		{
			this->len_ = 1;
			modrm_ = ModRM(3,0,r);
		}
		inline explicit Addr(XMMRegister x)
		{
			this->len_ = 1;
			modrm_ = ModRM(3,0,x);
		}
		inline explicit Addr(int disp)
		{
			this->len_ = 5;
			modrm_ = ModRM(0,0,ebp);
			disp_.d32_ = disp;
		}
		explicit Addr(Register r,Imm32 disp);
		explicit  Addr(SIB sib,Imm32 disp);
		explicit Addr(Register index,const unsigned char scale,Imm32 disp);
	public:
		inline void encoder(Register r){modrm_.reg_ = r;}
		inline unsigned char mod(){return buf_[0];}
		inline unsigned char rm(){return buf_[1];}

		union
		{
			unsigned char buf_[6];
			struct
			{
				ModRM modrm_;
				SIB	  sib_;
				Displacement disp_;
			};
		};

		int len_;
	};
}

using namespace X86;

class vlMasm
{
public:
	unsigned char * base;
	unsigned char * last;
	unsigned char * pc;
public:
	vlMasm():base(0),last(0),pc(0){}
public: 
	void ensurePC( unsigned char * & pc );
public:
	void mov(Register r1, Register r2);
	void mov(Register r,  Addr addr);
	void mov(Addr addr,	  Register r);
	void mov(Register r,  Imm32 imm);
	void mov(Addr addr,	  Imm32 imm);
public:
	void add(Register r1, Register r2);
	void add(Register r,  Addr addr);
	void add(Addr addr,	  Register r);
	void add(Register r,  Imm32 imm);
	void add(Addr addr,	  Imm32 imm);
public:
	void sub(Register r1, Register r2);
	void sub(Register r,  Addr addr);
	void sub(Addr addr,	  Register r);
	void sub(Register r,  Imm32 imm);
	void sub(Addr addr,	  Imm32 imm);
public:
	void mul(Register r1, Register r2);
	void mul(Register r,  Addr addr);
	void mul(Addr addr,	  Register r);
	void mul(Register r,  Imm32 imm);
	void mul(Addr addr,	  Imm32 imm);
public:
	void div(Register r1, Register r2);
	void div(Register r,  Addr addr);
	void div(Addr addr,	  Register r);
	void div(Register r,  Imm32 imm);
	void div(Addr addr,	  Imm32 imm);
public:
	void and(Register r1, Register r2);
	void and(Register r,  Addr addr);
	void and(Addr addr,	  Register r);
	void and(Register r,  Imm32 imm);
	void and(Addr addr,	  Imm32 imm);
public:
	void or(Register r1, Register r2);
	void or(Register r,  Addr addr);
	void or(Addr addr,	 Register r);
	void or(Register r,  Imm32 imm);
	void or(Addr addr,	 Imm32 imm);
public:
	void push(Register r);
	void push(Addr addr);
	void push(Imm32 imm);
public:
	void ret();
	void ret(unsigned char);
public:
	void nop();
	void cdr();
	void br();
};

}

#endif //WPP5_VLMASM_H

