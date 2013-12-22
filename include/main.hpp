#ifndef MAIN_HPP
#define MAIN_HPP

#include "ast.hpp"
#include "numbat.hpp"
#include "generator.hpp"
#include "lexer.hpp"

#include <fstream>
#include <string>


std::string loadFromFile (const std::string & file);

void callback ();


#endif /* MAIN_HPP */
