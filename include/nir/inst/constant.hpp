#pragma once


#include "../instruction.hpp"
#include "../forward.hpp"

#include <memory>
#include <string>


namespace nir {

using std::string;

class Constant : public Instruction {
	CONST_VISITABLE
	public:
		
		struct Data {
			
			public:
				
				enum TYPE {
					DOUBLE,
					INT,
					STRING,
					UINT
				};
				
				const string & asString () const {assert (type == STRING); return *str;}
				double asDouble () const {return d;}
				int64_t asInt () const {return i;}
				TYPE getType () const {return type;}
				uint64_t asUint () const {return u;}
				
				Data (double d) : d (d), type (DOUBLE) {}
				Data (int64_t i) : i (i), type (INT) {}
				Data (const string & s) : str (new string (s)), type (STRING) {}
				Data (uint64_t u) : u (u), type (UINT) {}
				
				
				Data (const Data &)=delete;
				const Data & operator = (const Data &)=delete;
				
				~Data () {
					if (type == STRING) delete str;
				}
				
			private:
				
				union {
					double d;
					int64_t i;
					uint64_t u;
					string * str;
				};
				
				TYPE type;
				
		};
		
		bool validate () const {return true;}
		
		const nir::Instruction * recreate (const std::vector <Argument> & replacmentArgs) const {abort ();}
		
		const std::vector <std::unique_ptr <Data>> & getData () const {return data;}
		
		template <typename T>
		void push_back (const T & t) {data.push_back (std::unique_ptr <Data> (new Data (t)));}
		
		Constant (const std::vector <const Type *> & types, const std::vector <symbol> & idens) : Instruction ({}, types, idens) {}
		
	protected:
	private:
		
		std::vector <std::unique_ptr <Data>> data;
		
		friend Scope;
		
		virtual string strDump (text::PrintMode mode) const {
			
			string s;
			for (const auto & d : data) {
				switch (d->getType ()) {
					case Data::DOUBLE:
						s += std::to_string (d->asDouble ()) + " ";
						break;
					case Data::INT:
						s += std::to_string (d->asInt ()) + " ";
						break;
					case Data::STRING:
						s += d->asString () + " ";
						break;
					case Data::UINT:
						s += std::to_string (d->asUint ()) + " ";
						break;
				}
			}
			if (not s.empty ()) s.pop_back ();
			return s;
			
		}
		
};


};