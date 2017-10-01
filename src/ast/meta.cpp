#include <ast/literal.hpp>
#include <ast/meta.hpp>
#include <map>
#include <utility/report.hpp>
#include <vector>


namespace AST {


std::map <TypePtr, size_t> typeIDmap;
DynArray <TypePtr> reverseTypeIDmap;


std::pair <string, FuncPtr> APIfunc (const string & iden, const BasicArray <TypePtr> & params, const BasicArray <TypePtr> & retVals, const std::function <const BasicArray <NodePtr>(const BasicArray <NodePtr>, const CallingData &)> & func) {
	auto fPtr = std::make_shared <Function> ();
	fPtr->iden = iden;
	fPtr->nativeFunction = func;
	fPtr->params = params;
	fPtr->retVals = retVals;
	fPtr->metaData ["functional"] = "true";
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
		[](const BasicArray <NodePtr> args, const CallingData &) -> const BasicArray <NodePtr> {
			assert (args.size () == 1);
			assert (typeid (*(args [0].get ())) == typeid (AST::Number));
			auto number = reinterpret_cast <const AST::Number*> (args [0].get ());
			assert (number->getType () == Numeric::get (Numeric::ArithmaticType::INT, 0));
			auto in = number->getValue ()->toUint64 ();
			auto baseType = reverseTypeIDmap [in];
			auto refType = Ref::get (baseType);
			size_t typeID = reverseTypeIDmap.size ();
			typeIDmap [refType] = typeID;
			reverseTypeIDmap.push_back (refType);
			return {std::make_shared <AST::Number> (number->getPos (), number->getFile (), std::make_shared <NumericLiteralTemplate <uint64_t>> (typeID), number->getType ())};
		}
	));
	apiFuncs.insert (APIfunc (
		"static_assert",
		{Numeric::get (Numeric::ArithmaticType::UINT, 1)},
		{Numeric::get (Numeric::ArithmaticType::UINT, 1)},
		[](const BasicArray <NodePtr> args, const CallingData & callingData) -> const BasicArray <NodePtr> {
			assert (args.size () == 1);
			auto number = static_cast <const AST::Number*> (args [0].get ());
			assert (number);
			assert (number->getType () == Numeric::get (Numeric::ArithmaticType::UINT, 1));
			bool assertion = number->getValue ()->toUint64 ();
			if (not assertion) {
				report::logMessage (report::Severity::ERROR, callingData.file, callingData.position, "Static assertion failed");
			}
			return args;
		}
	));
}


string Reflect::toString (text::PrintMode mode) const {
	
	return metaTag->toString (mode) + " " + target->toString (mode);
	
}


std::map <std::pair <Node*, Type*>, TypePtr> ReflectType::typeMap;

size_t ReflectType::getTypeId (const TypePtr & type) {
	auto itt = typeIDmap.find (type);
	if (itt != typeIDmap.end ()) {
		return itt->second;
	} else {
		auto id = typeIDmap [type] = reverseTypeIDmap.size ();
		reverseTypeIDmap.push_back (type);
		return id;
	}
}

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

TypePtr ReflectType::getType (size_t id) {
	assert (id < reverseTypeIDmap.size ());
	return reverseTypeIDmap [id];
}

string ReflectType::toString (text::PrintMode mode) const {
	
	return metaTag->toString (mode) + " " + target->toString (mode);
	
}

}
