#pragma once


#include <codegen/genericPass.hpp>
#include <codegen/passProcession.hpp>
#include <memory>
#include <set>


namespace codegen {
namespace analysis {


class CallGraph : public GenericPass {
		
	public:
		
		const std::multiset <std::pair <const nir::Function *, const nir::Function *>> & getEdges () const {return edges;}
		
		virtual void run (const nir::Module * module);
		
		virtual ~CallGraph () {}
		
	protected:
	private:
		
		friend PassProcession;
		
		CallGraph (PassProcession * PassProcession);
		
		std::multiset <std::pair <const nir::Function *, const nir::Function *>> edges;
		
};


}
}