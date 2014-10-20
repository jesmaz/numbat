#ifndef CORE_HPP
#define CORE_HPP

#include "ast.hpp"
#include "cast.hpp"
#include "parser.hpp"

namespace numbat {
namespace parser {


ASTnode defArithmetic (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args);
ASTnode defAssign (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args);
ASTnode defCompare (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args);
ASTnode defConcat (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args);
ASTnode defLogic (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args);
ASTnode defNegation (NumbatScope * scope, const string & func, const std::vector <ASTnode> & args);

ASTnode parseArrayDecleration (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseAssignmentOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseBinary (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseBlockOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseCall (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseComma (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseFunctionCall (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseIfStatment (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseIndex (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseRedirectOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseReferenceOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseScopeOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseSubExpression (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseTernaryOperator (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseUnary (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);
ASTnode parseWhileLoop (NumbatScope * scope, const string & func, const std::vector <Position> & args, std::list <OperatorDecleration::OperatorMatch> * matches, OperatorDecleration::DefaultImplementation defImp);


};
};

#endif /*CORE_HPP*/