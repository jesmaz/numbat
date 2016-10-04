#pragma once

#include <forward.hpp>
#include <lexer.hpp>
#include <parse/tree/base.hpp>


namespace parser {

//Why have a complicated interface?
//These two functions should be sufficient

//Both take in an input program and build a parse tree, don't forget to delete it when you are finished with it
PTNode parse (const string & program, const numbat::File * file);
PTNode parse (numbat::lexer::tkstring::const_iterator start, numbat::lexer::tkstring::const_iterator end, const numbat::File * file);


}
