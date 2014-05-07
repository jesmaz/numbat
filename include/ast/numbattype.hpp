#ifndef NUMBATTYPE_HPP
#define NUMBATTYPE_HPP

#include "astbase.hpp"

#include <sstream>


namespace numbat {
namespace parser {


class NumbatType {
	public:
		virtual const bool isArray () const {return false;}
		virtual const bool isFloat () const {return false;}
		virtual const bool isRaw () const {return false;}
		virtual const bool isSigned () const {return false;}
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
	private:
		string iden;
		std::vector <ASTnode> members;
};


}
}

#endif /*NUMBATTYPE_HPP*/