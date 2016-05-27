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
		
		Struct (const std::map <string, const Type *> & members, const std::vector <const Type *> & memberArr);
		
	protected:
	private:
		
		std::map <string, const Type *> members;
		std::vector <const Type *> memberArr;
		
		virtual string strDump (text::PrintMode mode) const;
		
};


};


#endif/*NIR_STRUCT*/