#include <parse/tree/keyword.hpp>


namespace parser {


string ParseTreeKeyword::strDump (text::PrintMode mode) {
	return mode&text::COLOUR ? text::blu + sym + text::reset : sym;
}


}
