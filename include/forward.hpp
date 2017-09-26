#pragma once


#include <memory>
#include <string>

using std::string;


namespace AST {


class Type;
class Node;
class Value;
class Variable;

struct Context;
struct Function;

typedef std::shared_ptr <Function> FuncPtr;
typedef std::shared_ptr <Node> NodePtr;
typedef std::shared_ptr <Type> TypePtr;
typedef std::shared_ptr <Value> ValPtr;
typedef std::shared_ptr <Variable> VarPtr;


}


namespace numbat {


struct File;


namespace lexer {


struct position {
	uint32_t line, col;
};


}


}


namespace nir {


struct Module;
struct Scope;


}


namespace parser {


using std::string;


enum class OPERATION {ADD, AND, AS, ASSIGN, BAND, BNOT, BOR, BXOR, CMPEQ, CMPGT, CMPGTE, CMPLT, CMPLTE, CMPNE, CONCAT, DECREMENT, DIV, IN, INCREMENT, INDEX, LNOT, MUL, NEG, NONE, OR, REM, SUB, __COUNT__};


class ParseTreeNode;
struct Function;
struct Struct;

typedef ParseTreeNode * PTNode;


}
