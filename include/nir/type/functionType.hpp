#ifndef NIR_POINTER_TYPE
#define NIR_POINTER_TYPE


#include "../type.hpp"

#include <map>
#include <vector>


namespace nir {


class FunctionType : public Type {
	CONST_VISITABLE
	public:
		
		static string toString (const std::vector <const Type *> & args, text::PrintMode mode=text::PLAIN) {
			if (args.empty ()) {
				return "()";
			}
			string ret = "(";
			ret += args.front ()->toString (mode);
			for (size_t i=1, l=args.size (); i<l; ++i) {
				ret += ", " + args [i]->toString (mode);
			}
			return ret;
		}
		static string toString (const std::vector <const Type *> & args, const std::vector <const Type *> & ret, text::PrintMode mode=text::PLAIN) {
			return toString (args, mode) + " -> " + toString (ret, mode);
		}
		static const FunctionType * get (const std::vector <const Type *> & args, const std::vector <const Type *> & ret) {
			string search = toString (args, ret);
			auto itt = functionTypes.find (search);
			if (itt != functionTypes.end ()) {
				return itt->second;
			}
			return functionTypes [search] = new FunctionType (args, ret);
		}
		
		string toString (text::PrintMode mode=text::PLAIN) {if (this) return strDump (mode); else return "nullptr";}
		
		virtual size_t calculateSize (size_t ptrSize) const {return ptrSize;}
		
	protected:
	private:
		
		FunctionType (const std::vector <const Type *> & args, const std::vector <const Type *> & ret) : args (args), ret (ret) {}
		
		virtual string strDump (text::PrintMode mode) const {return toString (args, ret, mode);}
		
		
		const std::vector <const Type *> args, ret;
		
		static std::map <string, FunctionType *> functionTypes;
		
};


};


#endif/*NIR_POINTER_TYPE*/