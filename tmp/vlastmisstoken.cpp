#include "vlastmisstoken.hpp"

using namespace xlang;

vlAstMissToken::vlAstMissToken(int missKind): missKind(missKind),vlAstToken(akMissToken)
{
}

vlAstMissToken::vlAstMissToken(int missKind,vlToken& token ): missKind(missKind),vlAstToken(akMissToken,token)
{
}

vlAstMissToken::vlAstMissToken(int missKind,vlAstNode* node, vlToken& token):
	missKind(missKind),node(node), vlAstToken(akMissToken,token)
{
}
