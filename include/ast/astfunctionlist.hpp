#ifndef ASTFUNCTIONLIST_HPP
#define ASTFUNCTIONLIST_HPP


#include "astbase.hpp"
#include "functiondecleration.hpp"

#include <list>


namespace numbat {
namespace parser {


class ASTfunctionlist : public ASTbase {
	
	public:
		
		const std::vector <FunctionDecleration *> & getElements () const {return elements;}
		virtual bool isParsed () const {return true;}
		virtual bool isValid () const {return true;}
		virtual size_t calculateWeight () const {return 0;}
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + iden;}
		
		ASTfunctionlist () {}
		ASTfunctionlist (const string & iden, const std::vector <shared_ptr <FunctionDecleration>> & ein) : iden (iden) {for (auto e : ein) {elements.push_back (e.get ());}}
		ASTfunctionlist (const string & iden, const std::vector <FunctionDecleration *> & elements) : iden (iden), elements (elements) {}
		
	private:
		
		string iden;
		std::vector <FunctionDecleration *> elements;
		
};

};
};

#endif /*ASTFUNCTIONLIST_HPP*/