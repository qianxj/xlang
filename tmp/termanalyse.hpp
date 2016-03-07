#ifndef WPP_TERMANALYSE_H
#define WPP_TERMANALYSE_H

#include "term.hpp"

namespace trap {
namespace complier {

coder::astNode* makeMatchToken(LangContext & context,int pos);
coder::astExpr* getBranchCond(LangContext & context, std::set<int>& firstSet);
int analysePushSpendingSyntaxTerm(LangContext & context, int term);
coder::astNode* makeMatchToken(LangContext & context,int pos);
coder::astNode* analyseSyntax(LangContext & context, int term, int pos, bool bbreak = false,bool bsure = true);
coder::astNode* sureSyntax(LangContext & context, int term, int pos, bool bbreak = false,bool bsure = true);
coder::astNode* optionSureSyntax(LangContext & context, int term, int pos,coder::astNode* stmtFalseProcess);
int analyseTermSyntax(LangContext & context, int term);

int analyseTerm(LangContext & context, int term);
int analyseTermContainer(LangContext & context, int term);
int analyseTerm(LangContext & context, int term);
int analyse(LangContext & context);

}//complier
}//trap

#endif //WPP_TERMANALYSE_H