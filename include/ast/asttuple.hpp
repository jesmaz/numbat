#ifndef ASTTUPLE_HPP
#define ASTTUPLE_HPP

#include "astbase.hpp"

#include <list>


namespace numbat {
namespace parser {


class ASTtuple : public ASTbase {
	VISITABLE
	public:
		const std::list <ASTnode> & getElements () const {return elements;}
		virtual bool isValid () const;
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const;
		
		ASTtuple () {}
		ASTtuple (const ASTnode & single) : elements (1, single) {}
		ASTtuple (const ASTnode & lhs, const ASTnode & rhs) : elements () {elements.push_back (lhs); elements.push_back (rhs);}
		ASTtuple (const ASTnode & lhs, const std::list <ASTnode> & lst) : elements (lst) {elements.push_front (lhs);}
		ASTtuple (const std::list <ASTnode> & lst) : elements (lst) {}
		ASTtuple (const std::list <ASTnode> & lst, const ASTnode & rhs) : elements (lst) {elements.push_back (rhs);}
		ASTtuple (const std::list <ASTnode> & lhs, const std::list <ASTnode> & rhs) : elements (lhs) {elements.insert (elements.end (), rhs.begin (), rhs.end ());}
		ASTtuple (const std::vector <ASTnode> & lst) : elements (lst.begin (), lst.end ()) {}
	private:
		std::list <ASTnode> elements;
};


}
}


#endif /*ASTTUPLE_HPP*/