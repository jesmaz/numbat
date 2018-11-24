#pragma once


#include <forward.hpp>
#include <memory>


struct symbol_t {
	
	public:
		
		const bool operator == (const symbol_t & rhs) const {
			if (str and rhs.str) {
				return *str == *rhs.str;
			} else {
				return str.get () == rhs.str.get ();
			}
		}
		
		const bool operator < (const symbol_t & rhs) const {
			if (str and rhs.str) {
				return *str < *rhs.str;
			} else {
				return str.get () < rhs.str.get ();
			}
		}
		
		const string & operator * () const {
			return *str;
		}
		
		symbol_t () : str (nullptr) {}
		symbol_t (const string & str) : str (new string (str)) {}
		symbol_t (const symbol_t & other) : str (other.str ? new string (*other.str) : nullptr) {}
		
		const symbol_t & operator = (const symbol_t & other) {if (other.str) {str = std::make_unique <string> (*other.str);} else {str = nullptr;} return *this;}
		
	private:
		
		std::unique_ptr <string> str;
		
};
