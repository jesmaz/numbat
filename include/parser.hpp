#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "position.hpp"

#include <stack>

namespace numbat {
namespace parser {


ASTnode parseNumericliteral (const Position & pos, NumbatScope * scope);

string parseString (const Position & pos);

};
};

#endif