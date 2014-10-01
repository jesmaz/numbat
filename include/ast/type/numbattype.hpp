#ifndef NUMBATTYPE_HPP
#define NUMBATTYPE_HPP

#include "../astbase.hpp"
#include "../functiondecleration.hpp"

#include <map>
#include <set>
#include <sstream>


namespace numbat {
namespace parser {


class NumbatPointerType;
class NumbatScope;

class NumbatType {
	public:
		const bool hasTag (const string & tag) const {return metaTags.count (tag);}
		const FunctionDecleration * getMalloc () const {return mallocFunc;}
		const FunctionDecleration * getFree () const {return freeFunc;}
		void setMalloc (FunctionDecleration * malloc) const {mallocFunc = malloc;}
		void setFree (FunctionDecleration * free) const {freeFunc = free;}
		const std::vector <shared_ptr <FunctionDecleration>> & getConstructors () const {return constructors;}
		const std::vector <FunctionDecleration *> getMethods (const string & iden) const {std::vector <FunctionDecleration *> m; for (auto e = methods.lower_bound (iden), end = methods.upper_bound (iden); e != end; ++e) {m.push_back (e->second);} return m;}
		void addConstructor (const shared_ptr <FunctionDecleration> & func) {constructors.push_back (func);}
		void addMethod (const string & iden, FunctionDecleration * func) {methods.insert (std::make_pair (iden, func));}
		virtual const ASTnode getArrayType () const {return nullptr;}
		virtual const bool isArray () const {return false;}
		virtual const bool isFloat () const {return false;}
		virtual const bool isRaw () const {return false;}
		virtual const bool isSigned () const {return false;}
		virtual const bool isValid () const;
		virtual const size_t getBitSize () const;
		virtual const ssize_t getLengthIndex () const {return -1;}
		virtual const std::vector <ASTnode> & getMembers () const {return members;}
		virtual const string & getIden () const {return iden;}
		virtual string convertDataToString (const void * data) const {size_t offset=0; return convertDataToString (data, offset);}
		virtual string convertDataToString (const void * data, size_t & offset) const;
		virtual string toString (const string & indent = "") const;
		
		int findMember (const std::string & iden) const;
		void buildData (std::vector <ASTnode> data) {members = data;}
		
		friend NumbatType * getArrayType (NumbatScope * scope, const NumbatType * type, size_t dimentions);
		
		NumbatType () {}
		NumbatType (string iden, FunctionDecleration * mallocFunc, FunctionDecleration *  freeFunc) : iden (iden), mallocFunc (mallocFunc), freeFunc (freeFunc) {}
		NumbatType (string iden, const std::set <string> & meta, FunctionDecleration * mallocFunc, FunctionDecleration *  freeFunc) : iden (iden), metaTags (meta), mallocFunc (mallocFunc), freeFunc (freeFunc) {}
	private:
		string iden;
		std::set <string> metaTags;
		std::vector <ASTnode> members;
		std::vector <shared_ptr <FunctionDecleration>> constructors;
		std::multimap <string, FunctionDecleration *> methods;
		mutable FunctionDecleration * mallocFunc, * freeFunc;
		mutable std::vector <NumbatPointerType *> arrayType;
};


}
}

#endif /*NUMBATTYPE_HPP*/