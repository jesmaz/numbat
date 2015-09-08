#include <iostream>

#include "../include/ast.hpp"
#include "../include/numbat.hpp"
#include "../include/parse/numbatparser.hpp"



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
	
	numbat::Numbat numbat;
	
	createType ("ptrint", numbat.getEngine ()->getDataLayout ()->getPointerSizeInBits (), numbat::parser::NumbatRawType::UNSIGNED);
	
	auto module = numbat::parser::Module::import ("core util");
	numbat::NumbatParser parser;
	string line;
	numbat::parser::AbstractSyntaxTree * ast = module->getAST ();
	//numbat::parser::BodyGenerator generator;
	//ast->accept (generator);
	for (;;) {
		std::cout << " >> " << std::flush;
		std::getline (std::cin, line);
		PTNode parseTree = parser.parse (line);
		assert (typeid (*parseTree) == typeid (ParseTree));
		for (PTNode n : reinterpret_cast <ParseTree*> (parseTree)->getBody ()) {
			numbat::parser::ASTnode node = n->build (ast);
			//node->accept (generator);
			if (node) {
				std::cout << node->toString () << '\n';
			} else {
				std::cout << "nil\n";
			}
		}
	}
	return 0;
	
}