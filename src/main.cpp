#include <codegen/target.hpp>
#include <codegen/llvm.hpp>
#include <file.hpp>
#include <utility/config.hpp>


int main (int argl, char ** args) {
	
	Config::initalise (argl, args);
	const Config & cfg = Config::globalConfig ();
	
	codegen::Target * target = new codegen::LLVM;
	
	nir::Module nirModule (target);
	for (const string & file : cfg.files) {
		numbat::File::compile (file, &nirModule);
	}
	
	if (not nirModule.validate ()) {
		return 1;
	}
	
	nirModule.build ();
	return 0;
	
}