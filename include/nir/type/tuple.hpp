#pragma once


#include <nir/type.hpp>
#include <set>


namespace nir {


class Tuple : public Type {
	CONST_VISITABLE
	public:
		
		virtual size_t calculateSize (size_t ptrSize) const;
		virtual ssize_t calculateOffset (size_t ptrSize, size_t index) const;
		
		const BasicArray <const Type *> & getMemberArr () const {return *memberArr;}
		
		void replaceTuple (const BasicArray <const Type *> & args);
		
		static const Tuple * getTuple (const BasicArray <const Type *> & args);
		static Tuple * newTuple ();
		
	protected:
	private:
		
		Tuple () {}
		
		const BasicArray <const Type *> * memberArr;
		
		virtual string strDump (text::PrintMode mode) const;
		
		struct CmpPtr {bool operator ()(const BasicArray <const Type *> * const& a, const BasicArray <const Type *> * const& b) const {return *a < *b;}};
		static std::set <const BasicArray <const Type *> *, CmpPtr> tuples;
		static std::set <const Tuple *> tPtrs;
		
};


};
