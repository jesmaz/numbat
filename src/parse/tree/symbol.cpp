#include <parse/tree/symbol.hpp>


namespace parser {


string ParseTreeSymbol::strDump (text::PrintMode mode) {
	return mode&text::COLOUR ? text::cyn + sym + text::reset : sym;
}


}
