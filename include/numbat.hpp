#ifndef NUMBAT_HPP
#define NUMBAT_HPP

#include <fstream>

#include "ast.hpp"
#include "module.hpp"
#include "generator.hpp"
#include "visitor.hpp"

namespace numbat {

using namespace llvm;
using lexer::tkstring;

class Numbat {

	public:
		
		template <typename Ret, typename ... Args>
		struct NumbatFunction {
			public:
				operator bool () {return fptr != nullptr;}
				Ret operator () (Args ... args) {return fptr (args ...);}
				friend Numbat;
				NumbatFunction () : fptr (nullptr) {}
			private:
				NumbatFunction (void * fp) {
					// Would have prefered 'fptr (reinterpret_cast <Ret (*) (Args ...)> (fp))' but the ide was having a meldown
					*reinterpret_cast <void **> (&fptr) = fp;
				}
				Ret (* fptr) (Args ...);
		};
		
		template <typename Ret>
		struct NumbatFunction <Ret, void> {
			public:
				operator bool () {return fptr != nullptr;}
				Ret operator () () {return fptr ();}
				friend Numbat;
				NumbatFunction () : fptr (nullptr) {}
			private:
				NumbatFunction (void * fptr) : fptr (reinterpret_cast <Ret (*) ()> (fptr)) {}
				Ret (* fptr) ();
		};
		
		struct NumbatType {
			public:
				operator bool () {return type != nullptr;}
				friend Numbat;
				NumbatType () : type (nullptr) {}
			private:
				NumbatType (Type * t) : type (t) {}
				Type * type;
		};
		
		template <typename Ret, typename ... Args>
		NumbatFunction <Ret, Args ...> getFunction (const std::string & func) {return NumbatFunction <Ret, Args ...> (reinterpret_cast <void*> (engine->getFunctionAddress (func)));}
		
		void dump () {module->dump ();}
		void loadFromCode (const std::string & code);
		void loadFromFile (const std::string & file);
		void loadFromModule (const shared_ptr <parser::Module> & mod);
		
		template <typename T>
		void registerFunction (FunctionType * ft, const std::string & name, T funcPtr) {engine->addGlobalMapping (Function::Create (ft, Function::ExternalLinkage, name, module), reinterpret_cast <void *> (funcPtr));}
		//template <typename T, typename Ret, typename ... Args>
		//void registerFunction (const std::string & name, T funcPtr);
		ExecutionEngine * getEngine () {return engine;}
		Module * getModule () {return module;}
		
		Numbat ();
		//Numbat (const Numbat & other);
		//~Numbat ();
		//Numbat & operator = (const Numbat & other);
		//bool operator == (const Numbat & other) const;
		
	private:
		
		ExecutionEngine * engine;
		Module * module;
		
};
};

#endif // NUMBAT_HPP
