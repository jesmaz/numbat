#include <ast/type.hpp>
#include <nir/module.hpp>
#include <nir/parameter.hpp>
#include <nir/scope.hpp>
#include <nir/type/tuple.hpp>
#include <parse/tree/struct.hpp>
#include <utility/report.hpp>


namespace parser {


AST::TypePtr Struct::createType (AST::Context & ctx) {
	return stype = std::make_shared <AST::Struct> (getPos (), ctx.getSourceFile (), iden);
}

AST::NodePtr Struct::createAST (AST::Context & ctx) {
	if (not stype) {
		stype = std::make_shared <AST::Struct> (getPos (), ctx.getSourceFile (), iden);
	}
	size_t position=0;
	stype->members = members.map <AST::NodePtr> ([&](auto & m){
		auto param = m->createASTparam (ctx);
		stype->positionMap [m->getIden ()] = position;
		position++;
		return param;
	});
	return stype;
}

string Struct::strDump (text::PrintMode mode) {
	string s = (mode&text::COLOUR ? text::blu + "struct " + text::reset : "struct ") + iden;
	s += " {" + text::listPtrToStr (members.begin (), members.end (), mode) + "}";
	return s;
}


}
