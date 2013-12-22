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
	Numbat numbat;
	numbat.loadFromFile ("test.nbt");
	uint64_t t;
	//numbat.getFunction <void, uint64_t *> ("tfunc") (&t);
	//std::cout << t << std::endl;
	std::cout << (int)*(numbat.getFunction <uint8_t *> ("func") ()) << std::endl;
	return 0;
}

void callback () {
	std::cout << "callback" << std::endl;
}
