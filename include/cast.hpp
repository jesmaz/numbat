#ifndef CAST_HPP
#define CAST_HPP

#include "ast/astbase.hpp"
#include "ast/callable/astcallable.hpp"

#include <vector>

namespace numbat {
namespace parser {


ASTnode createStaticCast (const ASTnode & arg, const ASTnode & type, int maxDepth=1);

shared_ptr <ASTcallable> findBestMatch (const std::vector <ASTnode> & args, const std::vector <FunctionDecleration *> & candidates, int maxDepth=1);

std::vector <ASTnode> createStaticCast (const std::vector <ASTnode> & args, const std::vector <ASTnode> & types, int maxDepth=1);


}
}

#endif /*CAST_HPP*/