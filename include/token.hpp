#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <utility/array.hpp>


namespace numbat {
namespace lexer {

enum class TOKEN {
	atsym,
	as,
	assembly,
	binary,
	breaktkn,
	casetkn,
	chararrayliteral,
	colon,
	continuetkn,
	def,
	eof,
	elsetkn,
	end,
	enumtkn,
	externdef,
	fortkn,
	gototkn,
	identifier,
	iftkn,
	import,
	indent,
	interface,
	label,
	nil,
	numericliteral,
	operatortkn,
	ret,
	raw, rawf,
	semicolon,
	stringliteral,
	structure,
	switchtkn,
	symbol,
	ternary,
	typedeftkn,
	typetkn,
	typemodifier,
	unary,
	whiletkn,
	whitespace,
};

struct position {
	uint32_t line, col;
};

struct token {
	TOKEN type;
	position pos;
	std::string iden;
	const bool operator < (const token & rhs) const {return  (type == rhs.type) ? iden < rhs.iden : type < rhs.type;}
	const bool operator == (const token & rhs) const {return type == rhs.type and iden == rhs.iden;}
	const bool operator != (const token & rhs) const {return type != rhs.type or iden != rhs.iden;}
	token () {}
	token (TOKEN type, const std::string & iden, position pos={0,0}) : type (type), pos (pos), iden (iden) {}
};

typedef DynArray <token, 128> tkstring;


inline tkstring::const_iterator findEOF (tkstring::const_iterator itt, const tkstring::const_iterator & end) {
	while (itt != end and itt->type != TOKEN::eof) ++itt;
	return itt;
}

inline tkstring::iterator findEOF (tkstring::iterator itt, const tkstring::iterator & end) {
	while (itt != end and itt->type != TOKEN::eof) ++itt;
	return itt;
}

inline const tkstring & operator += (tkstring & lhs, const token & rhs) {
	lhs.push_back (rhs);
	return lhs;
}

inline const tkstring & operator += (tkstring & lhs, const tkstring & rhs) {
	lhs.insert (lhs.end (), rhs.begin (), rhs.end ());
	return lhs;
}

inline const tkstring operator + (const tkstring & lhs, const token & rhs) {
	auto ret = lhs;
	ret.push_back (rhs);
	return ret;
}

inline const tkstring operator + (const tkstring & lhs, const tkstring & rhs) {
	auto ret = lhs;
	ret.insert (ret.end (), rhs.begin (), rhs.end ());
	return ret;
}


};
};


#endif
