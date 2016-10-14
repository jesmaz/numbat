#pragma once


#include <forward.hpp>
#include <token.hpp>
#include <ostream>
#include <string>


namespace nir {


using std::string;


struct symbol_t {
	string iden;
	numbat::lexer::position pos={0,0};
	const numbat::File * file=nullptr;
	operator const string & () const {return iden;}
	symbol_t (const string & iden) : iden (iden) {}
	symbol_t (const string & iden, numbat::lexer::position pos) : iden (iden), pos (pos) {}
	symbol_t (const string & iden, numbat::lexer::position pos, const numbat::File * file) : iden (iden), pos (pos), file (file) {}
};


inline std::ostream & operator << (std::ostream & out, const symbol_t & sym) {
	return out << sym.iden;
}

inline const string operator + (const string & str, const symbol_t & sym) {
	return str + sym.iden;
}

};
