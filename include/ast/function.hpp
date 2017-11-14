#pragma once


#include <ast/node.hpp>
#include <ast/type.hpp>
#include <functional>
#include <map>
#include <set>
#include <utility/array.hpp>
#include <utility/literals.hpp>


namespace AST {

struct CallingData {
	numbat::lexer::position position;
	const numbat::File * file;
};

struct Function {
	
	public:
		
		DynArray <FuncPtr> & getDirectlyCalled () {return directlyCalled;}
		const DynArray <FuncPtr> & getDirectlyCalled () const {return directlyCalled;}
		const BasicArray <TypePtr> getParams () const {return params;}
		const BasicArray <TypePtr> getRetVals () const {return retVals;}
		const NodePtr getBody () const {return body;}
		const numbat::lexer::position getPos () const {return pos;}
		const std::function <const BasicArray <Literal> (const BasicArray <Literal>, const CallingData &)> & getNative () const {return nativeFunction;}
		std::map <std::string, std::string> & getMetaData () {return metaData;}
		const std::map <std::string, std::string> & getMetaData () const {return metaData;}
		const string & getIden () const {return iden;}
		void replaceBody (NodePtr b) {body = b;}
		
	protected:
	private:
		
		friend AST::Context;
		friend parser::Function;
		friend std::pair <string, FuncPtr> APIfunc (const string & iden, const BasicArray <TypePtr> & params, const BasicArray <TypePtr> & retVals, const std::function <const BasicArray <Literal>(const BasicArray <Literal>, const CallingData &)> &func);
		
		BasicArray <TypePtr> params, retVals;
		BasicArray <NodePtr> defParams, defRets;
		std::map <std::string, std::string> metaData;
		std::map <std::string, size_t> positions;
		DynArray <FuncPtr> directlyCalled;
		
		string iden;
		NodePtr body;
		numbat::lexer::position pos;
		std::function <const BasicArray <Literal>(const BasicArray <Literal>, const CallingData &)> nativeFunction;
		
};

class Function_Ptr : public Node {
	
	public:
		
		const FuncPtr & getFunc () const {return func;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Function_Ptr (numbat::lexer::position pos, const numbat::File * file, const FuncPtr & func) : Node (pos, file), func (func) {}
		
	protected:
	private:
		
		FuncPtr func;
		
};

}
