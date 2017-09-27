#include <ast/call.hpp>
#include <ast/meta.hpp>
#include <ast/passes/metatagswizzle.hpp>
#include <ast/variable.hpp>


namespace AST {


void MetaTagSwizzlePass::visit (const Reflect & node) {
	nPtr = SwizzleSubjectPass (node.getMetaTag ()) (node.getTarget ());
}


void SwizzleSubjectPass::visit (const Unresolved_Constructor & node) {
	auto type = node.getVar ()->getType ();
	type = ReflectType::get (metaTag, type);
	auto var = std::make_shared <Variable> (node.getVar ()->getPos (), node.getVar ()->getFile (), node.getVar ()->getIden (), type);
	nPtr = std::make_shared <Unresolved_Constructor> (node.getPos (), node.getFile (), var, node.getArgs ());
}

void SwizzleSubjectPass::visit (const Variable & node) {
	auto type = node.getType ();
	type = ReflectType::get (metaTag, type);
	nPtr = std::make_shared <Variable> (node.getPos (), node.getFile (), node.getIden (), type, node.getCurrentValue ());
}


}
