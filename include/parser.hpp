#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "context.hpp"
#include "position.hpp"

namespace numbat {
namespace parser {


void parseBody (Parser parser, NumbatScope * body);


};
};

#endif