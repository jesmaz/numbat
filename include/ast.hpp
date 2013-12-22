#ifndef ABSTRACT_SYNTAX_TREE_HPP
#define ABSTRACT_SYNTAX_TREE_HPP

#include "token.hpp"
#include "lexer.hpp"
#include "visitor.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <unordered_set>
#include <vector>

using std::shared_ptr;
using std::string;
using std::unique_ptr;

namespace numbat {
namespace parser {


const size_t pointerSize = 8;

typedef lexer::tkstring::const_iterator tkitt;
class ASTfunctionPointer;
class NumbatType;


class ASTbase : public visitor::BaseVisitable {
	public:
		virtual bool isAlias () const {return false;}
		virtual bool isCallable () const {return false;}
		virtual bool isConst () const {return false;}
		virtual bool isList () const {return false;}
		virtual bool isRaw () const {return false;}
		virtual bool isValid () const=0;
		virtual shared_ptr <NumbatType> getType () const {return nullptr;}
		virtual size_t calculateWeight () const {return 0;};
		virtual size_t getSize () const=0;
		virtual string getIden () const=0;
		virtual string toString (const string & indent = "") const=0;
		ASTbase () {}
};

typedef shared_ptr <ASTbase> ASTnode;

class ASTlistInterface {
	public:
		virtual const std::vector <ASTnode> getElements () const=0;
};

struct FunctionDecleration {
	public:
		const ASTnode & getBody () const {return body;}
		const std::vector <ASTnode> & getArgs () const {return parameters;}
		const std::vector <ASTnode> & getType () const {return type;}
		const string getIden () const {return iden;}
		string toString (const string & indent = "") {
			string str = "(";
			for (ASTnode n : parameters) {
				str += n->toString (indent) + ", ";
			}
			str += "): {";
			for (ASTnode n : type) {
				str += n->toString (indent) + ", ";
			}
			str += "}";
			if (body)
				return str + "\n" + body->toString (indent + "\t") + "\n";
			else
				return str + "\n";
		}
		void assignBody (ASTnode b) {body = b;}
		
		FunctionDecleration () {}
		FunctionDecleration (const string & iden, const std::vector <ASTnode> & args, const std::vector <ASTnode> & type) : iden (iden), parameters (args), type (type) {}
	private:
		string iden;
		std::vector <ASTnode> parameters, type;
		ASTnode body;
};

class NumbatType {
	public:
		virtual const bool isFloat () const {return false;}
		virtual const bool isRaw () const {return false;}
		virtual const bool isSigned () const {return false;}
		virtual const size_t getSize () const {size_t size=0; for (const ASTnode & mem : members) size += mem->getSize (); return size;}
		virtual const std::vector <ASTnode> & getMembers () const {return members;}
		virtual const string & getIden () const {return iden;}
		virtual string convertDataToString (const void * data) const {size_t offset=0; return convertDataToString (data, offset);}
		virtual string convertDataToString (const void * data, size_t & offset) const {
			string ret = "{";
			for (const ASTnode & member : members) {
				ret += member->getType ()->convertDataToString (data, offset) + ", ";
			}
			return ret + "}";
		}
		virtual string toString (const string & indent = "") const {
			std::ostringstream ss;
			ss << "'" << iden << "' {";
			for (ASTnode n : members) ss << n->toString ("") << ", ";
			ss << "}";
			return ss.str ();
		}
		
		void buildData (std::vector <ASTnode> data) {members = data;}
		
		NumbatType () {}
		NumbatType (string iden) : iden (iden) {}
	private:
		string iden;
		std::vector <ASTnode> members;
};

class NumbatRawType : public NumbatType {
	public:
		enum Type {FLOAT, SIGNED, UNSIGNED};
		virtual const bool isFloat () const {return type == FLOAT;}
		virtual const bool isRaw () const {return true;}
		virtual const bool isSigned () const {return type == SIGNED;}
		virtual const size_t getSize () const {return size;}
		virtual const Type getRawType () const {return type;}
		virtual string convertDataToString (const void * data) const {size_t offset=0; return convertDataToString (data, offset);}
		virtual string convertDataToString (const void * data, size_t & offset) const {
			std::ostringstream os;
			if (size == 64) {
				os << *reinterpret_cast <const uint64_t *> (data);
				offset += 8;
			} else if (size == 32) {
				os << *reinterpret_cast <const uint32_t *> (data);
				offset += 4;
			} else if (size == 16) {
				os << *reinterpret_cast <const uint16_t *> (data);
				offset += 2;
			} else if (size == 8) {
				os << *reinterpret_cast <const uint8_t *> (data);
				offset += 1;
			} else {
				os << "unknown";
			}
			return os.str ();
		}
		virtual string toString (const string & indent = "") const {
			std::ostringstream ss;
			ss << "raw ";
			switch (type) {
				case FLOAT:
					ss << "@fint ";
					break;
				case SIGNED:
					ss << "@sint ";
					break;
				case UNSIGNED:
					ss << "@uint ";
					break;
			}
			ss << size;
			return ss.str ();
		}
		
		NumbatRawType () {}
		NumbatRawType (string iden, size_t size, Type type) : NumbatType (iden), size (size), type (type) {} // for raw data
	private:
		size_t size=0;
		Type type;
};

struct NumbatVariable {
	public:
		bool isAlias () const {return type->isAlias ();}
		bool isConst () const {return type->isConst ();}
		shared_ptr <NumbatType> getType () const {return type->getType ();}
		const size_t getSize () const {return type->getSize ();}
		const string & getIden () const {return iden;}
		
		NumbatVariable () {}
		NumbatVariable (const ASTnode & type, const string & iden) : type (type), iden (iden) {}
	private:
		ASTnode type;
		string iden;
};

struct OperatorDecleration {
	public:
		enum class TYPE {array, binary, index_call, ternary, unarypostfix, unaryprefix, INVALID};
		
		struct OperatorMatch {
			const static bool treeOrder (const OperatorMatch & lhs, const OperatorMatch & rhs) {
				if (lhs.opp->precidance == rhs.opp->precidance) {
					if (lhs.opp->ltr) {
						return lhs.ptr < rhs.ptr;
					} else {
						return lhs.ptr > rhs.ptr;
					}
				} else {
					return lhs.opp->precidance > rhs.opp->precidance;
				}
			}
			const static bool parseOrder (const OperatorMatch & lhs, const OperatorMatch & rhs) {return lhs.ptr < rhs.ptr;}
			std::shared_ptr <OperatorDecleration> opp;
			tkitt ptr;
		};
		
		const bool isLtr () const {return ltr;}
		const bool operator < (const OperatorDecleration & rhs) const {if (precidance == rhs.precidance) return pattern < rhs.pattern; return precidance < rhs.precidance;}
		const std::vector <string> & getSymbols () const {return symbols;}
		const string & getPattern () const {return pattern;}
		const TYPE getType () const {return type;}
		
		static TYPE calculateOperatorType (const string & pattern) {
			TYPE type = TYPE::INVALID;
			string simplePtn;
			for (char c : pattern) {
				if (c == ' ') {
					if (!(simplePtn.size () and simplePtn.back () == ' '))
						simplePtn += c;
				} else if (!(simplePtn.size () and simplePtn.back () == '*')) {
					simplePtn += '*';
				}
			}
			//std::cout << "'" << simplePtn << "'    '" << pattern << "'" << std::endl;
			switch (simplePtn.length ()) {
				case 2:
					if (simplePtn == " *") type = TYPE::unarypostfix;
					else if (simplePtn == "* ") type = TYPE::unaryprefix;
					break;
				case 3:
					if (simplePtn == " * ") type = TYPE::binary;
					else if (simplePtn == "* *") type = TYPE::array;
					break;
				case 4:
					if (simplePtn == " * *") type = TYPE::index_call;
					break;
				case 5:
					if (simplePtn == " * * ") type = TYPE::ternary;
			}
			return type;
		}
		
		OperatorDecleration ();
		OperatorDecleration (int precidance, bool ltr, const string & pattern) : precidance (precidance), ltr (ltr), pattern (pattern), type (calculateOperatorType (pattern)) {
			string buffer = " ";
			for (char c : pattern) {
				if (buffer == "") {
					buffer += c;
				} else {
					if (c == ' ') {
						if (buffer != " ") {
							symbols.push_back (buffer);
							buffer = " ";
						}
					} else {
						if (buffer == " ") {
							symbols.push_back (buffer);
							buffer = c;
						}
					}
				}
			}
			if (buffer != "")
				symbols.push_back (buffer);
		}
		
	private:
		int precidance;
		bool ltr;
		std::vector <string> symbols;
		string pattern; // space for identifier eg: " * ", " [ ]", " ? : "
		TYPE type;
};

class ASTcallable : public ASTbase {
	public:
		virtual bool isCallable () const {return true;}
		virtual const shared_ptr <FunctionDecleration> & getFunction () const=0;
};



class ASTbody : public ASTbase {
	VISITABLE
	public:
		const std::vector <ASTnode> & getBody () const {return body;}
		virtual bool isValid () const {bool b=true; for (const ASTnode & n : body) b &= n->isValid (); return b;}
		virtual size_t getSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {
			string str;
			string inden = indent + '\t';
			for (ASTnode n : body) {
				if (n)
					str += n->toString (inden) + '\n';
				else
					str += "null\n";
			}
			return str;
		}
		
		ASTbody () {}
		ASTbody (std::vector <ASTnode> body) : body (body) {}
	private:
		std::vector <ASTnode> body;
};

class ASTcall : public ASTcallable {
	VISITABLE
	public:
		const shared_ptr <ASTcallable> & getCallee () const {return callee;}
		const std::vector <ASTnode> & getArgs () const {return args;}
		virtual bool isValid () const {bool b=callee->isValid (); for (const ASTnode & n : args) b &= n->isValid (); return b;}
		virtual const shared_ptr <FunctionDecleration> & getFunction () const {return callee->getFunction ();}
		virtual size_t getSize () const {return 0;}
		virtual string getIden () const {return callee->getIden ();}
		virtual string toString (const string & indent = "") const {
			string ret = indent + callee->toString () + "(";
			for (const ASTnode & arg : args) {
				ret += arg->toString () + ", ";
			}
			return ret + ")";
		}
		
		ASTcall () {}
		ASTcall (const shared_ptr <ASTcallable> & callee, const std::vector <ASTnode> & args) : callee (callee), args (args) {}
	private:
		shared_ptr <ASTcallable> callee;
		std::vector <ASTnode> args;
};

class ASTcallerror : public ASTcallable {
	public:
		virtual bool isValid () const {return false;}
		virtual const shared_ptr <FunctionDecleration> & getFunction () const {return func;}
		virtual size_t getSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + message;}
		
		ASTcallerror () {}
		ASTcallerror (const string & message) : message (message) {}
	private:
		string message;
		shared_ptr <FunctionDecleration> func = nullptr;
};

class ASTcallindex : public ASTbase {
	VISITABLE
	public:
		const shared_ptr <ASTcallable> & getCall () const {return call;}
		const size_t getIndex () const {return index;}
		virtual bool isAlias () const {return ref->isAlias ();}
		virtual bool isConst () const {return ref->isConst ();}
		virtual bool isValid () const {return call->isValid ();}
		virtual shared_ptr <NumbatType> getType () const {return ref->getType ();}
		virtual size_t getSize () const {return ref->getSize ();}
		virtual string getIden () const {return ref->getIden ();}
		virtual string toString (const string & indent = "") const {return !index ? call->toString (indent) : ref->toString (indent);}
		
		ASTcallindex () {}
		ASTcallindex (const shared_ptr <ASTcallable> & call, const size_t index) : ref (call->getFunction ()->getType () [index]), call (call), index (index) {}
	private:
		ASTnode ref;
		shared_ptr <ASTcallable> call;
		size_t index;
};

template <typename T>
class ASTconstant : public ASTbase {
	public:
		virtual bool isAlias () const {return type->isAlias ();}
		virtual bool isConst () const {return true;}
		virtual bool isValid () const {return type->isValid ();}
		virtual const T & getValue () const=0;
		virtual shared_ptr <NumbatType> getType () const {return type->getType ();}
		virtual size_t getSize () const {return type->getSize ();}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + type->getIden ();}
		
		ASTconstant () {}
		ASTconstant (const ASTnode & type) : type (type) {}
	private:
		ASTnode type;
};

class ASTconstantInt : public ASTconstant <size_t> {
	VISITABLE
	public:
		const size_t & getValue () const {return val;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return ASTconstant::toString (indent) + " 0";}
		
		ASTconstantInt () {}
		ASTconstantInt (const ASTnode & type, const size_t val) : ASTconstant (type), val (val) {}
	private:
		size_t val;
};

class ASTerror : public ASTbase {
	public:
		virtual bool isValid () const {return false;}
		virtual size_t getSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + message;}
		
		ASTerror () {}
		ASTerror (const string & message) : message (message) {}
	private:
		string message;
};

class ASTfunctionPointer : public ASTcallable {
	VISITABLE
	public:
		virtual bool isValid () const {return true;}
		virtual const shared_ptr <FunctionDecleration> & getFunction () const {return function;}
		virtual size_t getSize () const {return pointerSize;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + "'" + function->getIden () + "'";}
		
		ASTfunctionPointer () {}
		ASTfunctionPointer (shared_ptr <FunctionDecleration> function) : function (function) {}
		
	private:
		shared_ptr <FunctionDecleration> function;
};

class ASTidentifier : public ASTbase {
	VISITABLE
	public:
		virtual bool isValid () const {return true;}
		virtual size_t getSize () const {return 0;}
		virtual string getIden () const {return iden;}
		virtual string toString (const string & indent = "") const {return indent + iden;}
		
		ASTidentifier () {}
		ASTidentifier (const string & iden) : iden (iden) {}
	private:
		string iden;
};

class ASTnumbatInstr : public ASTbase {
	VISITABLE
	public:
		const string & getInstr () const {return instr;}
		const std::vector <ASTnode> getArgs () const {return args;}
		virtual bool isValid () const {bool b=true; for (const ASTnode & n : args) b &= n->isValid (); return b;}
		virtual size_t getSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {
			string ret = indent + "asm 'numbat' : '" + instr + "' (";
			for (const ASTnode & arg : args) {
				ret += arg->toString () + ", ";
			}
			return ret + ")";
		}
		
		ASTnumbatInstr () {}
		ASTnumbatInstr (const string & instr, const std::vector <ASTnode> & args) : instr (instr), args (args)/*, args (args)*/ {}
	private:
		string instr;
		std::vector <ASTnode> args;
};

class ASTparamater : public ASTbase {
	VISITABLE
	public:
		const std::shared_ptr <NumbatVariable> & getVariable () {return variable;}
		virtual bool isAlias () const {return variable->isAlias ();}
		virtual bool isConst () const {return variable->isConst ();}
		virtual bool isValid () const {return variable != nullptr;}
		virtual shared_ptr <NumbatType> getType () const {return variable->getType ();}
		virtual size_t getSize () const {return variable->getSize ();}
		virtual string getIden () const {return variable->getIden ();}
		virtual string toString (const string & indent = "") const {
			return variable->getType ()->toString (indent) + " " + variable->getIden ();
		}
		
		ASTparamater () {}
		ASTparamater (const std::shared_ptr <NumbatVariable> & variable) : variable (variable) {}
	private:
		std::shared_ptr <NumbatVariable> variable;
};

class ASTrawdata : public ASTbase {
	VISITABLE
	public:
		virtual bool isAlias () const {return alias;}
		virtual bool isConst () const {return constType;}
		virtual bool isRaw () const {return true;}
		virtual bool isValid () const {return true;}
		virtual shared_ptr <NumbatType> getType () const {return type;}
		virtual size_t getSize () const {return type->getSize ();}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {
			return indent + type->getIden ();
		}
		
		ASTrawdata () {}
		ASTrawdata (const bool alias, const bool constType, const shared_ptr <NumbatType> & type, const std::set <string> & metaTags) : alias (alias), constType (constType), type (type), metaTags (metaTags) {}
	private:
		bool alias, constType;
		shared_ptr <NumbatType> type;
		std::set <string> metaTags;
};

class ASTreturn : public ASTbase {
	VISITABLE
	public:
		const ASTnode & getExpr () const {return expr;}
		virtual bool isAlias () const {return expr->isAlias ();}
		virtual bool isConst () const {return expr->isConst ();}
		virtual bool isValid () const {return expr->isValid ();}
		virtual shared_ptr <NumbatType> getType () const {return expr->getType ();}
		virtual size_t getSize () const {return expr->getSize ();}
		virtual string getIden () const {return expr->getIden ();}
		virtual string toString (const string & indent = "") const {return indent + "return " + expr->toString ();}
		
		ASTreturn () {}
		ASTreturn (const ASTnode expr) : expr (expr) {}
	private:
		ASTnode expr;
};

class ASTreturnvoid : public ASTbase {
	VISITABLE
	public:
		virtual bool isValid () const {return true;}
		virtual size_t getSize () const {return 0;}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {return indent + "return void";}
		
		ASTreturnvoid () {}
	private:
};

class ASTtype : public ASTbase {
	VISITABLE
	public:
		virtual bool isAlias () const {return alias;}
		virtual bool isConst () const {return constType;}
		virtual bool isValid () const {return true;}
		virtual shared_ptr <NumbatType> getType () const {return type;}
		virtual size_t getSize () const {return type->getSize ();}
		virtual string getIden () const {return "";}
		virtual string toString (const string & indent = "") const {
			return indent + type->getIden ();
		}
		
		ASTtype () {}
		ASTtype (const bool alias, const bool constType, const shared_ptr <NumbatType> & type) : alias (alias), constType (constType), type (type) {}
	private:
		bool alias, constType;
		shared_ptr <NumbatType> type;
};

class ASTvariable : public ASTbase {
	VISITABLE
	public:
		const std::shared_ptr <NumbatVariable> & getVariable () {return variable;}
		virtual bool isAlias () const {return variable->isAlias ();}
		virtual bool isConst () const {return variable->isConst ();}
		virtual bool isValid () const {return true;}
		virtual shared_ptr <NumbatType> getType () const {return variable->getType ();}
		virtual size_t getSize () const {return variable->getSize ();}
		virtual string getIden () const {return variable->getIden ();}
		virtual string toString (const string & indent = "") const {return indent + variable->getIden ();}
		
		ASTvariable () {}
		ASTvariable (const std::shared_ptr <NumbatVariable> & variable) : variable (variable) {}
	private:
		std::shared_ptr <NumbatVariable> variable;
};


struct AbstractSyntaxTree {
	
	public:
		const bool buildSuccessfull () const {return !buildFail;}
		const std::multimap <string, shared_ptr <FunctionDecleration>> getFunctions () const {return functions;}
		string toString (const string & indent = "") const {
			string str="";
			for (auto & itt : types) {
				str += itt.first + " : " + itt.second->toString (indent) + "\n";
			}
			str += '\n';
			for (auto & itt : functions) {
				str += indent + "'" + itt.first + "' " + itt.second->toString (indent);
			}
			return str;
		}
		
		AbstractSyntaxTree () {}
		AbstractSyntaxTree (tkitt beg, tkitt end);
		
	private:
		
		ASTnode createCallNode (const shared_ptr <ASTcallable> & callee, const std::vector <ASTnode> & args);
		ASTnode createStaticCast (const ASTnode & arg, const ASTnode & type, int maxDepth=1);
		ASTnode parseAssembly (const string & type, const string & code);
		ASTnode parseBody (tkitt end); // TODO: needs template info parem
		ASTnode parseExpression (tkitt end);
		ASTnode parseExpression (std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end, const std::vector <ASTnode> * args = nullptr);
		ASTnode parseOperator (const OperatorDecleration & opp, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt softEnd, tkitt end);
		ASTnode parseParameter (tkitt end);
		ASTnode parsePrimaryExpression (tkitt end) {return parsePrimaryExpression (end, nullptr);}
		ASTnode parsePrimaryExpression (tkitt end, const std::vector <ASTnode> * args);
		ASTnode parseStatment (tkitt end);
		ASTnode parseType (tkitt end);
		
		bool eatSemicolon (tkitt end) {if (itt->type == lexer::TOKEN::semicolon) return nextToken (end); return false;}
		bool flushLine (tkitt end);
		bool nextToken (tkitt end);
		bool parseFunctionPrototype (std::vector <ASTnode> & args, string & iden, tkitt end);
		
		FunctionDecleration * parseFunctionDecleration (tkitt end);
		
		OperatorDecleration::OperatorMatch matchFromRight (const string & patern, tkitt beg, tkitt end);
		
		shared_ptr <ASTcallable> findFunction (const string & iden, const std::vector <ASTnode> & args, int convert=1);
		
		shared_ptr <NumbatType> generateRawType (const string & key, size_t size, const std::set <string> & metaArgs);
		
		size_t parseIntLiteral (tkitt end);
		
		std::list <OperatorDecleration::OperatorMatch> generateOperatorMatches (tkitt end);
		
		std::set <string> parseMetaTags (tkitt end) {std::set <string> ret; while (itt->type == lexer::TOKEN::atsym and nextToken (end)) {ret.insert (itt->iden); nextToken (end);} return ret;}
		
		std::vector <ASTnode> createCallIndexList (const shared_ptr <ASTcallable> & call);
		std::vector <ASTnode> createStaticCast (const std::vector <ASTnode> & args, const std::vector <ASTnode> & types, int maxDepth=1);
		std::vector <ASTnode> parseArgs (ASTnode (AbstractSyntaxTree::*arg)(tkitt), tkitt end);
		std::vector <ASTnode> parseTemplateArgs (tkitt end);
		
		string parseStructDecleration (tkitt end);
		
		tkitt findIndent (const int level, tkitt end);
		tkitt findLastToken (const string & token, tkitt end) {return findLastToken (token, itt, end);}
		tkitt findLastToken (const string & token, tkitt beg, tkitt end);
		tkitt findToken (const string & token, tkitt end) {return findToken (token, itt, end);}
		tkitt findToken (const string & token, tkitt beg, tkitt end);
		
		void error (const string & message, tkitt end) {printError (message); flushLine (end);}
		void printError (const string & message) {buildFail = true; std::cerr << "error on line " << line << ": " << message << '\n';}
		void parseOperatorDecleration (tkitt end);
		void parseTypeDef (tkitt end);
		
		
		
		
		bool buildFail = false;
		
		int indentLevel=0;
		
		tkitt itt;
		
		size_t line = 0;
		
		std::map <string, shared_ptr <NumbatType>> types;
		std::map <string, shared_ptr <NumbatVariable>> variables;
		std::map <string, shared_ptr <OperatorDecleration>> operators;
		
		std::multimap <string, shared_ptr <FunctionDecleration>> functions;
		std::multimap <string, shared_ptr <OperatorDecleration>> operatorsByFirstToken;
		
		std::set <shared_ptr <OperatorDecleration>, std::greater <shared_ptr <OperatorDecleration>>> precidenceOrderedOperators;
		
		std::unordered_set <string> parenOpperators, oppTokens, ternaryStart;
		
};


std::vector <ASTnode> expandNodes (const std::vector <ASTnode> & args);

void splitListAboutTkn (std::list <OperatorDecleration::OperatorMatch> & lhs, std::list <OperatorDecleration::OperatorMatch> & source, tkitt);


};
};

#endif /* ABSTRACT_SYNTAX_TREE_HPP */
