#include "../include/main.hpp"

using namespace llvm;
using namespace numbat;

std::string loadFromFile (const std::string & file) {
    std::ifstream fin (file);
    std::string out;
    std::string buffer;
    if (fin.is_open ()) {
        while (std::getline (fin, buffer))
            out += buffer + "\n";
        fin.close ();
    }
    return out;
}

struct cstruct {
	double a, b, c;
};

struct testStruct {
	uint32_t s, t;
};

int main (int argl, char ** args) {
	
	InitializeAllTargets ();
	InitializeAllTargetMCs();
	InitializeAllAsmPrinters();
	InitializeAllAsmParsers();
	
	string outfile = "a.out";
	bool emitAssembly=false;
	bool link=true;
	std::set <string> files;
	for (int i=1; i<argl; ++i) {
		char * str = args [i];
		//int len = strlen (str);
		if (str [0] == '-') {
			if (str [1] == '-') {
				//TODO: handle options
			} else {
				for (int j=1; str [j]; ++j) {
					switch (str[j]) {
						case 'c':
							link = false;
							break;
						case 'o':
							if (i+1<argl) {
								outfile = args [i+1];
								++i;
							} else {
								//TODO: handle invalid options
							}
							break;
						case 's':
							emitAssembly = true;
							break;
						default:
							//TODO: handle invalid options
							break;
					}
				}
			}
		} else {
			files.insert (str);
		}
	}
	
	shared_ptr <parser::Module> core = parser::Module::createEmpty ("numbat core");
	core->insertStatmentParser ("while", &numbat::parser::parseWhileLoop);
	Numbat numbat;
	for (const string & file : files) {
		numbat.loadFromModule (parser::Module::createFromFile (file));
	}
	
	Module * mod = numbat.getModule ();
	Triple theTriple (mod->getTargetTriple ());
	
	string error;
	const Target * target = TargetRegistry::lookupTarget ("x86-64", theTriple, error);
	if (!target) {
		std::cerr << error << std::endl;
		return 1;
	}
	SubtargetFeatures features;
	features.AddFeature ("64bit", true);
	
	TargetOptions options;
	
	OwningPtr <TargetMachine> mchPtr (target->createTargetMachine (theTriple.getTriple (), "generic", features.getString (), options));
	if (!mchPtr.get ()) {
		std::cerr << "Could not allocate the target machine" << std::endl;
		return 1;
	}
	TargetMachine * machine = mchPtr.get ();
	
	machine->setAsmVerbosityDefault (true);
	
	PassManager PM;
	PM.add (new TargetLibraryInfo (theTriple));
	machine->addAnalysisPasses (PM);
	PM.add (new DataLayout (*machine->getDataLayout ()));
	
	TargetMachine::CodeGenFileType fileType = TargetMachine::CGFT_ObjectFile;
	int flags = 0;
	if (!emitAssembly) {
		flags = raw_fd_ostream::F_Binary;
	} else {
		fileType = TargetMachine::CGFT_AssemblyFile;
	}
	OwningPtr <tool_output_file> out (new tool_output_file (outfile.c_str (), error, flags));
	if (!error.empty ()) {
		std::cerr << error << std::endl;
		return 1;
	}
	{
		formatted_raw_ostream fos (out->os ());
		if (machine->addPassesToEmitFile (PM, fos, fileType)) {
			std::cerr << "The target does not suport the generaton of this file type" << std::endl;
			return 1;
		}
	
		PM.run (*mod);
	}
	
	out->keep ();
	
	/*if (link) {
		
	} else {
		
	}*/
	
	
	/*std::string file = loadFromFile ("test.nbt");
	lexer::tkstring test = lexer::lexFile (file);
	parser::AbstractSyntaxTree ast (test.begin (), lexer::findEOF (test.begin (), test.end ()));
	std::cout << ast.toString () << std::endl << "Build success: " << std::boolalpha << ast.buildSuccessfull () << std::endl;
	if (ast.buildSuccessfull ()) {
		string errStr;
		Module * module = new Module ("main", getGlobalContext ());
		InitializeNativeTarget ();
		ExecutionEngine * engine = EngineBuilder (module).setErrorStr (&errStr).create ();
		FunctionPassManager fpm (module);
		fpm.add (new DataLayout(*engine->getDataLayout()));
		fpm.add (createAAEvalPass ());
		fpm.add (createInstructionCombiningPass ());
		fpm.add (createReassociatePass ());
		fpm.add (createGVNPass ());
		fpm.add (createCFGSimplificationPass ());
		fpm.doInitialization ();
		parser::BodyGenerator generator (module, &fpm);
		generator.visit (ast);
	}*/
	//Numbat numbat;
	//numbat.loadFromFile ("test.nbt");
	//numbat.loadFromModule (parser::Module::createFromFile ("test.nbt"));
	//uint64_t t;
	//numbat.getFunction <void, uint64_t *> ("tfunc") (&t);
	//std::cout << t << std::endl;
	//std::cout << (numbat.getFunction <int64_t> ("cCallbackTest") ()) << std::endl;
	return 0;
}

void callback () {
	std::cout << "callback" << std::endl;
}
