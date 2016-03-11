#ifndef NIR_MODULE
#define NIR_MODULE


#include "../codegen/target.hpp"
#include "forward.hpp"
#include "function.hpp"
#include "type.hpp"


#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace nir {

using std::string;

typedef const string * symbol;

struct Module {
	
	public:
		
		Builder registerFunction (std::vector <Type> args, std::vector <Type> ret, const string & name);
		
		Scope * getGlobalScope ();
		
		symbol findSymbol (const string & iden);
		symbol newSymbol (const string & iden="", bool force=true);
		
		void build ();
		void holdInst (Instruction * inst);
		void registerPrimative (Type::ArithmaticType arith, uint32_t width, const string & name);
		void registerStruct (std::vector <Type> members, const string & name);
		
		Module (codegen::Target * target);
		
		friend Scope;
		
	protected:
	private:
		
		struct Data {
			struct symbCmp {
				bool operator ()(symbol lhs, symbol rhs) {return *lhs < *rhs;}
			};
			std::map <symbol, Function *> functions;
			std::map <symbol, Type *> types;
			std::set <Instruction *> init;
			std::set <symbol, symbCmp> symbols;
			std::set <Scope *> scopes;
			Scope * globalScope=nullptr;
			codegen::Target * target;
			size_t n=0;
		};
		
		std::shared_ptr <Data> data;
		
		friend Scope;
		
	
};

};

#endif/*NIR_MODULE*/