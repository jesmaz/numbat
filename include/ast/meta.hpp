#pragma once


#include <ast/function.hpp>
#include <ast/node.hpp>
#include <ast/type.hpp>


namespace AST {


std::pair <string, FuncPtr> APIfunc (const string & iden, const BasicArray <TypePtr> & params, const BasicArray <TypePtr> & retVals, const std::function <const BasicArray <Literal>(const BasicArray <Literal>, const CallingData &)> & func);


class Reflect : public Node {
	
	public:
		
		static FuncPtr api (const string & iden);
		static void initAPI (const BasicArray <std::pair <string, FuncPtr>> & extraAPIfuncs = {});
		
		NodePtr getMetaTag () const {return metaTag;}
		NodePtr getTarget () const {return target;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		Reflect (numbat::lexer::position pos, const numbat::File * file, const NodePtr & metaTag, const NodePtr & target) : Node (pos, file), metaTag (metaTag), target (target) {}
		
	protected:
	private:
		
		NodePtr metaTag, target;
		static std::map <string, FuncPtr> apiFuncs;
		
};

class ReflectType : public Type {
	
	public:
		
		NodePtr getMetaTag () const {return metaTag;}
		TypePtr getTarget () const {return target;}
		
		void accept (AbstractPass & pass) const {pass.visit (*this);}
		virtual string toString (text::PrintMode mode) const;
		
		static size_t getTypeId (const TypePtr & type);
		static TypePtr get (const NodePtr & metaTag, const TypePtr & target);
		static TypePtr getType (size_t id);
		
	protected:
	private:
		
		ReflectType (const NodePtr & metaTag, const TypePtr & target) : Type (metaTag->getPos (), metaTag->getFile ()), metaTag (metaTag), target (target) {}
		
		NodePtr metaTag;
		TypePtr target;
		static std::map <std::pair <Node*, Type*>, TypePtr> typeMap;
		
};

}
