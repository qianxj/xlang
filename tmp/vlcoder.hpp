#ifndef WPP5_VLCODER_H
#define WPP5_VLCODER_H

#include <vector>
#include <map>
#include <string>

namespace xlang{

class vlCoder
{
public:
	vlCoder();
	~vlCoder();
public:
	struct CodeSpce
	{
		unsigned char * code;
		unsigned char * pc;
		int length;
	} * hCodeSpace;

	CodeSpce * allocCodeSpace(int len = 4096*1024);
	void freeCodeSpace(CodeSpce * p);
public:
	int emit(unsigned char *data, int len);
	int emit(unsigned char data);
	int emit(short data);
	int emit(int data);
	int emit(void* data);
	int emit(CodeSpce * p ,unsigned char *data, int len);
	int emit(CodeSpce * p ,unsigned char data);
	int emit(CodeSpce * p ,short data);
	int emit(CodeSpce * p ,int data);
	int emit(CodeSpce * p ,void* data);

	unsigned char * getCodePc();
public:
	struct Label
	{
		bool initial;
		int dest;
		std::vector<int> links;
		Label():dest(0),initial(false){};
		void Reset()
		{
			initial = 0;
			dest = 0;
			links.clear();
		}
	};
	std::vector<Label *> labels;
	std::vector<int> freeLabels;
	int  askLabel();
	void  freeLabel(int hlabel);
	void toLabel(int hlabel ,bool addr = false);
	void label(CodeSpce * p,int hlabel,bool addr = false);
	void labelas(CodeSpce * p,int hlabel,int  hlabelas);
	void label(int hlabel,bool addr = false);
	void labelas(int hlabel,int  hlabelas);
private:
	int register_;
	int register_count_;
public:
	int curRegister();
	int askRegister();
	void freeRegister();
	void clearRegister(bool clearRegisterCount = false);
	int getRegisterCount();
public:
	bool Disassemble(unsigned char * addr ,std::wstring & decodestr);
};

} //namespace xlang

#endif //WPP5_VLCODER_H