#pragma once


#include <utility/literals.hpp>
#include <vector>


struct LiteralStack {
	
	public:
		
		Literal & operator [] (size_t index) {
			if (literals.size () <= index) {
				literals.resize (index + 1);
			}
			return literals [index];
		}
		
		size_t reserve () {
			literals.push_back (Literal ());
			return literals.size () - 1;
		}
		
	protected:
	private:
		
		std::vector <Literal> literals;
		
};
