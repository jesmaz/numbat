#include <utility/alphanumeric.hpp>

std::string numToAlphabet (size_t k) {
	if (k < 26) {
		return std::string () + char ('a' + k);
	} else {
		return numToAlphabet (k/26) + char ('a' + k%26);
	}
}