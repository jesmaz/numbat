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
		
		const nir::Instruction * recreate (const std::vector <Argument> & replacmentArgs) const {abort ();}
		
		const std::vector <Value> & getValues () const {return values;}
		
		Constant (const std::vector <const Type *> & types, const std::vector <Value> & values, const std::vector <symbol> & idens) : Instruction ({}, types, idens), values (values) {}
		
	protected:
	private:
		
		std::vector <Value> values;
		
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
