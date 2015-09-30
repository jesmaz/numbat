#ifndef NUMBATPARSER_HPP
#define NUMBATPARSER_HPP


#include "parser.hpp"
#include "tree/call.hpp"
#include "tree/import.hpp"
#include "tree/index.hpp"
#include "tree/list.hpp"
#include "tree/slice.hpp"
#include "tree/variable.hpp"


namespace numbat {


struct NumbatParser {
	
	public:
		
		PTNode parse (const string & prog) {return parser.parse (prog);}
		
		NumbatParser ();
		
	protected:
	private:
		
		Parser parser;
		
};


}

#endif /*NUMBATPARSER_HPP*/