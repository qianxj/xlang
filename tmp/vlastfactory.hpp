#ifndef WPP5_ASTFACTORY_H
#define WPP5_ASTFACTORY_H

#include "vlAstNode.hpp"
#include "vlAstAxis.hpp"
#include "vlAstToken.hpp"
#include "vlAstMissToken.hpp"
#include "vlAstIdentifier.hpp"
#include "vlasttypedata.hpp"
#include "vlAstDataBuffer.hpp"
#include <vector>

namespace xlang{ 

class vlAstFactory
{
public:
	 vlAstNode * CreateNode(const wchar_t* nodeName);
public:
	vlAstAxis * CreateAxis(vlAstNode * node);
	vlAstNode * CreateAstNode(vlAstKind kind);
	vlAstIdentifier * CreateAstIdentifier(vlAstKind,int ide);
	vlAstIdentifier * CreateAstIdentifier(vlAstKind,int ide,vlToken &token);
	vlAstIdentifier * CreateAstIdentifier(int ide,vlToken &token);
	vlAstToken * CreateAstToken(vlAstKind kind,vlToken &token);
	vlAstToken * CreateAstToken(vlToken &token);
	vlAstMissToken * CreateAstMissToken(int missKind,vlToken &token);

	vlAstDataBuffer* CreateAstDataBuffer();
	vlAstTypeData* CreateAstTypeData();
public:
	unsigned char* EnsureBuffuer(int len);
public:
	vlAstFactory();
	~vlAstFactory();
private:
	int blockUsed_;
	int blockSize_ ;
public:
	std::vector<unsigned char * >* blocks_;
public:
	std::vector<unsigned char * >* blocksfree_;
	struct FCheckPoint
	{
		int blockUsed_;
		int blockSize_ ;
		int indexBlock;
	};
	int checkpoint();
	int restorepoint();
	std::vector<FCheckPoint> checkPoints_;
};


} //namespace xlang

#endif //ASTFACTORY