#include <ast/passes/execute.hpp>
#include <ast/passes/nir.hpp>
#include <ast/sequence.hpp>
#include <codegen/target.hpp>
#include <codegen/llvm.hpp>
#include <file.hpp>
#include <fstream>
#include <nir/module.hpp>
#include <nir/scope.hpp>
#include <utility/config.hpp>
#include <utility/report.hpp>


int numbatMain (const Config & cfg) {
	
	codegen::Target * target = codegen::Target::find ("llvm");
	
	nir::Module nirModule;
	{
		DynArray <AST::NodePtr> seq;
		for (const string & file : cfg.files) {
			auto * f = numbat::File::compile (file);
			seq.push_back (f->getAST ());
		}
		seq.push_back (numbat::File::builtIn ()->getAST ());
		auto program = std::make_shared <AST::Sequence> (numbat::lexer::position {0, 0}, numbat::File::builtIn (), seq);
		auto transformed = AST::transform (program);
		
		if (cfg.interpret) {
			AST::ExecutePass exec;
			exec (transformed);
			if (cfg.outfile != "a.out" and not report::compilationFailed ()) {
				std::ofstream fout (cfg.outfile);
				fout << "success" << std::endl;
			}
			
		} else {
			AST::NirPass nirPass (nirModule.getGlobalScope ());
			nirPass (transformed);
			
		}
		
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
