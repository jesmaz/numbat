#pragma once


#include <string>

using std::string;


namespace numbat {


struct File;


}


namespace nir {


struct Module;
struct Scope;


}


namespace parser {


using std::string;


enum class OPERATION {ADD, AND, AS, ASSIGN, BAND, BNOT, BOR, BXOR, CMPEQ, CMPGT, CMPGTE, CMPLT, CMPLTE, CMPNE, CONCAT, DECREMENT, DIV, IN, INCREMENT, LNOT, MUL, NEG, NONE, OR, REM, SUB};


class ParseTreeNode;
struct Function;
struct Struct;

typedef ParseTreeNode * PTNode;


}
