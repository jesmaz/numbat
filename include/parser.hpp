#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "position.hpp"

#include <stack>

namespace numbat {
namespace parser {


ASTnode parseNumericliteral (const Position & pos, NumbatScope * scope);
ASTnode parseType (Position * pos, NumbatScope * scope);

std::list <OperatorDecleration::OperatorMatch> generateOperatorMatches (const ParsingContext * context, Position pos);

string parseString (const Position & pos);

};
};

#endif