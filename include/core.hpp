#ifndef CORE_HPP
#define CORE_HPP

#include "ast.hpp"

namespace numbat {
namespace parser {


ASTnode defArithmetic (AbstractSyntaxTree * ast, const string & func, const ASTnode & lhs, const ASTnode & rhs, tkitt end);
ASTnode defAssign (AbstractSyntaxTree * ast, const string & func, const ASTnode & lhs, const ASTnode & rhs, tkitt end);
ASTnode defCompare (AbstractSyntaxTree * ast, const string & func, const ASTnode & lhs, const ASTnode & rhs, tkitt end);

ASTnode parseExpression (AbstractSyntaxTree * ast, tkitt);
ASTnode parseWhileLoop (AbstractSyntaxTree * ast, tkitt);

ASTnode parseArithmeticOperator (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseArrayDecleration (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseAssignmentOperator (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseBinary (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end, defBinaryImp defImp=nullptr);
ASTnode parseComparisonOperator (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseElementReferenceOperator (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseFunctionCall (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseGenericArray (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseGenericBinary (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseGenericIndexCall (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseGenericTernary (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseGenericUnaryPrefix (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseGenericUnaryPostfix (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseRedirectOperator (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);
ASTnode parseTupleOperator (AbstractSyntaxTree * ast, const string & func, const std::vector <tkitt> & oppLoc, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end);

shared_ptr <ASTcallable> findBestMatch (AbstractSyntaxTree * ast, const std::vector <ASTnode> & args, const std::vector <shared_ptr <FunctionDecleration>> & candidates);


};
};

#endif /*CORE_HPP*/