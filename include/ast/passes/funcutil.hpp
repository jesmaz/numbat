#pragma once


#include <forward.hpp>
#include <utility/array.hpp>


namespace AST {


NodePtr createCall (numbat::lexer::position pos, const numbat::File * file, const FuncPtr & func, const BasicArray <NodePtr> & args, size_t & score);


}

