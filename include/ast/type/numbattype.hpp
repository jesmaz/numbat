#ifndef NUMBATTYPE_HPP
#define NUMBATTYPE_HPP

#include "../astbase.hpp"
#include "../functiondecleration.hpp"

#include <set>
#include <sstream>


namespace numbat {
namespace parser {


class NumbatType {
	public:
		const bool hasTag (const string & tag) const {return metaTags.count (tag);}
		const std::vector <shared_ptr <FunctionDecleration>> & getConstructors () const {return constructors;}
		void addConstructor (const shared_ptr <FunctionDecleration> & func) {constructors.push_back (func);}
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
		
		NumbatType () {}
		NumbatType (string iden) : iden (iden) {}
		NumbatType (string iden, const std::set <string> & meta) : iden (iden), metaTags (meta) {}
	private:
		string iden;
		std::set <string> metaTags;
		std::vector <ASTnode> members;
		std::vector <shared_ptr <FunctionDecleration>> constructors;
};


}
}

#endif /*NUMBATTYPE_HPP*/