#include "../include/ast/type/numbatrawtype.hpp"

namespace numbat {
namespace parser {


string NumbatRawType::convertDataToString (const void * data, size_t & offset) const {
	std::ostringstream os;
	if (size == 64) {
		os << *reinterpret_cast <const uint64_t *> (data);
		offset += 8;
	} else if (size == 32) {
		os << *reinterpret_cast <const uint32_t *> (data);
		offset += 4;
	} else if (size == 16) {
		os << *reinterpret_cast <const uint16_t *> (data);
		offset += 2;
	} else if (size == 8) {
		os << *reinterpret_cast <const uint8_t *> (data);
		offset += 1;
	} else {
		os << "unknown";
	}
	return os.str ();
}

string NumbatRawType::toString (const string & indent) const {
	std::ostringstream ss;
	ss << "raw ";
	switch (type) {
		case FLOAT:
			ss << "@fint ";
			break;
		case SIGNED:
			ss << "@sint ";
			break;
		case UNSIGNED:
			ss << "@uint ";
			break;
	}
	ss << size;
	return ss.str ();
}

}
}