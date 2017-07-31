#include <ast/meta.hpp>
#include <map>
#include <utility/report.hpp>
#include <vector>


namespace AST {


std::map <TypePtr, size_t> typeIDmap;
DynArray <TypePtr> revrseTypeIDmap;


std::pair <string, FuncPtr> APIfunc (const string & iden, const BasicArray <TypePtr> & params, const BasicArray <TypePtr> & retVals, const std::function <void(void*, void*)> &func) {
	auto fPtr = std::make_shared <Function> ();
	fPtr->iden = iden;
	fPtr->nativeFunction = func;
	fPtr->params = params;
	fPtr->retVals = retVals;
	return std::make_pair (iden, fPtr);
}


std::map <string, FuncPtr> Reflect::apiFuncs;


FuncPtr Reflect::api (const string & iden) {
	if (apiFuncs.empty ()) initAPI ();
	auto itt = apiFuncs.find (iden);
	auto end = apiFuncs.end ();
	if (itt != end) {
		return itt->second;
	}
	return nullptr;
}


void Reflect::initAPI () {
	apiFuncs.insert (APIfunc (
		"AST.Pointer",
		{Numeric::get (Numeric::ArithmaticType::INT, 0)},
		{Numeric::get (Numeric::ArithmaticType::INT, 0)},
		[](void * voidIn, void * voidOut) {
			auto * in = reinterpret_cast <size_t *> (voidIn);
			auto * out = reinterpret_cast <size_t *> (voidOut);
			auto baseType = revrseTypeIDmap [*in];
			auto refType = std::make_shared <Ref> (baseType->getPos (), baseType);
			size_t typeID = revrseTypeIDmap.size ();
			typeIDmap [refType] = typeID;
			revrseTypeIDmap.push_back (refType);
			*out = typeID;
		}
	));
}


string Reflect::toString (text::PrintMode mode) const {
	
	return metaTag->toString (mode) + " " + target->toString (mode);
	
}


std::map <std::pair <Node*, Type*>, TypePtr> ReflectType::typeMap;

TypePtr ReflectType::get (const NodePtr & metaTag, const TypePtr & target) {
	auto key = std::make_pair (metaTag.get (), target.get ());
	auto itt = typeMap.find (key);
	auto end = typeMap.end ();
	if (itt == end) {
		return typeMap [key] = std::shared_ptr <ReflectType> (new ReflectType (metaTag, target));
	} else {
		return itt->second;
	}
}

string ReflectType::toString (text::PrintMode mode) const {
	
	return metaTag->toString (mode) + " " + target->toString (mode);
	
}

}
