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
ASTnode parseExpression (Position pos, NumbatScope * scope, std::list <OperatorDecleration::OperatorMatch> matches);
ASTnode parseNumericliteral (const Position & pos, NumbatScope * scope);
ASTnode parseOperator (Position pos, NumbatScope * scope, const OperatorDecleration & opp, std::list <OperatorDecleration::OperatorMatch> * matches, std::vector <tkitt> matchPtr);
ASTnode parsePrimaryExpression (Position pos, NumbatScope * scope);
ASTnode parseType (Position * pos, NumbatScope * scope);

std::list <OperatorDecleration::OperatorMatch> generateOperatorMatches (const ParsingContext * context, Position pos);

std::vector <ASTnode> parseArgs (Position pos, NumbatScope * scope);

string parseString (const Position & pos);

void importModule (NumbatScope * scope, shared_ptr <Module> module, bool extention);
void parseBodyInline (Position pos, NumbatScope * scope);
void parseImport (Position pos, NumbatScope * scope);
void parseModule (Position pos, NumbatScope * scope);
void parseTypeDef (Position pos, NumbatScope * scope);

void * futureEnum (void *);
void * futureFunc (void *);
void * futureStruct (void *);
void * parseEnumDecleration (Position pos, NumbatScope * scope);
void * parseFunctionDecleration (Position pos, NumbatScope * scope);
void * parseStructDecleration (Position pos, NumbatScope * scope);


};
};

#endif