#pragma once


#include <ast/node.hpp>
#include <ast/type.hpp>
#include <functional>
#include <map>
#include <set>
#include <utility/array.hpp>


namespace AST {

struct CallingData {
	numbat::lexer::position position;
	const numbat::File * file;
};

struct Function {
	
	public:
		
		const BasicArray <TypePtr> getParams () const {return params;}
		const BasicArray <TypePtr> getRetVals () const {return retVals;}
		const numbat::lexer::position getPos () const {return pos;}
		const std::function <const BasicArray <NodePtr>(const BasicArray <NodePtr>, const CallingData &)> & getNative () const {return nativeFunction;}
		const std::map <std::string, std::string> & getMetaData () const {return metaData;}
		const string & getIden () const {return iden;}
		
	protected:
	private:
		
		friend AST::Context;
		friend parser::Function;
		friend std::pair <string, FuncPtr> APIfunc (const string & iden, const BasicArray <TypePtr> & params, const BasicArray <TypePtr> & retVals, const std::function <const BasicArray <NodePtr>(const BasicArray <NodePtr>, const CallingData &)> &func);
		
		BasicArray <TypePtr> params, retVals;
		BasicArray <NodePtr> defParams, defRets;
		std::map <std::string, std::string> metaData;
		std::map <std::string, size_t> positions;
		
		string iden;
		NodePtr body;
		numbat::lexer::position pos;
		std::function <const BasicArray <NodePtr>(const BasicArray <NodePtr>, const CallingData &)> nativeFunction;
		
};

class Function_Ptr : public Node {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Function_Ptr (numbat::lexer::position pos, const numbat::File * file, const FuncPtr & func) : Node (pos, file), func (func) {}
		
	protected:
	private:
		
		FuncPtr func;
		
};

}
