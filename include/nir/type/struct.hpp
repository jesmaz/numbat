#ifndef NIR_STRUCT
#define NIR_STRUCT


#include "../type.hpp"

#include <map>
#include <vector>


namespace nir {


class Struct : public Type {
	CONST_VISITABLE
	public:
		
		virtual size_t calculateSize (size_t ptrSize) const;
		
		Struct (const std::map <string, Type *> & members, const std::vector <Type *> & memberArr);
		
	protected:
	private:
		
		std::map <string, Type *> members;
		std::vector <Type *> memberArr;
		
		virtual string strDump (text::PrintMode mode) const;
		
};


};


#endif/*NIR_STRUCT*/