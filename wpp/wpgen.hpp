#ifndef WPGEN_H
#define WPGEN_H

#include "wpsymnode.hpp"
#include "wpcode.hpp"

namespace xl {namespace wp {

coder::PNode * genTerm(WPContext* context, HSymbol rulerID);
coder::PNode * genTerm(WPContext* context, SymDeclVar* 	symVar);
coder::PNode * genTerm(WPContext* context, SymOneof* 	symOneof);
coder::PNode * genTerm(WPContext* context, SymRepeat* 	symRepeat);
coder::PNode * genTerm(WPContext* context, SymAction* 	symAction);
coder::PNode * genTerm(WPContext* context, SymAssign* 	symAssign);
coder::PNode * genTerm(WPContext* context, SymLiteral* 	symLiteral);
coder::PNode * genTerm(WPContext* context, SymKeyword* 	symKeyword);
coder::PNode * genTerm(WPContext* context, SymKeyword* 	symOperate);
coder::PNode * genTerm(WPContext* context, SymTerm* 	symTerm);
coder::PNode * genTerm(WPContext* context, SymNode<SymKind> * node);

coder::PNode * genCond(WPContext* context, FirstSet * firstset);

}} //namespace xl::wp


#endif //WPGEN_H