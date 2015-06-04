#ifndef FUNCTION_HPP
#define FUNCTION_HPP


#include "base.hpp"

#include <vector>


struct ParseTree;

struct Function {
	
	public:
	protected:
	private:
		
		std::vector <PTNode> tplate, tags, params, type;
		string iden;
		ParseTree * body;
		
};


#endif /*FUNCTION_HPP*/