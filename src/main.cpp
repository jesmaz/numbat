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
	bool emitLLVM = false;
	bool jit = false;
	std::set <string> files;
	for (int i=1; i<argl; ++i) {
		char * str = args [i];
		//int len = strlen (str);
		if (str [0] == '-') {
			if (str [1] == '-') {
				//TODO: handle options
			} else {
				if (std::strcmp (str, "-emit-llvm") == 0) {
					emitLLVM = true;
					continue;
				} else if (std::strcmp (str, "-jit") == 0) {
					jit = true;
					continue;
				} else if (std::strcmp (str, "-debug-compiler") == 0) {
					parser::Module::setDebugMode (true);
					continue;
				}
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
						case 'S':
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
	
	parser::Module::addIncludeDir ("/usr/include/numbat");
	
	shared_ptr <parser::Module> core = parser::Module::createEmpty ("numbat core");
	
	core->addOperator (200, true, "{}", parser::parseBlockOperator);
	core->addOperator (200, true, "{ }", parser::parseBlockOperator);
	core->addOperator (200, true, "()", parser::parseSubExpression);
	core->addOperator (200, true, "( )", parser::parseSubExpression);
	core->addOperator (200, true, " ()", parser::parseCall);
	core->addOperator (200, true, " ( )", parser::parseCall);
	//core->addOperator (200, true, "[] ", parser::parseTemplate);
	//core->addOperator (200, true, "[ ] ", parser::parseTemplate);
	core->addOperator (200, true, " []", parser::parseIndex);
	core->addOperator (200, true, " [ ]", parser::parseIndex);
	core->addOperator (200, true, " [ ] ", parser::parseArrayDecleration);
	core->addOperator (200, true, " . ", parser::parseReferenceOperator);
	
	//core->addOperator (300, false, "++ ", parser::parseUnary, parser::defIterate);
	//core->addOperator (300, false, "-- ", parser::parseUnary, parser::defIterate);
	core->addOperator (300, false, "- ", parser::parseUnary, parser::defNegation);
	core->addOperator (300, false, "! ", parser::parseUnary, parser::defNegation);
	core->addOperator (300, false, "not ", parser::parseUnary, parser::defNegation);
	core->addOperator (300, false, "~ ", parser::parseUnary, parser::defNegation);
	
	core->addOperator (500, true, " * ", parser::parseBinary, parser::defArithmetic);
	core->addOperator (500, true, " / ", parser::parseBinary, parser::defArithmetic);
	core->addOperator (500, true, " % ", parser::parseBinary, parser::defArithmetic);
	
	core->addOperator (600, true, " + ", parser::parseBinary, parser::defArithmetic);
	core->addOperator (600, true, " - ", parser::parseBinary, parser::defArithmetic);
	core->addOperator (600, true, " ~ ", parser::parseBinary, parser::defConcat);
	
	core->addOperator (700, true, " << ", parser::parseBinary, parser::defArithmetic);
	core->addOperator (700, true, " >> ", parser::parseBinary, parser::defArithmetic);
	
	core->addOperator (800, true, " & ", parser::parseBinary, parser::defArithmetic);
	
	core->addOperator (900, true, " ^ ", parser::parseBinary, parser::defArithmetic);
	
	core->addOperator (1000, true, " | ", parser::parseBinary, parser::defArithmetic);
	
	core->addOperator (1100, true, " < ", parser::parseBinary, parser::defCompare);
	core->addOperator (1100, true, " <= ", parser::parseBinary, parser::defCompare);
	core->addOperator (1100, true, " > ", parser::parseBinary, parser::defCompare);
	core->addOperator (1100, true, " >= ", parser::parseBinary, parser::defCompare);
	
	core->addOperator (1200, true, " == ", parser::parseBinary, parser::defCompare);
	core->addOperator (1200, true, " != ", parser::parseBinary, parser::defCompare);
	
	core->addOperator (1300, true, " and ", parser::parseBinary, parser::defLogic);
	
	core->addOperator (1400, true, " or ", parser::parseBinary, parser::defLogic);
	
	core->addOperator (1500, true, " , ", parser::parseComma);
	
	core->addOperator (1600, false, " = ", parser::parseAssignmentOperator);
	core->addOperator (1600, false, " += ", parser::parseAssignmentOperator);
	core->addOperator (1600, false, " -= ", parser::parseAssignmentOperator);
	core->addOperator (1600, false, " ~= ", parser::parseAssignmentOperator);
	core->addOperator (1600, false, " *= ", parser::parseAssignmentOperator);
	core->addOperator (1600, false, " /= ", parser::parseAssignmentOperator);
	core->addOperator (1600, false, " %= ", parser::parseAssignmentOperator);
	core->addOperator (1600, false, " <<= ", parser::parseAssignmentOperator);
	core->addOperator (1600, false, " >>= ", parser::parseAssignmentOperator);
	core->addOperator (1600, false, " &= ", parser::parseAssignmentOperator);
	core->addOperator (1600, false, " ^= ", parser::parseAssignmentOperator);
	core->addOperator (1600, false, " |= ", parser::parseAssignmentOperator);
	core->addOperator (1600, false, " => ", parser::parseRedirectOperator);
	
	core->addOperator (1700, false, "while( ) ", parser::parseWhileLoop);
	
	core->addBrace ("(", ")");
	core->addBrace ("{", "}");
	core->addBrace ("[", "]");
	core->addBrace ("?", ":");
	
	auto createType = [&] (const string & iden, size_t s, parser::NumbatRawType::Type t) {
		if (!parser::createRawType (core->getAST (), iden, s, t)) std::cerr << "Failed to register " << iden << " as a type" << std::endl;
	};
	
	createType ("bool", 1, parser::NumbatRawType::UNSIGNED);
	
	createType ("uint8", 8, parser::NumbatRawType::UNSIGNED);
	createType ("uint16", 16, parser::NumbatRawType::UNSIGNED);
	createType ("uint32", 32, parser::NumbatRawType::UNSIGNED);
	createType ("uint64", 64, parser::NumbatRawType::UNSIGNED);
	
	createType ("int8", 8, parser::NumbatRawType::SIGNED);
	createType ("int16", 16, parser::NumbatRawType::SIGNED);
	createType ("int32", 32, parser::NumbatRawType::SIGNED);
	createType ("int64", 64, parser::NumbatRawType::SIGNED);
	
	createType ("half", 16, parser::NumbatRawType::FLOAT);
	createType ("float", 32, parser::NumbatRawType::FLOAT);
	createType ("double", 64, parser::NumbatRawType::FLOAT);
	createType ("quad", 128, parser::NumbatRawType::FLOAT);
	
	Numbat numbat;
	
	createType ("ptrint", numbat.getEngine ()->getDataLayout ()->getPointerSizeInBits (), parser::NumbatRawType::UNSIGNED);
	
	for (const string & file : files) {
		shared_ptr <parser::Module> mod = parser::Module::createFromFile (file);
		if (!mod->validate ()) {
			std::cerr << "Build failed" << std::endl;
			return 0;
		}
		numbat.loadFromModule (mod);
	}
	
	if (jit) {
		numbat.getFunction <int> ("__entry__") ();
	}
	
	Module * mod = numbat.getModule ();
	Triple theTriple (mod->getTargetTriple ());
	
	string error;
	
	if (emitAssembly and emitLLVM) {
		tool_output_file out (outfile.c_str (), error, sys::fs::F_None);
		mod->print (out.os (), nullptr);
		out.keep ();
		return 0;
	}
	
	const Target * target = TargetRegistry::lookupTarget ("x86-64", theTriple, error);
	if (!target) {
		std::cerr << error << std::endl;
		return 1;
	}
	SubtargetFeatures features;
	features.AddFeature ("64bit");
	
	TargetOptions options;
	
	TargetMachine * machine = target->createTargetMachine (theTriple.getTriple (), "generic", features.getString (), options);
	if (!machine) {
		std::cerr << "Could not allocate the target machine" << std::endl;
		return 1;
	}
	
	machine->setAsmVerbosityDefault (true);
	
	PassManager PM;
	PM.add (new TargetLibraryInfo (theTriple));
	machine->addAnalysisPasses (PM);
	PM.add (new DataLayoutPass (*machine->getDataLayout ()));
	
	TargetMachine::CodeGenFileType fileType = TargetMachine::CGFT_ObjectFile;
	if (emitAssembly) {
		link = false;
		fileType = TargetMachine::CGFT_AssemblyFile;
	}
	tool_output_file out (".numbat.o", error, sys::fs::F_None);
	if (!error.empty ()) {
		std::cerr << error << std::endl;
		return 1;
	}
	{
		formatted_raw_ostream fos (out.os ());
		if (machine->addPassesToEmitFile (PM, fos, fileType)) {
			std::cerr << "The target does not suport the generaton of this file type" << std::endl;
			return 1;
		}
	
		PM.run (*mod);
	}
	
	out.keep ();
	
	if (link) {
		system (("ld .numbat.o -e __entry__ -lc -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o " + outfile).c_str ());
	} else {
		system (("mv .numbat.o " + outfile).c_str ());
	}
	
	
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
