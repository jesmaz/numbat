#include <iostream>

#include "../include/ast.hpp"
#include "../include/codegen/interpreter.hpp"
#include "../include/nir/module.hpp"
#include "../include/numbat.hpp"
#include "../include/parse/numbatparser.hpp"
#include <parse/handparser.hpp>



int main (int argc, char ** args) {
	
	numbat::parser::Module::addIncludeDir ("/usr/include/numbat");
	shared_ptr <numbat::parser::Module> core = numbat::parser::Module::createEmpty ("numbat core");
	
	auto createType = [&] (const string & iden, size_t s, numbat::parser::NumbatRawType::Type t) {
		if (!numbat::parser::createRawType (core->getAST (), iden, s, t)) std::cerr << "Failed to register " << iden << " as a type" << std::endl;
	};
	
	createType ("bool", 1, numbat::parser::NumbatRawType::UNSIGNED);
	
	createType ("uint8", 8, numbat::parser::NumbatRawType::UNSIGNED);
	createType ("uint16", 16, numbat::parser::NumbatRawType::UNSIGNED);
	createType ("uint32", 32, numbat::parser::NumbatRawType::UNSIGNED);
	createType ("uint64", 64, numbat::parser::NumbatRawType::UNSIGNED);
	
	createType ("int8", 8, numbat::parser::NumbatRawType::SIGNED);
	createType ("int16", 16, numbat::parser::NumbatRawType::SIGNED);
	createType ("int32", 32, numbat::parser::NumbatRawType::SIGNED);
	createType ("int64", 64, numbat::parser::NumbatRawType::SIGNED);
	
	createType ("half", 16, numbat::parser::NumbatRawType::FLOAT);
	createType ("float", 32, numbat::parser::NumbatRawType::FLOAT);
	createType ("double", 64, numbat::parser::NumbatRawType::FLOAT);
	createType ("quad", 128, numbat::parser::NumbatRawType::FLOAT);
	
	codegen::Interpreter
	interpreter;
	nir::Module nirmodule (&interpreter);
	auto globalScope = nirmodule.getGlobalScope ();
	
	nirmodule.registerPrimative (nir::Type::UINT, 1, "bool");
	
	nirmodule.registerPrimative (nir::Type::UINT, 8, "uint8");
	nirmodule.registerPrimative (nir::Type::UINT, 16, "uint16");
	nirmodule.registerPrimative (nir::Type::UINT, 32, "uint32");
	nirmodule.registerPrimative (nir::Type::UINT, 64, "uint64");
	
	nirmodule.registerPrimative (nir::Type::INT, 8, "int8");
	nirmodule.registerPrimative (nir::Type::INT, 16, "int16");
	nirmodule.registerPrimative (nir::Type::INT, 32, "int32");
	nirmodule.registerPrimative (nir::Type::INT, 64, "int64");
	
	nirmodule.registerPrimative (nir::Type::FPINT, 16, "half");
	nirmodule.registerPrimative (nir::Type::FPINT, 32, "float");
	nirmodule.registerPrimative (nir::Type::FPINT, 64, "double");
	nirmodule.registerPrimative (nir::Type::FPINT, 128, "quad");
	
	numbat::Numbat numbat;
	
	createType ("ptrint", numbat.getEngine ()->getDataLayout ()->getPointerSizeInBits (), numbat::parser::NumbatRawType::UNSIGNED);
	
	string line;
	for (;;) {
#ifndef N_PROMPT
		std::cout << " >> " << std::flush;
#endif
		if (not std::getline (std::cin, line)) {
			break;
		}
		auto parseTree = parse (line);
		std::cerr << parseTree->toString () << std::endl;
		const nir::Instruction * val = parseTree->build (globalScope, ParseTreeNode::BuildMode::NORMAL);
		std::cout << interpreter (val) << '\n';
	}
	return 0;
	
}