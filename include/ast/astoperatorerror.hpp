#ifndef ASTOPERATORERROR_HPP
#define ASTOPERATORERROR_HPP

#include "asterror.hpp"


namespace numbat {
namespace parser {


class ASToperatorError : public ASTerror {
	public:
		virtual bool isParsed () const {return false;}
		virtual bool isValid () const {return false;}
		virtual const std::list <const ASTbase *> getErrors () const {return std::list <const ASTbase *> ({this});}
		virtual size_t calculateWeight () const {return 0;}
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "";}
		
		ASToperatorError (size_t lineNo) : ASTerror (lineNo) {}
		ASToperatorError (size_t lineNo, const string & message) : ASTerror (lineNo, message) {}
	private:
};


}
}


#endif /*ASTOPERATORERROR_HPP*/