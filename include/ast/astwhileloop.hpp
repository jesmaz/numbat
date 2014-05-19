#ifndef ASTWHILELOOP_HPP
#define ASTWHILELOOP_HPP

#include "astbase.hpp"


namespace numbat {
namespace parser {


class ASTwhileloop : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getBody () const {return body;}
		const ASTnode & getCondition () const {return condition;}
		virtual bool isValid () const {return condition->isValid () and body->isValid ();}
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "while";}
		virtual string toString (const string & indent = "") const {return indent + "while " + condition->toString () + ":\n" + body->toString (indent + '\t');}
		
		ASTwhileloop () {}
		ASTwhileloop (const ASTnode & condition, const ASTnode & body) : condition (condition), body (body) {}
	private:
		ASTnode condition, body;
};


}
}


#endif /*ASTWHILELOOP_HPP*/