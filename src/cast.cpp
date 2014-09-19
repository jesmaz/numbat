#include "../include/ast/asterror.hpp"
#include "../include/ast/astnumbatinstr.hpp"
#include "../include/ast/callable/astcall.hpp"
#include "../include/ast/callable/astcallerror.hpp"
#include "../include/ast/callable/astfunctionpointer.hpp"
#include "../include/ast/memory/astcallindex.hpp"
#include "../include/ast/type/numbattype.hpp"
#include "../include/cast.hpp"

namespace numbat {
namespace parser {


ASTnode createStaticCast (const ASTnode & arg, const ASTnode & type, int maxDepth) {
	
	if (!arg->getType () or !type->getType ()) {
		return ASTnode (new ASTerror ("Invalid type"));
	}
	
	if (arg->getType () == type->getType ()) {
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
		shared_ptr <ASTcallable> func = findBestMatch (std::vector <ASTnode> ({arg}), type->getType ()->getMethods (type->getType ()->getIden ()), maxDepth-1);
		if (func->isValid ()) {
			return ASTnode (new ASTcallindex (func, 0));
		}

	}
	
	string s = "No suitable conversion, '" + type->getType ()->getIden () + "' required, found '" + arg->getType ()->getIden () + "'";
	return ASTnode (new ASTerror (s));
	
}

shared_ptr <ASTcallable> findBestMatch (const std::vector <ASTnode> & args, const std::vector <FunctionDecleration *> & candidates, int maxDepth) {
	
	FunctionDecleration * func = nullptr;
	std::vector <ASTnode> params (args);
	size_t weight = __UINT64_MAX__ - 1;
	for (auto & fdef : candidates) {
		if (fdef->getArgs ().size () == args.size ()) {
			std::vector <ASTnode> cast = createStaticCast (args, fdef->getArgs (), maxDepth);
			size_t w = 0;
			for (const ASTnode & node : cast) {
				if (node->isValid ()) {
					w += node->calculateWeight ();
				} else {
					w = __UINT64_MAX__;
					break;
				}
			}
			if (w < weight) {
				weight = w;
				func = fdef;
				params = cast;
			}	
		}
	}
	if (func) {
		return shared_ptr <ASTcallable> (new ASTcall (shared_ptr <ASTcallable> (new ASTfunctionPointer (func)), params));
	} else {
		string err = "\n\tTarget is: (";
		for (auto & arg : args) {
			if (!arg->isValid ()) {
				return shared_ptr <ASTcallable> (new ASTcallerror (arg->toString ()));
			}
			err += arg->getType ()->getIden () + ", ";
		}
		err += ")\n\tCandidates are:";
		for (auto & fdef : candidates) {
			err += "\n\t\t(";
			for (auto & arg : fdef->getArgs ()) {
				err += arg->getType ()->getIden () + ", ";
			}
			err += ")";
		}
		return shared_ptr <ASTcallable> (new ASTcallerror (err));
	}
	
}

std::vector <ASTnode> createStaticCast (const std::vector <ASTnode> & args, const std::vector <ASTnode> & types, int maxDepth) {
	
	std::vector <ASTnode> result (types.size ());
	auto argItt = args.begin (), argEnd = args.end (), typeItt = types.begin (), typeEnd = types.end ();
	for (auto resItt=result.begin (), resEnd=result.end (); typeItt != typeEnd and resItt != resEnd; ++argItt) {
		if (argItt == argEnd) {
			*resItt = ASTnode (new ASTerror (""));
			++resItt, ++typeItt;
		} else {
			if (!(*argItt)->isValid ()) {
				*resItt = *argItt;
				++resItt, ++typeItt;
			} else {
				*resItt = createStaticCast (*argItt, *typeItt, maxDepth);
				if (!(*resItt)->isValid () and (*argItt)->isList ()) {
					for (const ASTnode & index : (*argItt)->getList ()) {
						*resItt = createStaticCast (index, *typeItt, maxDepth);
						++resItt, ++typeItt;
					}
				} else {
					++resItt, ++typeItt;
				}
			}
		}
	}
	return result;
	
}


}
}