#pragma once

#include <string>


namespace nir {


enum class LINKAGE {EXTERNAL, LOCAL};

class Function;
class Instruction;
class Number;
class Parameter;
class PointerType;
class Struct;
class Type;

struct Block;
struct Builder;
struct Module;
struct Scope;

using std::string;

typedef const string * symbol;


}
