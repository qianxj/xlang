#ifndef WPP5_ASTMISSTOKEN_H
#define WPP5_ASTMISSTOKEN_H

#include "vlastkind.hpp"
#include "vlAstToken.hpp"

namespace xlang
{

class vlAstMissToken : public vlAstToken
{
public:
	vlAstMissToken(int missKind);
	vlAstMissToken(int missKind,vlToken& token );
	vlAstMissToken(int missKind,vlAstNode* node, vlToken& token);
public:
	int missKind;
	vlAstNode * node;
};


}

#endif //WPP5_ASTMISSTOKEN_H