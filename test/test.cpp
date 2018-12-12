#include <ast/call.hpp>
#include <ast/context.hpp>
#include <ast/flowcontrol.hpp>
#include <ast/function.hpp>
#include <ast/meta.hpp>
#include <ast/variable.hpp>
#include <file.hpp>
#include <gtest/gtest.h>
#include <utility/config.hpp>


BasicArray <string> executeFiles = {
	"flow_control/if_else.nbt",
	"numbers/unsigned.nbt",
	"system_calls/open_close.nbt",
	"system_calls/read.nbt"
};


int main (int argl, char ** args) {
	
	{
		
		auto fPtr = std::make_shared <AST::Function> ();
		auto context = std::make_unique <AST::Context> (*numbat::File::builtIn ()->getContext ().getRootContext (), fPtr.get ());
		auto param = std::make_shared <AST::Variable> (
			numbat::lexer::position {0, 0}, 
			context->getSourceFile (), 
			AST::Numeric::get (AST::Numeric::ArithmaticType::UINT, 1), 
			0, 
			AST::Variable::LOCATION::LOCAL, 
			"b"
		);
		context->var ("b", param);
		context->allocStack (param);
		auto sysCall = std::make_shared <AST::SystemCall> (
			numbat::lexer::position {0, 0}, 
			numbat::File::builtIn (), 
			AST::Numeric::get (AST::Numeric::ArithmaticType::INT, 32), 
			"exit", 
			BasicArray <AST::NodePtr> {
				std::make_shared <AST::StaticValue> (
					numbat::lexer::position {0, 0}, 
					numbat::File::builtIn (), 
					AST::Numeric::get (AST::Numeric::ArithmaticType::INT, 32), 
					1
				)
			}
		);
		auto body = std::make_shared <AST::IfElse> (
			numbat::lexer::position {0, 0},
			numbat::File::builtIn (),
			param,
			param,
			sysCall
		);
		
		fPtr->params = {param->getType ()};
		fPtr->replaceBody (body);
		fPtr->positions ["b"] = 0;
		fPtr->iden = "assert";
		fPtr->initialStack = context->getLocalStack ();
		
		numbat::File::builtIn ()->getContext ().getRootContext ()->func ("assert", fPtr);
		
	}
	
	Literal l (mpq_class (4));
	assert (l == l.to_double ());
	
	Config::mutableGlobalConfig ().const_folding = false;
	Config::mutableGlobalConfig ().prune_dead_code = false;
	::testing::InitGoogleTest (&argl, args);
	return RUN_ALL_TESTS ();
}
