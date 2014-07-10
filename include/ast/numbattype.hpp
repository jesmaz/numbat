#ifndef NUMBATTYPE_HPP
#define NUMBATTYPE_HPP

#include "astbase.hpp"

#include <set>
#include <sstream>


namespace numbat {
namespace parser {


class NumbatType {
	public:
		const bool hasTag (const string & tag) const {return metaTags.count (tag);}
		virtual const bool isArray () const {return false;}
		virtual const bool isFloat () const {return false;}
		virtual const bool isRaw () const {return false;}
		virtual const bool isSigned () const {return false;}
		virtual const bool isValid () const;
		virtual const size_t getBitSize () const;
		virtual const std::vector <ASTnode> & getMembers () const {return members;}
		virtual const string & getIden () const {return iden;}
		virtual string convertDataToString (const void * data) const {size_t offset=0; return convertDataToString (data, offset);}
		virtual string convertDataToString (const void * data, size_t & offset) const;
		virtual string toString (const string & indent = "") const;
		
		int findMember (const std::string & iden);
		void buildData (std::vector <ASTnode> data) {members = data;}
		
		NumbatType () {}
		NumbatType (string iden) : iden (iden) {}
		NumbatType (string iden, const std::set <string> & meta) : iden (iden), metaTags (meta) {}
	private:
		string iden;
		std::set <string> metaTags;
		std::vector <ASTnode> members;
};


}
}

#endif /*NUMBATTYPE_HPP*/