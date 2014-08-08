#include "../include/ast.hpp"
#include "../include/core.hpp"

namespace numbat {
using namespace lexer;
namespace parser {

//std::map <string, shared_ptr <OperatorDecleration>> AbstractSyntaxTree::operators;
//std::multimap <string, shared_ptr <OperatorDecleration>> AbstractSyntaxTree::operatorsByFirstToken;
//std::set <shared_ptr <OperatorDecleration>, std::greater <shared_ptr <OperatorDecleration>>> AbstractSyntaxTree::precidenceOrderedOperators;
//std::unordered_set <string> AbstractSyntaxTree::parenOpperators, AbstractSyntaxTree::oppTokens, AbstractSyntaxTree::ternaryStart;


AbstractSyntaxTree::AbstractSyntaxTree (tkitt beg, tkitt end, const string & path, const string & file) : path (path), file (file) {
	
	importModule (Module::createEmpty ("numbat core"), true);
	importModule (Module::import (path, "core util"), false);
	
	
	std::vector <std::pair <FunctionDecleration *, std::pair <size_t, tkitt>>> funcReparse;
	std::vector <std::pair <string, tkitt>> typeReparse;
	
	itt = beg;
	
	string iden;
	FunctionDecleration * funcDec;
	while (itt != end) {
		switch (itt->type) {
			case TOKEN::def:
				{
					funcDec = parseFunctionDecleration (end);
					int oldIndent = indentLevel;
					funcReparse.push_back (std::make_pair (funcDec, std::make_pair (indentLevel, itt)));
					eatSemicolon (end);
					if (itt->iden == "{") {
						nextToken (end);
						itt = findToken ("}", end);
						nextToken (end);
					} else {
						itt = findIndent (oldIndent, end);
					}
				}
				break;
				
			case TOKEN::enumtkn:
				parseEnum (end);
				break;
				
			case TOKEN::externdef:
				nextToken (end);
				parseFunctionDecleration (end);
				eatSemicolon (end);
				break;
				
			case TOKEN::import:
				parseImport (end);
				eatSemicolon (end);
				break;
				
			case TOKEN::indent:
				nextToken (end);
				break;
				
			case TOKEN::operatortkn:
				parseOperatorDecleration (end);
				eatSemicolon (end);
				break;
				
			case TOKEN::structure:
				iden = parseStructDecleration (end);
				typeReparse.push_back (std::make_pair (iden, itt));
				if (itt->iden == "{") nextToken (end), itt = findToken ("}", end), nextToken (end);
				eatSemicolon (end);
				break;
				
			case TOKEN::typedeftkn:
				parseTypeDef (end);
				break;
				
			case TOKEN::semicolon:
				nextToken (end);
				break;
				
			default:
				if (ASTnode n = parser::parseExpression (this, end)) {
					body.push_back (n);
				} else {
					nextToken (end);
				}
				break;
				
		}
	}
	
	for (auto var : variables) {
		var.second->isGlobal (true);
	}
	
	for (std::pair <string, tkitt> & pair : typeReparse) {
		itt = pair.second;
		if (itt->iden == "{") {
			NumbatType & type = *types [pair.first];
			nextToken (end);
			auto endItt = findToken ("}", end);
			type.buildData (parseArgs (&AbstractSyntaxTree::parseParameter, endItt));
			nextToken (end);
		}
	}
	
	auto oldVariables = variables;
	for (std::pair <FunctionDecleration *, std::pair <size_t, tkitt>> & pair : funcReparse) {
		if (pair.first) {
			itt = pair.second.second;
			indentLevel = pair.second.first;
			for (const ASTnode & arg : pair.first->getArgs ()) {
				ASTparamater * param = dynamic_cast <ASTparamater *> (arg.get ());
				if (param)
					variables [param->getIden ()] = param->getVariable ();
			}
			pair.first->assignBody (parseBody (end));
			variables = oldVariables;
		}
	}
	
}

ASTnode AbstractSyntaxTree::createArrayType (const ASTnode& dataType, size_t dimentions) {
	
	string key = dataType->toString () + " []";
	shared_ptr <NumbatType> nbtype;
	auto arrType = types.find (key);
	
	if (arrType == types.end ()) {
		std::set <string> tags;
		tags.insert ("export");
		ASTnode rawDataType = ASTnode (new ASTtype (false, false, dataType->getType ()));
		nbtype = types [key] = shared_ptr <NumbatType> (new NumbatPointerType (key, rawDataType, tags));
		ASTnode type = ASTnode (new ASTtype (false, false, generateRawType ("raw 64", 64, std::set <string> ())));
		ASTnode param = ASTnode (new ASTparamater (shared_ptr <NumbatVariable> (new NumbatVariable (type, "length"))));
		nbtype->buildData (std::vector <ASTnode> (1, param));
	} else {
		nbtype = arrType->second;
	}
	
	return ASTnode (new ASTtype (dataType->isAlias (), dataType->isConst (), nbtype));
	
}

ASTnode AbstractSyntaxTree::createBinaryCall (const string & func, const ASTnode & lhs, const ASTnode & rhs, tkitt end, defBinaryImp defImp) {
	
	const ASTtuple * tupleLhs = dynamic_cast <const ASTtuple *> (lhs.get ());
	const ASTtuple * tupleRhs = dynamic_cast <const ASTtuple *> (rhs.get ());
	//TODO: default implementations for tuples
	
	if (tupleLhs and tupleRhs) {
		
		if (tupleLhs->getElements ().size () != tupleRhs->getElements ().size ()) {
			error ("Binary operation has an unequal number of arguments.", end);
			return ASTnode (new ASTerror ("Argument length mismatch"));
		}
		
		auto lhsItt = tupleLhs->getElements ().begin ();
		auto rhsItt = tupleRhs->getElements ().begin ();
		auto lhsEnd = tupleLhs->getElements ().end ();
		auto rhsEnd = tupleRhs->getElements ().end ();
		
		std::vector <shared_ptr <ASTcallable>> calls;
		std::vector <ASTnode> args (2);
		
		std::list <ASTnode> lhsArgs, rhsArgs;
		
		for (; lhsItt != lhsEnd and rhsItt != rhsEnd; ++lhsItt, ++rhsItt) {
			args [0] = *lhsItt;
			args [1] = *rhsItt;
			shared_ptr <ASTcallable> call = findFunction (func, args);
			calls.push_back (call);
			if (!call->isValid ()) {
				error (call->toString (), end);
				continue;
			}
			lhsArgs.push_back (createStaticCast (args [0], call->getFunction ()->getArgs () [0]));
			rhsArgs.push_back (createStaticCast (args [1], call->getFunction ()->getArgs () [1]));
		}
		
		return ASTnode (new ASTtuplecall (func, calls, lhsArgs, rhsArgs));
		
	} else if (tupleLhs) {
		
		std::vector <ASTnode> args (2);
		
		if (rhs->isCallable ()) {
			
			std::vector <shared_ptr <ASTcallable>> calls;
			shared_ptr <ASTcallable> rhsFunc = std::static_pointer_cast <ASTcallable> (rhs);
			std::list <ASTnode> lhsArgs, rhsArgs;
			size_t i=0;
			
			for (const ASTnode & lvar : tupleLhs->getElements ()) {
				
				if (!lvar->isNil ()) {
					args [0] = lvar;
					args [1] = ASTnode (new ASTcallindex (rhsFunc, i));;
					shared_ptr <ASTcallable> call = findFunction (func, args);
					calls.push_back (call);
					if (!call->isValid ()) {
						error (call->toString (), end);
						continue;
					}
					lhsArgs.push_back (createStaticCast (args [0], call->getFunction ()->getArgs () [0]));
					rhsArgs.push_back (createStaticCast (args [1], call->getFunction ()->getArgs () [1]));
				}
				++i;
				
			}
			
			if (lhsArgs.empty ()) {
				return rhs;
			} else {
				return ASTnode (new ASTtuplecall (func, calls, lhsArgs, rhsArgs));
			}
			
		} else {
			error ("Composite binary tuple operations are unfortunetly not yet supported.", end);
			return ASTnode (new ASTerror ("Type mismatch"));
		}
		
	} else if (tupleRhs) {
		 
		error ("Composite binary tuple operations are unfortunetly not yet supported.", end);
		return ASTnode (new ASTerror ("Type mismatch"));
		
	} else {
		
		if (!lhs->isValid () or !rhs->isValid ()) {
			if (!lhs->isValid ()) {
				error (lhs->toString (), end);
			}
			if (!rhs->isValid ()) {
				error (rhs->toString (), end);
			}
			return ASTnode (new ASTerror ("Invalid expression"));
		}
		
		ASTnode tlhs = lhs;
		ASTnode trhs = rhs;
		
		if (lhs->isCallable ()) {
			tlhs = ASTnode (new ASTcallindex (std::dynamic_pointer_cast <ASTcallable> (lhs), 0));
		}
		if (rhs->isCallable ()) {
			trhs = ASTnode (new ASTcallindex (std::dynamic_pointer_cast <ASTcallable> (rhs), 0));
		}
		
		std::vector <ASTnode> args (2);
		args [0] = tlhs;
		args [1] = createStaticCast (trhs, tlhs);
		auto fbeg = functions.lower_bound (func), fend = functions.upper_bound (func);
		std::vector <shared_ptr <FunctionDecleration>> cands;
		while (fbeg != fend) cands.push_back (fbeg->second), ++fbeg;
		shared_ptr <ASTcallable> call =  parser::findBestMatch (this, args, cands, 0);
		if (!call->isValid () and defImp) {
			if (!tlhs->getType ()) return tlhs;
			if (!trhs->getType ()) return trhs;
			return defImp (this, func, tlhs, trhs, end);
		} else {
			return call;
		}
		
	}
	
}

ASTnode AbstractSyntaxTree::createCallNode (const shared_ptr <ASTcallable> & callee, const std::vector <ASTnode> & args) {
	if (!callee or !callee->isValid ()) return ASTnode (callee);
	ASTnode node (new ASTcall (callee, createStaticCast (args, callee->getFunction ()->getArgs (), 1)));
	if (!node->isValid ()) buildFail = true;
	return node;
}

ASTnode AbstractSyntaxTree::createStaticCast (const ASTnode & arg, const ASTnode & type, int maxDepth) {
	
	if (!arg->getType () or !type->getType ()) {
		return ASTnode (new ASTerror ("Invalid type"));
	}
	
	if (arg->getType () == type->getType ()) {
		/*if (arg->isAlias () and !type->isAlias ()) {
			return ASTnode (new ASTnumbatInstr ("load", std::vector <ASTnode> (1, arg), type));
		}*/
		//TODO: type modifier considerations
		return arg;
	}
	
	if (arg->getType ()->isRaw () and type->getType ()->isRaw ()) {
		if (arg->getType ()->getBitSize () != type->getType ()->getBitSize () or arg->getType ()->isFloat () != type->getType ()->isFloat ()) {
			std::vector <ASTnode> args;
			args.push_back (arg); args.push_back (type);
			return ASTnode (new ASTnumbatInstr ("cast", args, type));
		} else {
			return arg;
		}
	}
	
	if (0 < maxDepth) {
		
		shared_ptr <ASTcallable> func = parser::findBestMatch (this, std::vector <ASTnode> (1, arg), type->getType ()->getConstructors (), maxDepth-1);
		if (func->isValid ()) {
			return ASTnode (new ASTcallindex (func, 0));
		}
		
	}
	
	return ASTnode (new ASTerror ("No sutible conversion found '" + type->getType ()->getIden () + "' required, found '" + arg->getType ()->getIden () + "'"));
	
}

ASTnode AbstractSyntaxTree::createTuple (const ASTnode & lhs, const ASTnode & rhs) {
	
	ASTnode ret = nullptr;
	const ASTtuple * tupleLhs = dynamic_cast <const ASTtuple *> (lhs.get ());
	const ASTtuple * tupleRhs = dynamic_cast <const ASTtuple *> (rhs.get ());
	
	if (tupleLhs) {
		if (tupleRhs) {
			ret = ASTnode (new ASTtuple (tupleLhs->getElements (), tupleRhs->getElements ()));
		} else {
			ret = ASTnode (new ASTtuple (tupleLhs->getElements (), rhs));
		}
	} else if (tupleRhs) {
		ret = ASTnode (new ASTtuple (lhs, tupleRhs->getElements ()));
	} else {
		ret = ASTnode (new ASTtuple (lhs, rhs));
	}
	return ret;
	
}

//TODO: remove this feature
ASTnode AbstractSyntaxTree::parseAssembly (const string & type, const string & code) {
	
	tkitt oldItt = itt;
	tkstring instr = lex (code);
	itt = instr.begin();
	tkitt end = instr.end ();
	ASTnode node;
	
	if (itt->type == TOKEN::indent) {
		nextToken (end);
	}
	
	if (type == "numbat") {
		
		string instr = itt->iden;
		nextToken (end);
		auto args = parseArgs (&AbstractSyntaxTree::parsePrimaryExpression, end);
		node = ASTnode (new ASTnumbatInstr (instr, args, args [0]));
		
	} else {
		
		error ("Unidentified assembly type: '" + type + "'", end);
		node = ASTnode (new ASTerror ("Unidentified assembly type"));
		
	}
	
	itt = oldItt;
	return node;
	
}

ASTnode AbstractSyntaxTree::parseBody (tkitt end) {
	auto oldVariables = variables;
	size_t level = indentLevel;
	eatSemicolon (end);
	tkitt bend;
	if (itt->iden == "{") {
		nextToken (end);
		bend = findToken ("}", end);
		if (bend == end) {
			itt = findIndent (level, end);
			error ("Expected '}'", end);
		}
	} else {
		bend = findIndent (level, end);
	}
	
	std::vector <ASTnode> exprs;
	while (itt != bend) {
		auto func = statementParsers.find (itt->iden);
		ASTnode exp = nullptr;
		if (func != statementParsers.end ()) {
			exp = func->second (this, bend);
		} else {
			exp = parser::parseExpression (this, bend);
		}
		exprs.push_back (exp);
	}
	nextToken (end);
	eatSemicolon (end);
	variables = oldVariables;
	return ASTnode (new ASTbody (exprs));
}

ASTnode AbstractSyntaxTree::parseExpression (tkitt end) {
	
	if (itt == end) {
		error ("Expected primary expression", end);
		return ASTnode (new ASTerror ("null"));
	}
	
	if (itt->type == TOKEN::whitespace) nextToken (end);
	
	auto matches = generateOperatorMatches (end);
	return parseExpression (matches, end);
	
}

ASTnode AbstractSyntaxTree::parseExpression (std::list <OperatorDecleration::OperatorMatch> & matches, tkitt end, const std::vector <ASTnode> * args) {
	
	if (itt->type == TOKEN::assembly) {
		nextToken (end);
		if (itt->type != TOKEN::chararrayliteral); //TODO: panic!
		string type = itt->iden;
		nextToken (end);
		nextToken (end); // eat ':'
		if (itt->type != TOKEN::chararrayliteral); //TODO: panic!
		string code = itt->iden; 
		nextToken (end);
		/*std::vector <ASTnode> args;
		if (itt->iden == "(") {
			nextToken (end);
			args = parseArgs (&AbstractSyntaxTree::parseExpression, findToken (")", end));
			nextToken (end);
		}*/
		return parseAssembly (type, code);
	}
	
	if (args) {
		for (auto arg : *args) {
			if (!arg->isValid ()) return ASTnode (new ASTerror ("Bad arguments"));
		}
	}
	
	if (matches.size () == 0) return parsePrimaryExpression (end, args);
	
	matches.sort (&OperatorDecleration::OperatorMatch::treeOrder);
	
	OperatorDecleration::OperatorMatch match;
	ASTnode node = nullptr;
	
	while (!node and matches.size ()) {
		match = matches.front ();
		matches.pop_front ();
		node = parseOperator (*match.opp, matches, match.ptr, end, args);
	}
	
	if (node == nullptr) {
		return parsePrimaryExpression (end, args);
	}
	
	return node;
	
}

ASTnode AbstractSyntaxTree::parseNumericliteral (tkitt end) {
	
	ASTnode num;
	if (itt->iden.back () == 'f') {
		num = ASTnode (new ASTconstantFPInt (ASTnode (new ASTtype (false, true, types ["float"])), itt->iden));
	} else if (itt->iden.back () == 'h') {
		num = ASTnode (new ASTconstantFPInt (ASTnode (new ASTtype (false, true, types ["half"])), itt->iden));
	} else if (itt->iden.back () == 'q') {
		num = ASTnode (new ASTconstantFPInt (ASTnode (new ASTtype (false, true, types ["quad"])), itt->iden));
	} else if (itt->iden.find ('.') != string::npos) {
		num = ASTnode (new ASTconstantFPInt (ASTnode (new ASTtype (false, true, types ["double"])), itt->iden));
	} else {
		size_t l = std::stoull (itt->iden);
		num = ASTnode (new ASTconstantInt (ASTnode (new ASTtype (false, true, types ["uint64"])), l));
	}
	
	nextToken (end);
	return num;
	
}

ASTnode AbstractSyntaxTree::parseOperator (const OperatorDecleration & opp, std::list <OperatorDecleration::OperatorMatch> & matches, tkitt matchPtr, tkitt end, const std::vector <ASTnode> * prevArgs) {
	
	std::vector <tkitt> oppLoc;
	const std::vector <string> & symb = opp.getSymbols ();
	
	if (symb.size () < 1) {
		error ("Invalid operator", end);
		return ASTnode (new ASTerror ("Invalid operator"));
	}
	
	auto sItt = symb.begin (), sEnd = symb.end ();
	if (symb [0] == " ") {
		//std::cerr << "'" << matchPtr->iden << "'" << std::endl;
		++sItt;
		auto found = findLastToken (symb [0], itt, matchPtr);
		if (found == matchPtr) return nullptr;
	}
	oppLoc.push_back (matchPtr);
	auto found = matchPtr+1;
	for (++sItt; sItt < sEnd; ++sItt) {
		found = findToken (*sItt, found, end);
		if (*sItt != " ")
			oppLoc.push_back (found);
		if (found == end) return nullptr;
		++found;
	}
	
	if (symb.back () != " ") {
		if (findToken (" ", oppLoc.back () + 1, end) < end) return nullptr;
	}
	
	matches.sort (&OperatorDecleration::OperatorMatch::parseOrder);
	
	return opp.parse (this, oppLoc, matches, end);
	
}

ASTnode AbstractSyntaxTree::parseParameter (tkitt end) {
	
	tkitt assign = findToken ("=", end);
	ASTnode var = parseStatment (assign);
	ASTnode value = nullptr;
	
	if (assign != end) {
		itt = assign;
		nextToken (end);
		value = parseStatment (end);
	}
	nextToken (end);
	
	if (!var->isValid ()) {
		error (var->toString (), end);
		return var;
	}
	
	ASTvariable * avar = dynamic_cast <ASTvariable *> (var.get ());
	
	if (avar) {
		return ASTnode (new ASTparamater (avar->getVariable()));
	} else {
		error ("Incomplete parameter", end);
		return var;
	}
	
}

ASTnode AbstractSyntaxTree::parsePrimaryExpression (tkitt end, const std::vector <ASTnode> * args) {
	
	if (itt->type == TOKEN::whitespace) nextToken (end);
	
	if (itt->type == TOKEN::symbol and itt->iden == "(") {
		nextToken (end);
		auto ret = parseExpression (findToken (")", end));
		itt = end;
		return ret;
	}
	
	if (itt->type == TOKEN::chararrayliteral) {
		//TODO: return char array ASTnode
		nextToken (end);
		return ASTnode (new ASTerror ("NYI"));
	} else if (itt->type == TOKEN::numericliteral) {
		//TODO: deal with float and double literals
		return parseNumericliteral (end);
	} else if (itt->type == TOKEN::stringliteral) {
		const string & str = parseString (end);
		return ASTnode (new ASTconstantCString (ASTnode (new ASTtype (false, true, types ["string"])), str));
	} else if (itt->type == TOKEN::nil) {
		nextToken (end);
		return ASTnode (new ASTnil ());
	}
	
	if (itt->type == TOKEN::raw or itt->type == TOKEN::typemodifier or types.find (itt->iden) != types.end ()) {
		ASTnode type = parseType (end);
		if (itt != end) {
			return ASTnode (new ASTvariable (variables [itt->iden] = std::shared_ptr <NumbatVariable> (new NumbatVariable (type, itt->iden))));
		} else {
			return type;
		}
	}
	
	const string & iden = itt->iden;
	nextToken (end);
	return resolveSymbol (iden, nullptr);
	
}

ASTnode AbstractSyntaxTree::parseStatment (tkitt end) {
	
	tkstring reParse;
	
	tkitt beg = itt;
	
	token tkn;
	while (beg != end) {
		
		//std::cerr << "'" << beg->iden << "' " << oppTokens.count (beg->iden) << std::endl;
		if (tkn.iden == "") {
			if (beg->type == TOKEN::whitespace) {
			} else if (oppTokens.count (beg->iden) == 0) {
				reParse += *beg;
			} else {
				tkn = *beg;
			}
		} else if (beg->type == TOKEN::whitespace or beg->type == TOKEN::indent or oppTokens.count (tkn.iden + beg->iden) == 0) {
			reParse += tkn;
			tkn.iden = "";
			continue;
		} else {
			tkn.iden += beg->iden;
		}
		
		++beg;
	}
	if (tkn.iden != "") reParse += tkn;
	tkn.iden = "eof";
	tkn.type = TOKEN::eof;
	reParse += tkn;
	
	/*tkitt tk = reParse.begin ();
	tkitt tkend = reParse.end ();
	for (; tk != tkend; ++tk) {
		std::cerr << "'" << tk->iden << "' ";
	}
	std::cerr << std::endl;*/
	
	itt = reParse.begin ();
	tkitt rpend = lexer::findEOF (reParse.begin (), reParse.end ());
	ASTnode node;
	
	if (itt->type == TOKEN::ret) {
		nextToken (end);
		if (itt == rpend) {
			node = ASTnode (new ASTreturnvoid ());
		} else {
			node = ASTnode (new ASTreturn (parseExpression (rpend)));
		}
	} else {
		node = parseExpression (rpend);
	}
	itt = end;
	return node;
	
}

ASTnode AbstractSyntaxTree::parseType (tkitt end) {
	
	bool constTkn=false, ref=false;
	while (itt->type == TOKEN::typemodifier) {
		if (itt->iden == "ref") {
			ref = true;
		} else if (itt->iden == "const") {
			constTkn = true;
		}
		nextToken (end);
	}
	
	string iden = itt->iden;
	if (itt->type == TOKEN::raw) {
		nextToken (end); // eat 'raw' token
		string key = "raw " + itt->iden;
		size_t size=0;
		if (itt->type == TOKEN::numericliteral) {
			size = parseIntLiteral (end);
		} else {
			//TODO: handle variable sized raw data
		}
		std::set <string> metaTags = parseMetaTags (end);
		return ASTnode (new ASTrawdata (ref, constTkn, generateRawType (key, size, metaTags), metaTags));
		
	} else if (itt->type == TOKEN::identifier) {
		nextToken (end); // eat identifier
		// TODO: parse template arguments
		// TODO: parse meta tags
		string key = iden;
		// TODO: mutate key to the correct key for templates: key = findType (iden, template)
		auto itt = types.find (key);
		if (itt == types.end ()) {
			error ("'" + iden + "' is not a type", end);
			return ASTnode (new ASTerror ("Invalid type"));
		}
		return ASTnode (new ASTtype (ref, constTkn, itt->second));
		
	} else {
		error ("Expected type", end);
		return ASTnode (new ASTerror ("Invalid type"));
		
	}
	
}

ASTnode AbstractSyntaxTree::resolveSymbol (const string & iden, ASTnode parent) {
	
	ASTnode ret;
	if (parent) {
		
		if (const ASTmodule * mod = dynamic_cast <const ASTmodule *> (parent->getASTType ().get ())) {
			
			auto typ = mod->getModule ()->getTypes ().find (iden);
			if (typ != mod->getModule ()->getTypes ().end ()) {
				ret = ASTnode (new ASTtype (false, false, typ->second));
			} else {
			
				std::vector <shared_ptr <FunctionDecleration>> funcs;
				auto range = mod->getModule ()->getFunctions ().equal_range (iden);
				for (auto f = range.first; f!=range.second; ++f) {
					if (!f->second->hasTag ("local")) {
						funcs.push_back (f->second);
					}
				}
				if (!funcs.empty ()) {
					ret = ASTnode (new ASTfunctionlist (iden, funcs));
				} else {
					ret = ASTnode (new ASTerror ("'" + iden + "' is undefined"));
				}
			
			}
			
		} else if (parent->getType ()) {
			
			int index = parent->getType ()->findMember (iden);
			
			if (index >= 0) {
				ret = ASTnode (new ASTstructIndex (index, parent));
			} else {
				//TODO: search for apropriate functions
				ret = ASTnode (new ASTerror ("Member functions are not yet implemented"));
			}
			
		} else {
			
			ret = ASTnode (new ASTerror ("Invalid parent"));
			
		}
		
	} else {
		
		auto var = variables.find (iden);
		if (var != variables.end ()) {
			ret = ASTnode (new ASTvariable (var->second));
		} else {
			auto typ = types.find (iden);
			if (typ != types.end ()) {
				ret = ASTnode (new ASTtype (false, false, typ->second));
			} else {
				std::vector <shared_ptr <FunctionDecleration>> funcs = getFunctionList (iden);
				if (!funcs.empty ()) {
					ret = ASTnode (new ASTfunctionlist (iden, funcs));
				} else {
					ret = ASTnode (new ASTerror ("'" + iden + "' is undefined"));
				}
			}
		}
		
	}
	return ret;
	
}

bool AbstractSyntaxTree::flushLine (tkitt end) {
	while (itt != end and itt->type != TOKEN::semicolon) {
		++itt;
	}
	return nextToken (end);
}

bool AbstractSyntaxTree::nextToken (tkitt end) {
	if (itt != end) {
		++itt;
		while (itt->type == TOKEN::indent or itt->type == TOKEN::whitespace) {
			if (itt == end) return false;
			if (itt->type == TOKEN::indent)
				indentLevel = itt->iden.size ();
			++itt;
		}
		line = itt->line;
		//std::cerr << itt->iden << std::endl;
		return itt != end;
	}
	return false;
}

bool AbstractSyntaxTree::parseFunctionPrototype (std::vector <ASTnode> & args, string & iden, tkitt end) {
	
	if (/*itt->type == TOKEN::identifier*/true) {
		
		iden = itt->iden;
		nextToken (end);
		if (itt->iden != "(") {
			error ("'(' expected after '" + iden + "'", end);
			return false;
		}
		nextToken (end);
		tkitt argEnd = findToken (")", end);
		args = parseArgs (& AbstractSyntaxTree::parseParameter, argEnd);
		
	} else {
		
		iden = "";
		do {
			if (itt->type == TOKEN::chararrayliteral) {
				iden += itt->iden;
			} else if (itt->iden == "(") {
				iden += ' ';
				nextToken (end); // eat bracket
				std::vector <ASTnode> temp = parseArgs (& AbstractSyntaxTree::parseParameter, findToken (")", end));
				args.insert (args.end (), temp.begin (), temp.end ());
			} else {
				error ("Invalid function prototype", end);
				return false;
			}
		} while (nextToken (end));
		
		if (operators.find (iden) == operators.end ()) {
			error ("Use of undeclared operator", end);
			return false;
		}
		
	}
	
	nextToken (end);
	return true;
	
}

FunctionDecleration * AbstractSyntaxTree::parseFunctionDecleration (tkitt end) {
	
	nextToken (end); // eat def token
	std::set <string> metaTags;
	std::vector <ASTnode> args;
	std::vector <ASTnode> type;
	std::vector <ASTnode> templateArgs;
	string iden;
	FunctionDecleration * decl = nullptr;
	
	bool success = true;
	
	if (itt->iden == "[") {
		nextToken (end);
		//TODO: parse template args
		itt = findToken ("]", end);
		nextToken (end);
	}
	
	iden = itt->iden;
	nextToken (end);
	
	metaTags = parseMetaTags (end);
	
	if (itt->iden == "(") {
		nextToken (end);
		tkitt argEnd = findToken (")", end);
		args = parseArgs (& AbstractSyntaxTree::parseParameter, argEnd);
		nextToken (end);
	} else {
		//TODO: fail
	}
	
	if (itt->iden == "-" and (nextToken (end), itt->iden == ">")) {
		nextToken (end);
		if (itt->iden == "(") {
			nextToken (end);
			tkitt argEnd = findToken (")", end);
			type = parseArgs (&AbstractSyntaxTree::parseType, argEnd);
			nextToken (end);
		} else {
			//TODO: fail
		}
	}
	
	if (success) {
		decl = new FunctionDecleration (iden, args, type, metaTags);
		shared_ptr <FunctionDecleration> fptr (decl);
		functions.insert (std::make_pair (iden, fptr));
		auto type = types.find (iden);
		if (type != types.end ()) {
			type->second->addConstructor (fptr);
		}
	}
	
	return decl;
	
}

OperatorDecleration::OperatorMatch AbstractSyntaxTree::matchFromRight (const string & pattern, tkitt rend, tkitt rbeg) {
	
	OperatorDecleration::OperatorMatch match;
	
	return match;
	
}

shared_ptr <NumbatType> AbstractSyntaxTree::generateRawType (const string & key, size_t size, const std::set <string> & metaArgs) {
	
	string newKey = key;
	for (const string & arg : metaArgs) newKey += " @" + arg;
	auto itt = types.find (newKey);
	shared_ptr <NumbatType> type;
	if (itt == types.end ()) {
		NumbatRawType::Type rawType = NumbatRawType::SIGNED;
		if (metaArgs.count ("fint")) {
			rawType = NumbatRawType::FLOAT;
		} else if (metaArgs.count ("sint")) {
			rawType = NumbatRawType::SIGNED;
		} else if (metaArgs.count ("uint")) {
			rawType = NumbatRawType::UNSIGNED;
		}
		type = shared_ptr <NumbatType> (new NumbatRawType (key, size, rawType));
		types [newKey] = type;
	} else {
		type = itt->second;
	}
	return type; 
	
}

shared_ptr <ASTcallable> AbstractSyntaxTree::findFunction (const string & iden, const std::vector <ASTnode> & args, int convert) {
	
	auto beg = functions.lower_bound (iden), end = functions.upper_bound (iden);
	std::list <std::pair <uint32_t, shared_ptr <FunctionDecleration>>> found;
	
	std::vector <ASTnode> expandedArgs = expandNodes (args);
	
	while (beg != end) {
		if (beg->second->getArgs ().size () == expandedArgs.size ()) {
			bool fail=false;
			uint32_t score=0; // lower is better
			auto funcBegin = beg->second->getArgs ().begin ();
			auto funcEnd = beg->second->getArgs ().end ();
			auto argBegin = expandedArgs.begin ();
			auto argEnd = expandedArgs.end ();
			for (uint32_t count=0; funcBegin != funcEnd and argBegin != argEnd; ++funcBegin, ++argBegin, ++count) {
				if (convert > 0 and (*funcBegin)->getType () != (*argBegin)->getType () and (*funcBegin)->getType () and (*argBegin)->getType ()) {
					
					bool raw = (*funcBegin)->getType ()->isRaw () and (*argBegin)->getType ()->isRaw ();
					if (raw and (*funcBegin)->getType ()->getBitSize () == (*argBegin)->getType ()->getBitSize () and (*funcBegin)->getType ()->isFloat () == (*argBegin)->getType ()->isFloat ()) {
					} else if ((*funcBegin)->isAlias () and !(*funcBegin)->isConst ()) {
						fail = true;
					} else if (raw) {
						score += 10000 - count;
					} else if (findFunction ((*funcBegin)->getType ()->getIden (), std::vector <ASTnode> (1, *argBegin), convert - 1)->isValid ()) {
						score += 10000 - count;
					} else {
						fail = true;
					}
				}
			}
			if (!fail) {
				found.push_back (std::make_pair (score, beg->second));
			}
		}
		++beg;
	}
	
	found.sort ();
	
	if (found.size ()) {
		return shared_ptr <ASTcallable> (new ASTfunctionPointer (found.front ().second));
	}
	
	string arglist;
	for (auto arg : args) {
		if (arglist != "") {
			arglist += ", ";
		}
		if (arg->getType ()) {
			arglist += arg->getType ()->getIden ();
		} else {
			arglist += "NO TYPE";
		}
	}
	return shared_ptr <ASTcallable> (new ASTcallerror ("No sutible conversion for function: '" + iden + "' (" + arglist + ")"));
	
}

size_t AbstractSyntaxTree::parseIntLiteral (tkitt end) {
	size_t ret=0;
	try {
		ret = std::stoull (itt->iden);
		nextToken (end);
	} catch (std::invalid_argument e) {
		error ("Integer expected", end);
	}
	return ret;
}

std::list <OperatorDecleration::OperatorMatch> AbstractSyntaxTree::generateOperatorMatches (tkitt end) {
	
	std::list <OperatorDecleration::OperatorMatch> matches;
	int brace = 0;
	bool skip = false;
	
	std::list <std::pair <OperatorDecleration::OperatorMatch, size_t>> candidates, remove;
	
	for (tkitt tkn=itt, prev=itt, next=itt+1; tkn!=end; tkn=next, prev=tkn, ++next) {
		
		if (tkn->type == TOKEN::symbol) {
			if (tkn->iden == ")" or tkn->iden == "]" or tkn->iden == "}")
				--brace;
			if (0 > brace) break;
		}
		
		if (!brace) {
			
			auto oppBeg = operatorsByFirstToken.lower_bound (tkn->iden.substr (0, 1));
			auto oppEnd = operatorsByFirstToken.upper_bound (tkn->iden.substr (0, 1));
			for (; oppBeg != oppEnd; ++oppBeg) {
				OperatorDecleration::OperatorMatch match;
				match.opp = oppBeg->second;
				match.ptr = tkn;
				candidates.push_back (std::make_pair (match, oppBeg->second->getPattern ().find_first_not_of (" ")));
			}
			
			for (auto & cand : candidates) {
				
				const string & ptn = cand.first.opp->getPattern ();
				const string & sym = tkn->iden;
				
				if (ptn [cand.second] == ' ') {
					size_t len = std::min (ptn.size () - cand.second - 1, sym.size ());
					int cmp = sym.compare (0, len, ptn, cand.second + 1,  len);
					if (!cmp) {
						cand.second += len + 1;
					}
				} else if (tkn->type != TOKEN::chararrayliteral and tkn->type != TOKEN::stringliteral) {
					size_t len = std::min (ptn.size () - cand.second, sym.size ());
					int cmp = sym.compare (0, len, ptn, cand.second, len);
					if (cmp) {
						remove.push_back (cand);
						continue;
					} else {
						cand.second += len;
					}
				} else {
					remove.push_back (cand);
					continue;
				}
				if (cand.second >= ptn.size () or (cand.second + 1 >= ptn.size () and ptn.back () == ' ')) {
					matches.push_back (cand.first);
					remove.push_back (cand);
				}
				
			}
			for (auto & r : remove)
				candidates.remove (r);
			
			remove.clear ();
			
			
		} else {
			//if (!candidates.empty ())
			//	candidates.clear ();
		}
		
		if (tkn->type == TOKEN::symbol) {
			if (tkn->iden == "(" or tkn->iden == "[" or tkn->iden == "{")
				++brace;
		}
		
	}
	
	return matches;
	
}

std::vector <ASTnode> AbstractSyntaxTree::createCallIndexList (const shared_ptr <ASTcallable> & call) {
	
	std::vector <ASTnode> list (call->getFunction ()->getType ().size ());
	for (size_t i=0; i<list.size (); ++i) list [i] = ASTnode (new ASTcallindex (call, i));
	return list;
	
}

std::vector <ASTnode> AbstractSyntaxTree::createStaticCast (const std::vector <ASTnode> & args, const std::vector <ASTnode> & types, int maxDepth) {
	
	std::vector <ASTnode> result (types.size ());
	auto argItt = args.begin (), argEnd = args.end (), typeItt = types.begin (), typeEnd = types.end ();
	for (auto resItt=result.begin (), resEnd=result.end (); typeItt != typeEnd and resItt != resEnd; ++argItt) {
		if (argItt == argEnd) {
			//TODO: do something?
		} else {
			if (!(*argItt)->isValid ()) {
				*resItt = *argItt;
				++resItt, ++typeItt;
			} else if ((*argItt)->isCallable ()) {
				std::vector <ASTnode> indexing = createCallIndexList (std::dynamic_pointer_cast <ASTcallable> (*argItt));
				for (const ASTnode & index : indexing) {
					*resItt = createStaticCast (index, *typeItt, maxDepth);
					++resItt, ++typeItt;
				}
			} else {
				*resItt = createStaticCast (*argItt, *typeItt, maxDepth);
				++resItt, ++typeItt;
			}
		}
	}
	return result;
	
}

std::vector <ASTnode> AbstractSyntaxTree::parseArgs (ASTnode (AbstractSyntaxTree::*arg)(tkitt), tkitt end) {
	std::vector <ASTnode> args;
	tkitt expEnd;
	while (itt != (expEnd = findToken (",", end))) {
		args.push_back ((this->*arg)(expEnd));
		nextToken (end);
	}
	return args;
}

std::vector <ASTnode> AbstractSyntaxTree::parseTemplateArgs (tkitt end) {
	nextToken (end);
	std::vector <ASTnode> args;
	if (itt->iden != "<") {
		error ("'<' expected after '$'", end);
	} else {
		nextToken (end);
		tkitt argEnd = findToken (">", end);
		args = parseArgs (& AbstractSyntaxTree::parseParameter, argEnd);
		nextToken (end);
	}
	return args;
}

std::vector <shared_ptr <FunctionDecleration>> AbstractSyntaxTree::getFunctionList (const string & iden) {
	auto func_beg = functions.lower_bound (iden);
	auto func_end = functions.upper_bound (iden);
	std::vector <shared_ptr <FunctionDecleration>> funcs;
	while (func_beg != func_end) {
		funcs.push_back (func_beg->second);
		++func_beg;
	}
	return funcs;
}

string AbstractSyntaxTree::parseString (tkitt end) {
	
	string str;
	bool escaped = false;
	for (char c : itt->iden) {
		if (escaped) {
			switch (c) {
				case 'a':
					str += '\a';
					break;
				case 'b':
					str += '\b';
					break;
				case 'f':
					str += '\f';
					break;
				case 'n':
					str += '\n';
					break;
				case 'r':
					str += '\r';
					break;
				case 't':
					str += '\t';
					break;
				case 'v':
					str += '\v';
					break;
				case '\\':
					str += '\\';
					break;
				case '\'':
					str += '\'';
					break;
				case '\"':
					str += '\"';
					break;
				case '?':
					str += '?';
					break;
			}
			escaped = false;
		} else if (c == '\\') {
			escaped = true;
		} else {
			str += c;
		}
	}
	nextToken (end);
	return str;
	
}

string AbstractSyntaxTree::parseStructDecleration (tkitt end) {
	
	nextToken (end); // eat struct token
	
	if (itt->type != TOKEN::identifier) {
		error ("Identifier expected", end);
		return "";
	}
	
	string iden = itt->iden;
	nextToken (end); // eat identifier
	
	std::vector <ASTnode> templateArgs;
	if (itt->iden == "$") {
		templateArgs = parseTemplateArgs (end);
	}
	
	std::set <string> meta = parseMetaTags (end);
	
	if (types.find (iden) != types.end ()) {
		error ("Struct already declared", end);
	} else {
		types [iden] = unique_ptr <NumbatType> (new NumbatType (iden, meta));
	}
	
	return iden;
	
}

tkitt AbstractSyntaxTree::findBlockEnd (tkitt end) {
	
	int level=0;
	tkitt tkn = itt;
	while (tkn != end) {
		if (tkn->type == TOKEN::end) {
			if (!level) {
				break;
			} else {
				--level;
			}
		} else if (tkn->type == TOKEN::def or statementParsers.find (tkn->iden) != statementParsers.end ()) {
			++level;
		}
		++tkn;
	}
	return tkn;
	
}

tkitt AbstractSyntaxTree::findIndent (const int level, tkitt end) {
	auto found = itt;
	while (indentLevel > level and found != end) {
		++found;
		while (found->type == TOKEN::indent) {
			indentLevel = found->iden.size ();
			++found;
			if (found == end) return end;
		}
	}
	return found;
}

tkitt AbstractSyntaxTree::findLastToken (const string & token, tkitt tmpitt, tkitt end) {
	
	if (tmpitt == end) return end;
	tkitt trueEnd = end;
	int brace=0;
	//std::cerr << "'" << token << "'" << std::endl;
	bool findIden = token == " ";
	//std::cerr << "'" << token << "'" << std::endl;
	do {
		--end;
		if (!brace) {
			if (findIden) {
				switch (end->type) {
					case TOKEN::chararrayliteral:
					case TOKEN::identifier:
					case TOKEN::numericliteral:
					case TOKEN::stringliteral:
						return end;
					case TOKEN::symbol:
						//std::cerr << "'" << end->iden << "'" << std::boolalpha << (end->type == TOKEN::symbol) << std::boolalpha << (end->iden == ")") << std::endl;
						if (end->iden == ")")
							return findLastToken ("(", tmpitt, end);
						if (end->iden == "]")
							return findLastToken ("[", tmpitt, end);
						if (end->iden == "}")
							return findLastToken ("{", tmpitt, end);
					default:
						break;
				}
			} else if (end->iden == token and end->type != TOKEN::chararrayliteral and end->type != TOKEN::stringliteral) {
				return end;
			}
		}
		
		if (end->type == TOKEN::symbol) {
			if (end->iden == "(" or end->iden == "[" or end->iden == "{")
				++brace;
			else if (end->iden == ")" or end->iden == "]" or end->iden == "}")
				--brace;
		}
	} while (tmpitt != end);
	return trueEnd;
	
}

tkitt AbstractSyntaxTree::findToken (const string & token, tkitt tmpitt, tkitt end) {
	
	int brace=0;
	bool findIden = token == " ";
	while (tmpitt != end) {
		
		if (!brace) {
			if (findIden) {
				switch (tmpitt->type) {
					case TOKEN::chararrayliteral:
					case TOKEN::identifier:
					case TOKEN::numericliteral:
					case TOKEN::stringliteral:
						return tmpitt;
					case TOKEN::symbol:
						if (tmpitt->iden == "(")
							return findToken (")", tmpitt+1, end);
						if (tmpitt->iden == "[")
							return findToken ("]", tmpitt+1, end);
						if (tmpitt->iden == "{")
							return findToken ("}", tmpitt+1, end);
					default:
						break;
				}
			} else if (tmpitt->iden == token and tmpitt->type != TOKEN::chararrayliteral and tmpitt->type != TOKEN::stringliteral) {
				return tmpitt;
			}
		}
		
		if (tmpitt->type == TOKEN::symbol) {
			if (tmpitt->iden == "(" or tmpitt->iden == "[" or tmpitt->iden == "{")
				++brace;
			else if (tmpitt->iden == ")" or tmpitt->iden == "]" or tmpitt->iden == "}")
				--brace;
		}
		++tmpitt;
	}
	return tmpitt;
	
}

void AbstractSyntaxTree::addOperator (const string & pattern, const OperatorDecleration & oppdec) {
	
	if (operators.find (pattern) != operators.end ()) {
		return;
	}
	
	shared_ptr <OperatorDecleration> opp (new OperatorDecleration (oppdec));
	
	operators [pattern] = opp;
	precidenceOrderedOperators.insert (opp);
	
	switch (oppdec.getType ()) {
		case OperatorDecleration::TYPE::array:
		case OperatorDecleration::TYPE::index_call:
		case OperatorDecleration::TYPE::ternary:
		{
			size_t first = pattern.find_first_not_of (' ');
			size_t last = pattern.find_first_of (first, ' ');
			parenOpperators.insert (pattern.substr (first, last - first));
		}
		default:
			break;
	}
	
	for (const string & s : opp->getSymbols ()) {
		if (s != " ") {
			operatorsByFirstToken.insert (std::make_pair (s, opp));
			break;
		}
	}
	
	string nakedPattern = "";
	for (char c : pattern) {
		if (c != ' ') nakedPattern += c;
	}
	oppTokens.insert (nakedPattern);
	
}

void AbstractSyntaxTree::importModule (const shared_ptr <Module> & module, bool extention) {
	
	dependencies.insert (module);
	for (auto opp : module->getOperators ()) {
		addOperator (opp.first, *opp.second.get ());
	}
	for (auto func : module->getFunctions ()) {
		if (func.second->hasTag ("export") or func.second->hasTag ("global") or extention) {
			functions.insert (func);
		}
	}
	for (auto type : module->getTypes ()) {
		if (type.second->hasTag ("export") or type.second->hasTag ("global") or extention) {
			types [type.first] = type.second;
		}
	}
	for (auto stmt : module->getStatmentParsers()) {
		statementParsers [stmt.first] = stmt.second;
	}
	
}

void AbstractSyntaxTree::parseEnum (tkitt end) {
	
	error ("Enums not yet implemented", end);
	return;
	/*nextToken (end);//eat 'enum' token
	string iden = itt->iden;
	nextToken (end);
	std::set <string> meta = parseMetaTags (end);
	ASTnode type;
	
	if (itt->type == TOKEN::colon) {
		nextToken (end);
		type = resolveSymbol (itt->iden);
	} else {
		type = resolveSymbol ("uint32");
	}
	
	std::vector <ASTnode> members;
	if (itt->iden == "{") {
		nextToken (end);
		//members = parseArgs (& AbstractSyntaxTree::parseParameter, findToken ("}", end));
	}
	
	types [iden] = shared_ptr <NumbatType> (new NumbatEnumType (iden, type->getType (), meta));
	types [iden]->buildData (members);*/
	
}

void AbstractSyntaxTree::parseImport(tkitt end) {
	
	nextToken (end);//eat 'import' token
	shared_ptr <Module> module = nullptr;
	string name = itt->iden;
	
	if (itt->type == TOKEN::chararrayliteral) {
		
		module = Module::createFromFile (path, name);
		nextToken (end);
		
	} else if (itt->type == TOKEN::identifier) {
		
		module = Module::import (path, name);
		nextToken (end);
		
	}
	
	if (module) {
	
		ASTnode type = ASTnode (new ASTmodule (module));
		if (itt->type == TOKEN::as) {
			
			nextToken (end);//eat 'as' token
			variables [itt->iden] = shared_ptr <NumbatVariable> (new NumbatVariable (type, itt->iden));
			nextToken (end);
			
		} else {
			variables [name] = shared_ptr <NumbatVariable> (new NumbatVariable (type, itt->iden));
		}
		
		importModule (module, false);
		
	} else {
		flushLine (end);
	}
	eatSemicolon (end);
	
}

void AbstractSyntaxTree::parseOperatorDecleration (tkitt end) {
	
	error ("Operator declerations are no longer supported", end);
	return;
	
}

void AbstractSyntaxTree::parseTypeDef (tkitt end) {
	
	nextToken (end); // eat typedef
	ASTnode type = parseStatment (findToken (":", end));
	nextToken (end);
	string iden = itt->iden;
	nextToken (end);
	eatSemicolon (end);
	
	if (types.find (iden) == types.end ()) {
		if (type->isValid ()) {
			types [iden] = type->getType ();
		}
	} else {
		error ("", end);
	}
	
}


std::vector <ASTnode> expandNodes (const std::vector <ASTnode> & args) {
	
	std::vector <ASTnode> res;
	for (const ASTnode & arg : args) {
		if (arg->isCallable ()) {
			//std::cerr << arg->toString () << std::endl;
			const ASTcallable * call = reinterpret_cast <const ASTcallable *> (arg.get ());
			if (call->isValid ()) {
				for (const ASTnode & type : call->getFunction ()->getType ()) {
					res.push_back (type);
				}
			}
		} else if (arg->isList ()) {
			//TODO: expand list
		} else {
			res.push_back (arg);
		}
	}
	return res;
	
}

void splitListAboutTkn (std::list <OperatorDecleration::OperatorMatch> & lhs, std::list <OperatorDecleration::OperatorMatch> & source, tkitt itt) {
	
	OperatorDecleration::OperatorMatch match;
	match.ptr = itt;
	
	auto lower = std::lower_bound (source.begin (), source.end (), match, &OperatorDecleration::OperatorMatch::parseOrder);
	
	source.splice (lhs.begin (), lhs, source.begin (), lower);
	
	auto upper = std::upper_bound (source.begin (), source.end (), match, &OperatorDecleration::OperatorMatch::parseOrder);
	source.erase (source.begin (), upper);
	
}


};
};
