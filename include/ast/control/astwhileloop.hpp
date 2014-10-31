#ifndef ASTWHILELOOP_HPP
#define ASTWHILELOOP_HPP

#include "../astbase.hpp"


namespace numbat {
namespace parser {


class ASTwhileloop : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getBody () const {return body;}
		const ASTnode & getCondition () const {return condition;}
		virtual bool isParsed () const {return condition->isParsed () and body->isParsed ();}
		virtual bool isReturned () const {return body->isReturned ();}
		virtual bool isValid () const {return condition->isValid () and body->isValid ();}
		virtual const std::list <const ASTbase *> getErrors () const {auto c = condition->getErrors (), b = body->getErrors (); c.splice (c.end (), b); return b;}
		virtual size_t calculateWeight () const {return condition->calculateWeight () + body->calculateWeight () + 2;}//The extra 2 is for jumps
		virtual size_t getBitSize () const {return 0;}
		virtual string getIden () const {return "while";}
		virtual string toString (const string & indent = "") const {return indent + "while " + condition->toString () + ":\n" + body->toString (indent + '\t');}
		
		ASTwhileloop (size_t lineNo) : ASTbase (lineNo) {}
		ASTwhileloop (size_t lineNo, const ASTnode & condition, const ASTnode & body) : ASTbase (lineNo), condition (condition), body (body) {}
	private:
		ASTnode condition, body;
};


}
}


#endif /*ASTWHILELOOP_HPP*/