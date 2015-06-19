#ifndef NUMBATPARSER_HPP
#define NUMBATPARSER_HPP


#include "parser.hpp"


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