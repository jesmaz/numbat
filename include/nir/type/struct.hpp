#ifndef NIR_STRUCT
#define NIR_STRUCT


#include "../type.hpp"

#include <map>
 


namespace nir {


class Struct : public Type {
	CONST_VISITABLE
	public:
		
		virtual const Parameter * getParam (const string & str) const;
		virtual size_t calculateSize (size_t ptrSize) const;
		virtual ssize_t calculateOffset (size_t ptrSize, const string & iden) const;
		virtual ssize_t calculateIndex (const Parameter * param) const;
		
		const BasicArray <const Parameter *> & getMemberArr () const {return memberArr;}
		
		void populate (const BasicArray <const Parameter *> & memberArr);
		
		Struct () {}
		
	protected:
	private:
		
		std::map <string, const Parameter *> members;
		BasicArray <const Parameter *> memberArr;
		
		virtual string strDump (text::PrintMode mode) const;
		
};


};


#endif/*NIR_STRUCT*/
