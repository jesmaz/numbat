#ifndef LEXER_HPP
#define LEXER_HPP

#include "../include/token.hpp"

#include <cctype>
#include <cstdint>
#include <iostream>
#include <string>
#include <sstream>


namespace numbat {
namespace lexer {


std::string next (const char * source, size_t & pos, size_t length);

tkstring lex (const char * source, size_t length);
inline tkstring lex (const std::string & source) {return lex (source.c_str (), source.length ());}
inline tkstring lexFile (const std::string & source) {return lex (source.c_str (), source.length ()) + token (TOKEN::eof, "End of file");}
tkstring lexline (const char * source, size_t & pos, size_t length, uint32_t line=0);
token lexToken (const std::string & source);


};
};

#endif /* LEXER_HPP */
