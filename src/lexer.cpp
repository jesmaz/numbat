#include "../include/lexer.hpp"

namespace numbat {
namespace lexer {



std::string next (const char * source, size_t & pos, size_t length) {
    std::string str;
    if (pos < length) {
        while (pos < length and isspace (source [pos])) {
            if (source [pos] == '\r' or source [pos] == '\n') {
                return "\n";
            }
			str += source [pos];
            ++pos;
        }
		if (str != "") {
			return str;
		}
        if (source [pos] == '#') {
			while (++pos < length and source [pos] != '\r' and source [pos] != '\n');
			return "\n";
        }
        char quote = 0;
        if (source [pos] == '\'' or source [pos] == '"') {
            quote = source [pos];
            str += quote;
            char last = quote;
            ++pos;
            while (pos < length and !(last != '\\' and source [pos] == quote)) {
                str += source [pos];
                ++pos;
            }
            if (pos < length) {
               str += source [pos];
               ++pos;
            }
        } else {
            if (!isalnum (source [pos]) and source [pos] != '_') {
                str += source [pos];
                ++pos;
			} else if (isdigit (source [pos])) {
				while (isdigit (source [pos]) or source [pos] == '.' or source [pos] == 'x' or source [pos] == 'X' or (source [pos] >= 'a' and source [pos] <= 'f') or (source [pos] >= 'A' and source [pos] <= 'F')) {
					str += source [pos];
					++pos;
				}
			} else {
                while (pos < length and (isalnum (source [pos]) or source [pos] == '_')) {
                    str += source [pos];
                    ++pos;
                }
            }
        }
    } else {
        return "\n";
    }
    return str;
}


tkstring lex (const char * source, size_t length) {
	size_t pos=0, line=0;
	tkstring tks;
	while (pos < length) {
		auto tstr = lexline (source, pos, length, ++line);
		if (tks.size () > 2 and tstr.size () > 2 and (tks.rbegin () + 1)->type == TOKEN::stringliteral and (tstr.begin () + 1)->type == TOKEN::stringliteral) {
			(tks.rbegin () + 1)->iden += (tstr.begin () + 1)->iden;
			tks += tstr.front ();
			for (auto itt=(tstr.begin () + 2); itt != tstr.end (); ++itt) {
				tks += *itt;
			}
		} else {
			tks += tstr;
		}
	}
	return tks;
}

tkstring lexline (const char * source, size_t & pos, size_t length, size_t line) {
	tkstring str;
	std::string buffer;
	
	token init;
	init.type = TOKEN::indent;
	init.iden = "";
	init.line = line;
	while (pos < length and (source [pos] == ' ' or source [pos] == '\t')) {
		init.iden += source [pos];
		++pos;
	}
	str += init;
	
	while ((buffer = next (source, pos, length)) != "\n") {
		//std::cout << buffer << std::endl;
		if (buffer.length ()) {
			token t;
			t.iden = buffer;
			if (isdigit (buffer [0])) {
				t.type = TOKEN::numericliteral;
			} else if (buffer [0] == '\'') {
				t.iden = buffer.substr (1, buffer.length ()-2);
				t.type = TOKEN::chararrayliteral;
			} else if (buffer [0] == '"') {
				t.iden = buffer.substr (1, buffer.length ()-2);
				t.type = TOKEN::stringliteral;
			} else if (buffer == "@") {
				t.type = TOKEN::atsym;
			} else if (buffer == "ref") {
				t.type = TOKEN::typemodifier;
			} else if (buffer == "asm") {
				t.type = TOKEN::assembly;
			} else if (buffer == "binary") {
				t.type = TOKEN::binary;
			} else if (buffer == "case") {
				t.type = TOKEN::casetkn;
			} else if (buffer == "const") {
				t.type = TOKEN::typemodifier;
			} else if (buffer == "def") {
				t.type = TOKEN::def;
			} else if (buffer == "else") {
				t.type = TOKEN::elsetkn;
			} else if (buffer == "extern") {
				t.type = TOKEN::externdef;
			} else if (buffer == "if") {
				t.type = TOKEN::iftkn;
			} else if (buffer == "import") {
				t.type = TOKEN::import;
			} else if (buffer == "nil") {
				t.type = TOKEN::nil;
			} else if (buffer == "operator") {
				t.type = TOKEN::operatortkn;
			} else if (buffer == "raw") {
				t.type = TOKEN::raw;
			} else if (buffer == "rawf") {
				t.type = TOKEN::rawf;
			} else if (buffer == "return") {
				t.type = TOKEN::ret;
			} else if (buffer == "struct") {
				t.type = TOKEN::structure;
			} else if (buffer == "switch") {
				t.type = TOKEN::switchtkn;
			} else if (buffer == "type") {
				t.type = TOKEN::typetkn;
			} else if (buffer == "typedef") {
				t.type = TOKEN::typedeftkn;
			} else if (buffer == "ternary") {
				t.type = TOKEN::ternary;
			} else if (buffer == "unary") {
				t.type = TOKEN::unary;
			} else if (buffer == "while") {
				t.type = TOKEN::whiletkn;
			} else if (buffer == ";") {
				t.type = TOKEN::semicolon;
			} else if (isalpha (buffer [0]) or buffer [0] == '_') {
				t.type = TOKEN::identifier;
			} else if (isspace (buffer [0])) {
				t.type = TOKEN::whitespace;
			} else {
				t.type = TOKEN::symbol;
			}
			t.line = line;
			str += t;
		}
	}
	
	std::string s;
	switch (str.back ().type) {
		default:
			break;
		case TOKEN::symbol:
			s = str.back ().iden;
			if (s != ":" and s != "}" and s != "]" and s != ")") break;
		case TOKEN::chararrayliteral:
		case TOKEN::identifier:
		case TOKEN::numericliteral:
		case TOKEN::ret:
		case TOKEN::stringliteral:
			token t;
			t.type = TOKEN::semicolon;
			t.iden = ";";
			t.line = line;
			str += t;
	}
	
	++pos;
	
	return str;
}


};
};
