#pragma once


#include <ast/node.hpp>
#include <ast/type.hpp>
#include <functional>
#include <map>
#include <set>
#include <utility/array.hpp>


namespace AST {

struct Function {
	
	public:
		
		const BasicArray <TypePtr> getParams () const {return params;}
		const BasicArray <TypePtr> getRetVals () const {return retVals;}
		const numbat::lexer::position getPos () const {return pos;}
		const string & getIden () const {return iden;}
		
	protected:
	private:
		
		friend AST::Context;
		friend parser::Function;
		friend std::pair <string, FuncPtr> APIfunc (const string & iden, const BasicArray <TypePtr> & params, const BasicArray <TypePtr> & retVals, const std::function <void(void*, void*)> &func);
		
		BasicArray <TypePtr> params, retVals;
		std::map <std::string, size_t> positions;
		
		string iden;
		NodePtr body;
		numbat::lexer::position pos;
		std::function <void(void*, void*)> nativeFunction;
		
};

class Function_Ptr : public Node {
	
	public:
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Function_Ptr (numbat::lexer::position pos, const FuncPtr & func) : Node (pos), func (func) {}
		
	protected:
	private:
		
		FuncPtr func;
		
};

}
