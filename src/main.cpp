#include <codegen/target.hpp>
#include <codegen/llvm.hpp>
#include <file.hpp>
#include <utility/config.hpp>


int main (int argl, char ** args) {
	
	Config::initalise (argl, args);
	const Config & cfg = Config::globalConfig ();
	
	codegen::Target * target = codegen::Target::find ("llvm");
	
	nir::Module nirModule;
	for (const string & file : cfg.files) {
		numbat::File::compile (file, &nirModule);
	}
	
	if (not nirModule.validate ()) {
		return 1;
	}
	
	nirModule.build (target);
	target->finalise ();
	return 0;
	
}
