#pragma once


#include <memory>
#include <nir/instruction.hpp>
#include <nir/forward.hpp>
#include <nir/value.hpp>
#include <string>


namespace nir {

using std::string;

class Constant : public Instruction {
	CONST_VISITABLE
	public:
		
		bool validate () const {return true;}
		
		const nir::Instruction * recreate (const BasicArray <Argument> & replacmentArgs) const {abort ();}
		
		const BasicArray <Value> & getValues () const {return values;}
		
		Constant (const BasicArray <const Type *> & types, const BasicArray <Value> & values, const BasicArray <symbol> & idens) : Instruction ({}, types, idens), values (values) {}
		
	protected:
	private:
		
		BasicArray <Value> values;
		
		friend Scope;
		
		virtual string strDump (text::PrintMode mode) const {
			string s;
			for (const auto & v : values) {
				s += " " + v->toString (mode);
			}
			return s;
		}
		
};


};
