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
 

namespace nir {

using std::string;

typedef const string * symbol;

struct Module {
	
	public:
		
		bool validate () const;
		
		Builder registerFunction (DynArray <Type> args, DynArray <Type> ret, const string & name);
		
		const Block * getEntry () const;
		const std::map <symbol, Function *> getFunctions () const {return data->functions;}
		
		Scope * createRootScope ();
		Scope * getGlobalScope ();
		
		symbol findSymbol (const string & iden);
		symbol newSymbol (const string & iden="", bool force=true);
		
		void build (codegen::Target * target);
		void holdInst (Instruction * inst);
		void registerPrimitive (Type::ArithmaticType arith, uint32_t width, const string & name);
		void registerStruct (DynArray <Type> members, const string & name);
		
		Module ();
		
		friend Scope;
		
	protected:
	private:
		
		struct Data {
			struct symbCmp {
				bool operator ()(symbol lhs, symbol rhs) {return *lhs < *rhs;}
			};
			std::map <symbol, Function *> functions;
			std::map <symbol, const Type *> types;
			std::set <Instruction *> init;
			std::set <symbol, symbCmp> symbols;
			std::set <Scope *> scopes;
			Scope * globalScope=nullptr;
			size_t n=0;
			~Data ();
		};
		
		std::shared_ptr <Data> data;
		
		friend Scope;
		
	
};

};

#endif/*NIR_MODULE*/
