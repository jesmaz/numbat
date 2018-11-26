#include <ast/call.hpp>
#include <ast/context.hpp>
#include <ast/variable.hpp>
#include <file.hpp>
#include <forward.hpp>


namespace numbat {

namespace modules {

namespace builtin {


File * linuxModule () {
	
	auto pair = File::newBuiltinModule ("linux");
	
	auto context = pair.second;
	
	{
		
		auto fPtr = std::make_shared <AST::Function> ();
		auto pos = numbat::lexer::position {0, 0};
		auto f = context->getSourceFile ();
		auto cInt = AST::Numeric::get (AST::Numeric::ArithmaticType::INT, 32);
		auto counter = 0;
		
		auto makeParam = [&] (const AST::TypePtr & type, const string & name) {
			auto loc = AST::Variable::LOCATION::LOCAL;
			auto var = std::make_shared <AST::Variable> (pos, f, type, 0, loc, name);
			context->var (name, var);
			context->allocStack (var);
			fPtr->positions [name] = counter++;
			return var;
		};
		
		auto pathNameParam = makeParam (
			AST::Ref::get (AST::Const::get (AST::Numeric::get (AST::Numeric::ArithmaticType::UINT, 8))),
			"pathname"
		);
		auto flagsParam = makeParam (cInt, "flags");
		auto modeParam = makeParam (cInt,"mode");
		
		auto sysCall = std::make_shared <AST::SystemCall> (pos, f, cInt, "open", BasicArray <AST::NodePtr> {pathNameParam, flagsParam, modeParam});
		
		fPtr->params = {pathNameParam->getType (), flagsParam->getType (), modeParam->getType ()};
		fPtr->retVals = {cInt};
		fPtr->replaceBody (sysCall);
		fPtr->iden = "open";
		fPtr->initialStack = context->getLocalStack ();
		
		numbat::File::builtIn ()->getContext ().getRootContext ()->func ("open", fPtr);
		
	}
	
	return pair.first;
	
}


File * findContext (const string & id) {
	
	if (id == "linux") {
		return linuxModule ();
	}
	
	return nullptr;
	
}


}

}

}

