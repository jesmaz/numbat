#ifndef NIR_STRUCT
#define NIR_STRUCT


#include "../type.hpp"

#include <map>
#include <vector>


namespace nir {


class Struct : public Type {
	
	public:
		
		
		
	protected:
	private:
		
		std::map <string, Type *> members;
		std::vector <Type *> memberArr;
		
};


};


#endif/*NIR_STRUCT*/