#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "position.hpp"

#include <sstream>
#include <stack>

namespace numbat {
namespace parser {


ASTnode generateError (const Position & pos, const string & message);
ASTnode generateOperatorError (const Position & pos, const string & message);

ASTnode parseBody (Position pos, NumbatScope * scope);
ASTnode parseExpression (Position pos, NumbatScope * scope);
ASTnode parseNumericliteral (const Position & pos, NumbatScope * scope);
ASTnode parsePrimaryExpression (Position pos, NumbatScope * scope);
ASTnode parseType (Position * pos, NumbatScope * scope);

std::list <OperatorDecleration::OperatorMatch> generateOperatorMatches (const ParsingContext * context, Position pos);

std::vector <ASTnode> parseArgs (Position pos, NumbatScope * scope);

string parseString (const Position & pos);

void * futureFunc (void *);
void * futureStruct (void *);

};
};

#endif