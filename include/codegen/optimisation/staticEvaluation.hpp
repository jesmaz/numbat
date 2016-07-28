#pragma once


#include <codegen/genericPass.hpp>
#include <codegen/passProcession.hpp>
#include <memory>
#include <set>


namespace codegen {
namespace optimisation {


class StaticEvaluator : public GenericPass {
		
	public:
		
		virtual ~StaticEvaluator () {}
		
	protected:
	private:
		
		friend PassProcession;
		
		StaticEvaluator (PassProcession * passProcession, nir::Module * module);
		
};


}
}