#ifndef CORE_HPP
#define CORE_HPP

#include "ast.hpp"

namespace numbat {
namespace parser {


ASTnode parseExpression (AbstractSyntaxTree * ast, tkitt);
ASTnode parseWhileLoop (AbstractSyntaxTree * ast, tkitt);

ASTnode parseElementReferenceOperator (AbstractSyntaxTree * ast, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseTupleOperator (AbstractSyntaxTree * ast, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);


};
};

#endif /*CORE_HPP*/