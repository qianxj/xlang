#ifndef WPP5_ASTKIND_H
#define WPP5_ASTKIND_H

namespace xlang{ 

enum vlAstKind : unsigned short
{
	//真实种类
	akNilNode			= 0x401,
	akTokenNode			= 0x402,
	akAxisNode			= 0x403,
	akMissToken			= 0x404,
	akListNode			= 0x405,
	akIdentifierNode	= 0x406,
	akAstNode			= 0x407,

	akDataBuffer		= 0x408,
	akTypeData			= 0x409,

	//虚拟种类
	akBnfTerminal	= 0x601,
	akBnfLhs		= 0x602,
	akBnfRhs		= 0x603,
	akBnfRuler		= 0x604,
	akBnfGrammar	= 0x605,
	akBnfOption		= 0x606,
	akBnfRepeat		= 0x607,
	akBnfOneof		= 0x608,
	akBnfIdentifier	= 0x609,
	akBnfList		= 0x610,

	akModelModel,
	akModelSerializeList,
	akModelFeatureList,
	akModelFunctionList,
	akModelLexerList,
	akModelMacroList,
	akModelDTList,
	akModelParamList,

	akModelNode,
	akModelSerialize,
	akModelDT,
	akModelFeature,
	akModelFunction,
	akModelLexer,
	akModelMacro,
	akModelParam,

	//
	akProcNill		= akNilNode,		//empty
	akProcCall		= 0x701,			//vlAstIdentifier
	akProcLiteral	= akTokenNode,		//vlAstToken
	akProcItem		= 0x702,			//vlAstIdentifier
	akProcVarItem	= 0x703,			//vlAstIdentifier bote: index = ident
	akProcThis		= 0x704,			//vlAstIdentifier
	akProcBaseType	= 0x705,			//vlAstNode
	akProcName		= 0x706,			//vlAstIdentifier
	akCompBlock		= 0x707				//vlAstNode
};

namespace MissKind
{
	const static int none = 0;
	const static int keyword = 1;
	const static int operate = 2;
	const static int literal = 3;
	const static int ruler = 4;
	const static int oneof = 5;
	const static int option = 6;
	const static int repeat = 7;
	const static int identifier=8;
	const static int axisnode=9;
};

namespace MatchKind
{
	const static int none = 0;
	const static int keyword = 1;
	const static int operate = 2;
	const static int literal = 3;
	const static int ruler = 4;
	const static int oneof = 5;
	const static int option = 6;
	const static int repeat = 7;
	const static int identifier=8;
	const static int axisnode=9;
};


} //namespace xlang

#endif //ASTKIND