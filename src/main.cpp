#include <ast/passes/nir.hpp>
#include <ast/sequence.hpp>
#include <codegen/target.hpp>
#include <codegen/llvm.hpp>
#include <file.hpp>
#include <nir/module.hpp>
#include <nir/scope.hpp>
#include <utility/config.hpp>
#include <utility/report.hpp>


int numbatMain (const Config & cfg) {
	
	codegen::Target * target = codegen::Target::find ("llvm");
	
	nir::Module nirModule;
	AST::NirPass nirPass (nirModule.getGlobalScope ());
	{
		DynArray <AST::NodePtr> seq;
		for (const string & file : cfg.files) {
			auto * f = numbat::File::compile (file);
			seq.push_back (f->getAST ());
		}
		seq.push_back (numbat::File::builtIn ()->getAST ());
		auto program = std::make_shared <AST::Sequence> (numbat::lexer::position {0, 0}, numbat::File::builtIn (), seq);
		nirPass (AST::transform (program));
	}
	
	if (report::compilationFailed ()) {
		report::printLogs ();
		return 1;
	}
	
	if (not nirModule.validate ()) {
		report::printLogs ();
		return 1;
	}
	
	nirModule.build (target);
	target->finalise ();
	
	report::printLogs ();
	return report::compilationFailed ();
	
}
