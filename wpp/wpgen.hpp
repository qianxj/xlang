#ifndef WPGEN_H
#define WPGEN_H

#include "wpsymnode.hpp"


namespace xl {namespace wp {


void * genTerm(int ident);

void * genSynTerm(SymDeclVar* 	symVar);
void * genSynTerm(SymOneof* 	symOneof);
void * genSynTerm(SymLoop* 		symLoop);
void * genSynTerm(SymAction* 	symAction);
void * genSynTerm(SymAssign* 	symAssign);
void * genSynTerm(SymLiteral* 	symLiteral);
void * genSynTerm(SymKeyword* 	symKeyword);
void * genSyn(SymNode<SymKind> * node);

}} //namespace xl::wp


#endif //WPGEN_H