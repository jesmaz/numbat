#include <ast/passes/nir.hpp>
#include <codegen/target.hpp>
#include <codegen/llvm.hpp>
#include <file.hpp>
#include <nir/module.hpp>
#include <utility/config.hpp>
#include <utility/report.hpp>


int numbatMain (const Config & cfg) {
	
	codegen::Target * target = codegen::Target::find ("llvm");
	
	nir::Module nirModule;
	AST::NirPass nirPass (nirModule.getGlobalScope ());
	for (const string & file : cfg.files) {
		auto * f = numbat::File::compile (file);
		nirPass (f->getAST ());
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
