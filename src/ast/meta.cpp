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


std::map <string, FuncPtr> Reflect::apiFuncs = {
	{APIfunc (
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
	)}
};


FuncPtr Reflect::api (const string & iden) {
	auto itt = apiFuncs.find (iden);
	auto end = apiFuncs.end ();
	if (itt != end) {
		return itt->second;
	}
	return nullptr;
}


string Reflect::toString (text::PrintMode mode) const {
	
	return metaTag->toString (mode) + " " + target->toString (mode);
	
}

}
