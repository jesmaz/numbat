#ifndef ASTTUPLE_HPP
#define ASTTUPLE_HPP

#include "../astbase.hpp"

#include <list>


namespace numbat {
namespace parser {


class ASTtuple : public ASTbase {
	VISITABLE
	public:
		const std::list <ASTnode> & getElements () const {return elements;}
		virtual bool isList () const {return true;}
		virtual bool isParsed () const;
		virtual bool isValid () const;
		virtual size_t calculateWeight () const;
		virtual size_t getBitSize () const {return 0;}
		virtual const std::list <ASTnode> getList () const {return elements;}
		virtual const std::list <const ASTbase *> getErrors () const {std::list <const ASTbase *> err; for (const auto & e : elements) {auto r = e->getErrors (); err.splice (err.end (), r);} return err;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const;
		
		ASTtuple (size_t lineNo) : ASTbase (lineNo) {}
		ASTtuple (size_t lineNo, const ASTnode & single) : ASTbase (lineNo), elements (1, single) {}
		ASTtuple (size_t lineNo, const ASTnode & lhs, const ASTnode & rhs) : ASTbase (lineNo), elements () {elements.push_back (lhs); elements.push_back (rhs);}
		ASTtuple (size_t lineNo, const ASTnode & lhs, const std::list <ASTnode> & lst) : ASTbase (lineNo), elements (lst) {elements.push_front (lhs);}
		ASTtuple (size_t lineNo, const std::list <ASTnode> & lst) : ASTbase (lineNo), elements (lst) {}
		ASTtuple (size_t lineNo, const std::list <ASTnode> & lst, const ASTnode & rhs) : ASTbase (lineNo), elements (lst) {elements.push_back (rhs);}
		ASTtuple (size_t lineNo, const std::list <ASTnode> & lhs, const std::list <ASTnode> & rhs) : ASTbase (lineNo), elements (lhs) {elements.insert (elements.end (), rhs.begin (), rhs.end ());}
		ASTtuple (size_t lineNo, const std::vector <ASTnode> & lst) : ASTbase (lineNo), elements (lst.begin (), lst.end ()) {}
	private:
		std::list <ASTnode> elements;
};


}
}


#endif /*ASTTUPLE_HPP*/