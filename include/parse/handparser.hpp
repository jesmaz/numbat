#pragma once

#include <parse/tree/base.hpp>


//Why have a complicated interface?
//These two functions should be sufficient

//Both take in an input program and build a parse tree, don't forget to delete it when you are finished with it
PTNode parse (const string & program);
PTNode parse (numbat::lexer::tkstring::const_iterator start, numbat::lexer::tkstring::const_iterator end);