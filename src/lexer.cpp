#include <lexer.hpp>

namespace numbat {
namespace lexer {



std::string next (const char * source, size_t & pos, size_t length) {
	std::string str;
	if (pos < length) {
		while (pos < length and isspace (source [pos])) {
			if (source [pos] == '\r' or source [pos] == '\n') {
				return "\n";
			}
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
				last = source [pos];
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
				while (isdigit (source [pos]) or source [pos] == '.' or source [pos] == 'x' or source [pos] == 'X' or source [pos] == 'q' or source [pos] == 'Q' or source [pos] == 'd' or source [pos] == 'D' or source [pos] == 'f' or source [pos] == 'F' or source [pos] == 'u' or source [pos] == 'U' or (source [pos] >= 'a' and source [pos] <= 'f') or (source [pos] >= 'A' and source [pos] <= 'F')) {
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
	size_t pos=0;
	uint32_t line=0;
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

tkstring lexline (const char * source, size_t & pos, size_t length, uint32_t line) {
	tkstring str;
	std::string buffer;
	size_t lineStart = pos;
	
	while (pos < length and (source [pos] == ' ' or source [pos] == '\t')) {
		++pos;
	}
	
	while ((buffer = next (source, pos, length)) != "\n") {
		//std::cout << buffer << std::endl;
		if (buffer.length ()) {
			token t = lexToken (buffer);
			t.pos = {line, uint32_t (pos + 1 - lineStart - t.iden.size ())};
			str += t;
		}
	}
	
	if (not str.empty ()) {
		
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
				t.iden = "";
				t.pos = {line, uint32_t (pos + 1 - lineStart)};
				str += t;
		}
		
	}
	
	++pos;
	
	return str;
}

token lexToken (const std::string & source) {
	token t;
	t.iden = source;
	if (isdigit (source [0])) {
		t.type = TOKEN::numericliteral;
	} else if (source [0] == '\'') {
		t.iden = source.substr (1, source.length ()-2);
		t.type = TOKEN::chararrayliteral;
	} else if (source [0] == '"') {
		t.iden = source.substr (1, source.length ()-2);
		t.type = TOKEN::stringliteral;
	} else if (source == "@") {
		t.type = TOKEN::atsym;
	} else if (source == ":") {
		t.type = TOKEN::colon;
	} else if (source == "as") {
		t.type = TOKEN::as;
	} else if (source == "asm") {
		t.type = TOKEN::assembly;
	} else if (source == "binary") {
		t.type = TOKEN::binary;
	} else if (source == "case") {
		t.type = TOKEN::casetkn;
	} else if (source == "const") {
		t.type = TOKEN::typemodifier;
	} else if (source == "def") {
		t.type = TOKEN::def;
	} else if (source == "else") {
		t.type = TOKEN::elsetkn;
	} else if (source == "end") {
		t.type = TOKEN::end;
	} else if (source == "enum") {
		t.type = TOKEN::enumtkn;
	} else if (source == "extern") {
		t.type = TOKEN::externdef;
	} else if (source == "for") {
		t.type = TOKEN::fortkn;
	} else if (source == "if") {
		t.type = TOKEN::iftkn;
	} else if (source == "import") {
		t.type = TOKEN::import;
	} else if (source == "interface") {
		t.type = TOKEN::interface;
	} else if (source == "nil") {
		t.type = TOKEN::nil;
	} else if (source == "operator") {
		t.type = TOKEN::operatortkn;
	} else if (source == "raw") {
		t.type = TOKEN::raw;
	} else if (source == "rawf") {
		t.type = TOKEN::rawf;
	} else if (source == "return") {
		t.type = TOKEN::ret;
	} else if (source == "struct") {
		t.type = TOKEN::structure;
	} else if (source == "switch") {
		t.type = TOKEN::switchtkn;
	} else if (source == "type") {
		t.type = TOKEN::typetkn;
	} else if (source == "typedef") {
		t.type = TOKEN::typedeftkn;
	} else if (source == "ternary") {
		t.type = TOKEN::ternary;
	} else if (source == "unary") {
		t.type = TOKEN::unary;
	} else if (source == "while") {
		t.type = TOKEN::whiletkn;
	} else if (source == ";") {
		t.type = TOKEN::semicolon;
	}else if (source == "and" or source == "in" or source == "not" or source == "or" or source == "ref" or source == "var" or source == "vref" or source == "val") {
		t.type = TOKEN::symbol;
	} else if (isalpha (source [0]) or source [0] == '_') {
		t.type = TOKEN::identifier;
	} else if (isspace (source [0])) {
		t.type = TOKEN::whitespace;
	} else {
		t.type = TOKEN::symbol;
	}
	return t;
}


};
};

