#ifndef WPP_TERMEVAL_H
#define WPP_TERMEVAL_H

#include "term.hpp"

namespace trap {
namespace complier {

std::wstring eval(LangContext & context, coder::astNode * node);
std::wstring eval(LangContext & context, coder::astNode * node,std::wstring space);
std::wstring eval(LangContext & context,int term);
std::wstring eval(LangContext & context);

}}

#endif //WPP_TERMEVAL_H
