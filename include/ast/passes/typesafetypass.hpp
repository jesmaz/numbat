#pragma once


#include <ast/passes/analysis.hpp>
#include <set>


namespace AST {


class TypeSafetyPass : public AnalysisPass {
	
	public:
		
		virtual void visit (const Call_1 & node);
		virtual void visit (const Call_2 & node);
		virtual void visit (const Call_n & node);
		virtual void visit (const Basic_Operation & node);
		
	protected:
	private:
		
		
		
};


}

