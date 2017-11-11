#include <ast/meta.hpp>
#include <ast/passes/execute.hpp>
#include <ast/passes/reflectpass.hpp>
#include <ast/passes/resolve.hpp>
#include <ast/literal.hpp>
#include <ast/variable.hpp>


namespace AST {


void ReflectPass::visit (const Reflect & node) {
	auto metaTag = node.getMetaTag ();
	
}

void ReflectPass::visit (const ReflectType & node) {
	auto metaTag = node.getMetaTag ();
	auto target = node.getTarget ();
	auto arg = std::make_shared <Value> (
		node.getPos (),
		node.getFile (),
		Numeric::get (Numeric::ArithmaticType::INT, 0),
		ReflectType::getTypeId (target)
	);
	auto call = MakeCallPass (node.getPos (), node.getFile (), {arg}) (metaTag);
	auto res = FoldConstPass () (call);
	auto typeID = std::dynamic_pointer_cast <Value> (res);
	if (typeID) {
		size_t id = typeID->getLiteral ().to_uint64 ();
		auto type = ReflectType::getType (id);
		nPtr = type;
	} else {
		abort ();
	}
}

void ReflectPass::visit (const Variable & node) {
	
	auto type = std::dynamic_pointer_cast <Type> (ReflectPass () (node.getType ()));
	*std::dynamic_pointer_cast <Variable> (nPtr) = Variable (node.getPos (), node.getFile (), node.getIden (), type, node.getCurrentValue ());
	
}


}
