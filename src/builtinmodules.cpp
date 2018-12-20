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
	
	auto ctx = pair.second;
	auto cInt = AST::Numeric::get (AST::Numeric::ArithmaticType::INT, 32);
	auto cStr = AST::Ptr::get (AST::Const::get (AST::Numeric::get (AST::Numeric::ArithmaticType::UINT, 8)));
	auto size = AST::Numeric::get (AST::Numeric::ArithmaticType::UINT, 0);
	auto ssize = AST::Numeric::get (AST::Numeric::ArithmaticType::INT, 0);
	auto dataBuffer = AST::Ptr::get (AST::Numeric::get (AST::Numeric::ArithmaticType::UINT, 8));
	
	auto createSysCall = [&](const string & callName, const BasicArray <std::pair <string, AST::TypePtr>> & args, const AST::TypePtr & ret) {
		
		auto fPtr = std::make_shared <AST::Function> ();
		auto context = std::make_unique <AST::Context> (*ctx, fPtr.get ());
		auto pos = numbat::lexer::position {0, 0};
		auto f = context->getSourceFile ();
		auto counter = 0;
		
		auto makeParam = [&] (const std::pair <string, AST::TypePtr> & pair) {
			const string & name = pair.first;
			const AST::TypePtr & type = pair.second;
			auto loc = AST::Variable::LOCATION::LOCAL;
			auto var = std::make_shared <AST::Variable> (pos, f, type, 0, loc, name);
			context->var (name, var);
			context->allocStack (var);
			fPtr->positions [name] = counter++;
			return var;
		};
		
		auto params = args.map <AST::NodePtr> (makeParam);
		
		auto sysCall = std::make_shared <AST::SystemCall> (pos, f, cInt, callName, params);
		
		fPtr->params = params.map <AST::TypePtr> ([](auto & p){return p->getType ();});
		fPtr->retVals = {ret};
		fPtr->replaceBody (sysCall);
		fPtr->iden = callName;
		fPtr->initialStack = context->getLocalStack ();
		
		numbat::File::builtIn ()->getContext ().getRootContext ()->func (callName, fPtr);
		
	};
	
	createSysCall ("close", {{"fd", cInt}}, cInt);
	createSysCall ("open", {{"pathname", cStr}, {"flags", cInt}, {"mode", cInt}}, cInt);
	createSysCall ("read", {{"fd", cInt}, {"buffer", dataBuffer}, {"bytes", size}}, ssize);
	
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

