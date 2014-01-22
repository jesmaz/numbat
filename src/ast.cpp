
#include "../include/ast.hpp"

namespace numbat {
using namespace lexer;
namespace parser {

/*std::map <string, shared_ptr <OperatorDecleration>> AbstractSyntaxTree::operators;
std::multimap <string, shared_ptr <OperatorDecleration>> AbstractSyntaxTree::operatorsByFirstToken;
std::set <shared_ptr <OperatorDecleration>, std::greater <shared_ptr <OperatorDecleration>>> AbstractSyntaxTree::precidenceOrderedOperators;
std::unordered_set <string> AbstractSyntaxTree::parenOpperators, AbstractSyntaxTree::oppTokens, AbstractSyntaxTree::ternaryStart;*/


AbstractSyntaxTree::AbstractSyntaxTree (tkitt beg, tkitt end) {
	
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
					itt = findIndent (oldIndent, end);
				}
				break;
				
			case TOKEN::externdef:
				nextToken (end);
				parseFunctionDecleration (end);
				eatSemicolon (end);
				break;
				
			case TOKEN::import:
				parseImport (end);
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
				
			default:
				error ("Unexpected token '" + itt->iden + "'", end);
				break;
				
		}
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

ASTnode AbstractSyntaxTree::createBinaryCall (const string & func, const ASTnode & lhs, const ASTnode & rhs, tkitt end) {
	
	const ASTtuple * tupleLhs = dynamic_cast <const ASTtuple *> (lhs.get ());
	const ASTtuple * tupleRhs = dynamic_cast <const ASTtuple *> (rhs.get ());
	
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
				printError (call->toString ());
			}
			lhsArgs.push_back (createStaticCast (args [0], call->getFunction ()->getArgs () [0]));
			rhsArgs.push_back (createStaticCast (args [1], call->getFunction ()->getArgs () [1]));
		}
		
		return ASTnode (new ASTtuplecall (func, calls, lhsArgs, rhsArgs));
		
	} else if (tupleLhs or tupleRhs) {
		 
		error ("Composite binary tuple operations are unfortunetly not yet supported.", end);
		return ASTnode (new ASTerror ("Type mismatch"));
		
	} else {
		
		std::vector <ASTnode> args (2);
		args [0] = lhs;
		args [1] = rhs;
		return createCallNode (findFunction (func, args), args);
		
	}
	
}

ASTnode AbstractSyntaxTree::createCallNode (const shared_ptr <ASTcallable> & callee, const std::vector <ASTnode> & args) {
	if (!callee->isValid ()) return ASTnode (callee);
	ASTnode node (new ASTcall (callee, createStaticCast (args, callee->getFunction ()->getArgs (), 1)));
	if (!node->isValid ()) buildFail = true;
	return node;
}

ASTnode AbstractSyntaxTree::createStaticCast (const ASTnode & arg, const ASTnode & type, int maxDepth) {
	
	if (arg->getType () == type->getType ()) {
		//TODO: type modifier considerations
		return arg;
	}
	
	if (arg->getType ()->isRaw () and type->getType ()->isRaw ()) {
		std::vector <ASTnode> args;
		args.push_back (arg); args.push_back (type);
		return ASTnode (new ASTnumbatInstr ("cast", args));
	}
	
	if (maxDepth > 1) {
	
		ASTnode func;
		
		const string & iden = type->getType ()->getIden ();
		auto beg = functions.lower_bound (iden), end = functions.upper_bound (iden);
		std::list <std::pair <uint32_t, ASTnode>> found;
		
		while (beg != end) {
			if (beg->second->getArgs ().size () == 1) {
				const ASTnode & funcarg = beg->second->getArgs () [0];
				func = createStaticCast (arg, funcarg, maxDepth-1);
				if (func->isValid ()) {
					func = ASTnode (new ASTcall (shared_ptr <ASTcallable> (new ASTfunctionPointer (beg->second)), std::vector <ASTnode> (1, func)));
					found.push_back (std::make_pair (func->calculateWeight (), func));
				}
			}
			++beg;
		}
		
		if (found.size ()) {
			return found.front ().second;
		}
		
	}
	
	return ASTnode (new ASTerror ("No sutible conversion found."));
	
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
		node = ASTnode (new ASTnumbatInstr (instr, args));
		
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
	end = findIndent (level, end);
	tkitt scolon;
	std::vector <ASTnode> exprs;
	while ((scolon = findToken (";", end)) != end) {
		ASTnode node = parseStatment (scolon);
		eatSemicolon (end);
		if (node) {
			exprs.push_back (node);
		}
	}
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
	auto found = matchPtr;
	for (++sItt; sItt < sEnd; ++sItt) {
		found = findToken (*sItt, found, end);
		if (*sItt != " ")
			oppLoc.push_back (found);
		if (found == end) return nullptr;
	}
	
	matches.sort (&OperatorDecleration::OperatorMatch::parseOrder);
	ASTnode callee = nullptr;
	std::vector <ASTnode> args;
	
	if (prevArgs)
		args = *prevArgs;
	
	switch (opp.getType ()) {
		case OperatorDecleration::TYPE::array:
			itt = oppLoc [0] + 1;
			args = parseArgs (&AbstractSyntaxTree::parseExpression, oppLoc [1]);
			break;
			
		case OperatorDecleration::TYPE::binary:
			{
				std::list <OperatorDecleration::OperatorMatch> lhs;
				splitListAboutTkn (lhs, matches, oppLoc [0]);
				args.push_back (parseExpression (lhs, oppLoc [0]));
				itt = oppLoc [0] + 1;
				
				if (opp.getPattern () == " . ") {
					ASTnode expr = parseExpression (matches, end, &args);
					if (expr->isCallable ()) {
						callee = expr;
					} else {
						return expr;
					}
				} else if (opp.getPattern () == " , ") {
					return createTuple (args [0], parseExpression (matches, end));
				} else {
					return createBinaryCall (opp.getPattern (), args [0], parseExpression (matches, end), end);
				}
			}
			break;
			
		case OperatorDecleration::TYPE::index_call:
			{
				tkitt tmpItt = itt;
				itt = oppLoc [0] + 1;
				args = parseArgs (&AbstractSyntaxTree::parseExpression, oppLoc [1]);
				itt = tmpItt;
				callee = parseExpression (matches, oppLoc [0], &args);
				itt = oppLoc [1];
			}
			break;
			
		case OperatorDecleration::TYPE::ternary:
			{
				std::list <OperatorDecleration::OperatorMatch> lhs, mid;
				splitListAboutTkn (lhs, matches, oppLoc [0]);
				args.push_back (parseExpression (lhs, oppLoc [0]));
				itt = oppLoc [0] + 1;
				splitListAboutTkn (mid, matches, oppLoc [1]);
				args.push_back (parseExpression (mid, oppLoc [1]));
				itt = oppLoc [1] + 1;
				args.push_back (parseExpression (matches, oppLoc [2]));
			}
			break;
			
		case OperatorDecleration::TYPE::unarypostfix:
			args.push_back (parseExpression (matches, oppLoc [0]));
			break;
			
		case OperatorDecleration::TYPE::unaryprefix:
			itt = oppLoc [0] + 1;
			args.push_back (parseExpression (matches, end));
			break;
			
		default:
			error ("Invalid operator type", end);
			
	}
	
	
	shared_ptr <ASTcallable> callable;
	
	if (!callee) {
		callable = findFunction (opp.getPattern (), args);
	} else {
		callable = std::dynamic_pointer_cast <ASTcallable> (callee);
	}
	
	return createCallNode (callable, args);
	
}

ASTnode AbstractSyntaxTree::parseParameter (tkitt end) {
	ASTnode type = parseType (end);
	string iden = itt->iden;
	if (itt->type != TOKEN::identifier) {
		error ("Expected identifier", end);
		return nullptr;
	}
	nextToken (end); // eat identifier
	return ASTnode (new ASTparamater (variables [iden] = std::shared_ptr <NumbatVariable> (new NumbatVariable (type, iden))));
}

ASTnode AbstractSyntaxTree::parsePrimaryExpression (tkitt end, const std::vector <ASTnode> * args) {
	
	if (itt->type == TOKEN::whitespace) nextToken (end);
	
	if (itt->type == TOKEN::symbol and itt->iden == "(") {
		nextToken (end);
		return parseExpression (findToken (")", end));
	}
	
	if (itt->type == TOKEN::chararrayliteral) {
		//TODO: return char array ASTnode
		nextToken (end);
		return ASTnode (new ASTerror ("NYI"));
	} else if (itt->type == TOKEN::numericliteral) {
		//TODO: deal with float and double literals
		string key = "raw 64";
		size_t amount = parseIntLiteral (end);
		std::set <string> meta; meta.insert ("sint");
		ASTnode type = ASTnode (new ASTtype (false, true, generateRawType (key, 64, meta)));
		return ASTnode (new ASTconstantInt (type, amount));
	} else if (itt->type == TOKEN::stringliteral) {
		//TODO: return string ASTnode
		nextToken (end);
		return ASTnode (new ASTerror ("NYI"));
	}
	
	if (itt->type == TOKEN::raw or itt->type == TOKEN::typemodifier or types.find (itt->iden) != types.end ()) {
		ASTnode type = parseType (end);
		return ASTnode (new ASTvariable (variables [itt->iden] = std::shared_ptr <NumbatVariable> (new NumbatVariable (type, itt->iden))));
	}
	
	if (args and args->size () == 1 and args->front ()->getType ()) {
		std::cerr << args->front ()->toString (" --> ") << std::endl;
		std::cerr << args->front ()->getType ()->toString (" --> ") << std::endl;
		int index = args->front ()->getType ()->findMember (itt->iden);
		if (index >= 0) {
			nextToken (end);
			return ASTnode (new ASTstructIndex (index, args->front ()));
		}
	}
	
	if (args and args->size () and args->front ()->getType ()) {
		ASTnode node = findFunction (itt->iden, *args);
		if (node->isValid ()) {
			nextToken (end);
			return node;
		}
	}
	
	auto var = variables.find (itt->iden);
	if (var != variables.end ()) {
		nextToken (end);
		return ASTnode (new ASTvariable (var->second));
	}
	
	if (args) {
		ASTnode node = findFunction (itt->iden, *args);
		if (!node->isValid ()) {
			error (node->toString (), end);
		}
		nextToken (end);
		return node;
	}
	
	error ("undefined variable '" + itt->iden + "'", end);
	nextToken (end);
	return ASTnode (new ASTerror ("NYI"));
	
}

ASTnode AbstractSyntaxTree::parseStatment (tkitt end) {
	
	tkstring reParse;
	
	tkitt beg = itt;
	
	token tkn;
	while (beg != end) {
		
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
	
	itt = reParse.begin ();
	ASTnode node;
	
	if (itt->type == TOKEN::ret) {
		nextToken (end);
		if (itt == reParse.end ()) {
			node = ASTnode (new ASTreturnvoid ());
		} else {
			node = ASTnode (new ASTreturn (parseExpression (reParse.end ())));
		}
	} else {
		node = parseExpression (reParse.end ());
	}
	itt = end;
	return node;
	
}

ASTnode AbstractSyntaxTree::parseType (tkitt end) {
	
	bool constTkn=false, alias=false;
	while (itt->type == TOKEN::typemodifier) {
		if (itt->iden == "alias") {
			alias = true;
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
		return ASTnode (new ASTrawdata (alias, constTkn, generateRawType (key, size, metaTags), metaTags));
		
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
		return ASTnode (new ASTtype (alias, constTkn, itt->second));
		
	} else {
		error ("Expected type", end);
		return ASTnode (new ASTerror ("Invalid type"));
		
	}
	
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
	
	iden = itt->iden;
	nextToken (end);
	
	if (itt->iden == "$") {
		templateArgs = parseTemplateArgs (end);
	}
	
	metaTags = parseMetaTags (end);
	
	if (itt->iden != "(") {
		error ("'(' expected after '" + iden + "'", end);
		success = false;
	} else {
		nextToken (end);
		tkitt argEnd = findToken (")", end);
		args = parseArgs (& AbstractSyntaxTree::parseParameter, argEnd);
		nextToken (end);
	}
	//bool success = parseFunctionPrototype (args, iden, findToken (":", end));
	
	auto found = functions.find (iden);
	if (found != functions.end ()) {
		if (found->second->getArgs () == args) {
			error ("Function already declared", end);
			success = false;
		}
	}
	
	if (success) {
		nextToken (end); // eat ':' token
		
		if (itt->iden == "{") {
			nextToken (end); // eat '{' token
			tkitt argEnd = findToken ("}", end);
			type = parseArgs (&AbstractSyntaxTree::parseType, argEnd);
			nextToken (end); // eat '}' token
			
			if (metaTags.count ("cstyle") and type.size () > 1) {
				error ("Function is decleared cstyle but has multiple reuturn values", end);
			}
			
		}
		
		decl = new FunctionDecleration (iden, args, type, metaTags);
		functions.insert (std::make_pair (iden, unique_ptr <FunctionDecleration> (decl)));
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
				if (convert > 0 and (*funcBegin)->getType () != (*argBegin)->getType ()) {
					if ((*funcBegin)->getType ()->isRaw () and (*argBegin)->getType ()->isRaw ()) {
						score += 10000 - count;
					} else if (findFunction ((*funcBegin)->getType ()->getIden (), std::vector <ASTnode> (1, *argBegin), convert - 1)->isValid ()) {
						score += 10000 - count;
					} else {
						//fail = true;
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
	
	return shared_ptr <ASTcallable> (new ASTcallerror ("No function found"));
	
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
	
	for (tkitt tkn=itt, prev=itt, next=itt+1; tkn!=end; tkn=next, prev=tkn, ++next) {
		
		if (brace == 0) {
			
			auto oppBeg = operatorsByFirstToken.lower_bound (tkn->iden);
			auto oppEnd = operatorsByFirstToken.upper_bound (tkn->iden);
			
			while (oppBeg != oppEnd) {
			
				//bool valid=true;
				OperatorDecleration::OperatorMatch match;
				match.opp = oppBeg->second;
				match.ptr = tkn;
				matches.push_back (match);
				++oppBeg;
			
			}
			
		} else if (brace < 0) {
			break;
		}
		
		if (tkn->type == TOKEN::symbol) {
			if (tkn->iden == "(" or tkn->iden == "[" or tkn->iden == "{")
				++brace;
			else if (tkn->iden == ")" or tkn->iden == "]" or tkn->iden == "}")
				--brace;
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
			//std::cerr << (*argItt)->toString () << " : " << std::boolalpha << (*argItt)->isCallable () << std::endl;
			if ((*argItt)->isCallable ()) {
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
	//TODO: parse meta tags
	
	if (functions.find (iden) != functions.end ()) {
		error ("Struct already declared", end);
	} else {
		types [iden] = unique_ptr <NumbatType> (new NumbatType (iden));
	}
	
	return iden;
	
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
			} else if (end->iden == token) {
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
			} else if (tmpitt->iden == token) {
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

void AbstractSyntaxTree::parseImport(tkitt end) {
	nextToken (end);//eat 'import' token
	
}

void AbstractSyntaxTree::parseOperatorDecleration (tkitt end) {
	
	int precidence;
	bool ltr = false, neg = false;
	std::string pattern;
	tkitt decEnd = findToken (":", end);
	
	while (nextToken (decEnd)) {
		switch (itt->type) {
			case TOKEN::identifier:
				pattern += " ";
				break;
			case TOKEN::chararrayliteral:
				pattern += itt->iden;
				break;
			default:
				error ("Unexpected token: '" + itt->iden + "', expected char array or identifier", decEnd);
		}
	}
	
	nextToken (end); // eat ':' token
	
	if (itt->iden == "-") {
		neg = true;
		nextToken (end); // eat '-' token
	}
	
	if (itt->type != TOKEN::numericliteral) {
		error ("Expected integer", end);
		return;
	}
	
	precidence = parseIntLiteral (end);
	
	if (neg) precidence = -precidence;
	
	if (itt->iden == "ltr") {
		ltr = true;
	} else if (itt->iden == "rtl") {
		ltr = false;
	} else {
		error ("Expected an associativity (either 'ltr' or 'rtl')", end);
		return;
	}
	
	nextToken (end); // eat associativity
	OperatorDecleration oppdec (precidence, ltr, pattern);
	
	if (oppdec.getType () == OperatorDecleration::TYPE::INVALID) {
		error ("Malformed operator", end);
		return;
	}
	
	if (operators.find (pattern) != operators.end ()) {
		error ("Operator aready defined", end);
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
			oppTokens.insert (s);
			operatorsByFirstToken.insert (std::make_pair (s, opp));
		}
	}
	
}

void AbstractSyntaxTree::parseTypeDef (tkitt end) {
	
	nextToken (end); // eat typedef
	ASTnode type = parseType (findToken (":", end));
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
