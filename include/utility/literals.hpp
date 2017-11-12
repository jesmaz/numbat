#pragma once


#include <gmpxx.h>
#include <utility/array.hpp>
#include <utility/text.hpp>


struct Literal;

struct literal_virtual_table {
	bool (*op_eq) (const Literal &, const Literal &);
	bool (*op_lt) (const Literal &, const Literal &);
	bool (*op_lte) (const Literal &, const Literal &);
	bool (*op_not) (const Literal &);
	
	Literal (*conv_double) (const Literal &);
	Literal (*conv_int64) (const Literal &);
	Literal (*conv_uint64) (const Literal &);
	Literal (*conv_aint) (const Literal &);
	
	Literal (*op_add) (const Literal &, const Literal &);
	Literal (*op_band) (const Literal &, const Literal &);
	Literal (*op_bor) (const Literal &, const Literal &);
	Literal (*op_bxor) (const Literal &, const Literal &);
	Literal (*op_concat) (const Literal &, const Literal &);
	Literal (*op_div) (const Literal &, const Literal &);
	Literal (*op_index) (const Literal &, size_t);
	Literal (*op_mul) (const Literal &, const Literal &);
	Literal (*op_sub) (const Literal &, const Literal &);
	
	void (*assign) (Literal &, const Literal &);
	void (*copy_ctr) (Literal &, const Literal &);
	
	void (*destroy) (Literal &);
	
	static literal_virtual_table type_nil, type_array, type_array_index, type_fint64, type_fint32, type_int64, type_aint0, type_uint64;
};

struct Literal {
	
	public:
		
		bool isNil () const {return vTable == &literal_virtual_table::type_nil;}
		
		bool operator == (const Literal & rhs) const {return vTable->op_eq (*this, rhs);}
		bool operator != (const Literal & rhs) const {return not vTable->op_eq (*this, rhs);}
		bool operator < (const Literal & rhs) const {return vTable->op_lt (*this, rhs);}
		bool operator <= (const Literal & rhs) const {return vTable->op_lte (*this, rhs);}
		bool operator > (const Literal & rhs) const {return not vTable->op_lte (*this, rhs);}
		bool operator >= (const Literal & rhs) const {return not vTable->op_lt (*this, rhs);}
		bool operator not () const {return vTable->op_not (*this);}
		
		Literal concat (const Literal & other) const {return vTable->op_concat (*this, other);}
		Literal operator + (const Literal & other) const {return vTable->op_add (*this, other);}
		Literal operator | (const Literal & other) const {return vTable->op_bor (*this, other);}
		Literal operator & (const Literal & other) const {return vTable->op_band (*this, other);}
		Literal operator ^ (const Literal & other) const {return vTable->op_bxor (*this, other);}
		Literal operator / (const Literal & other) const {return vTable->op_div (*this, other);}
		Literal operator [] (size_t i) const {return vTable->op_index (*this, i);}
		Literal operator * (const Literal & other) const {return vTable->op_mul (*this, other);}
		Literal operator - (const Literal & other) const {return vTable->op_sub (*this, other);}
		
		std::string toString (text::PrintMode mode) const;
		
		mpq_class to_aint0 (bool * success=nullptr) const;
		double to_double (bool * success=nullptr) const;
		int64_t to_int64 (bool * success=nullptr) const;
		uint64_t to_uint64 (bool * success=nullptr) const;
		
		void destroy () {vTable->destroy (*this); vTable = &literal_virtual_table::type_nil; uint64 = 0;}
		
		Literal () : vTable (&literal_virtual_table::type_nil), uint64 (0) {}
		Literal (const Literal & rhs) {rhs.vTable->copy_ctr (*this, rhs);}
		Literal (double val) : vTable (&literal_virtual_table::type_fint64), fint64 (val) {}
		Literal (float val) : vTable (&literal_virtual_table::type_fint32), fint32 (val) {}
		Literal (int8_t val) : vTable (&literal_virtual_table::type_int64), int64 (val) {}
		Literal (int16_t val) : vTable (&literal_virtual_table::type_int64), int64 (val) {}
		Literal (int32_t val) : vTable (&literal_virtual_table::type_int64), int64 (val) {}
		Literal (int64_t val) : vTable (&literal_virtual_table::type_int64), int64 (val) {}
		Literal (uint8_t val) : vTable (&literal_virtual_table::type_uint64), uint64 (val) {}
		Literal (uint16_t val) : vTable (&literal_virtual_table::type_uint64), uint64 (val) {}
		Literal (uint32_t val) : vTable (&literal_virtual_table::type_uint64), uint64 (val) {}
		Literal (uint64_t val) : vTable (&literal_virtual_table::type_uint64), uint64 (val) {}
		Literal (const mpq_class & val) : vTable (&literal_virtual_table::type_aint0), aint0 (new mpq_class (val)) {}
		Literal (const BasicArray <Literal> & val) : vTable (&literal_virtual_table::type_array), array (new ArrayRef {val, 1}) {}
		
		const Literal & operator = (const Literal & rhs) {vTable->assign (*this, rhs); return *this;}
		~Literal () {vTable->destroy (*this);}
		
	protected:
	private:
		
		friend literal_virtual_table;
		
		struct ArrayRef {
			BasicArray <Literal> data;
			size_t owners;
		};
		struct ArrayIndex {
			ArrayRef * array;
			size_t index;
		};
		
		literal_virtual_table * vTable;
		union {
			ArrayIndex * arr_index;
			ArrayRef * array;
			double fint64;
			float fint32;
			int64_t int64;
			mpq_class * aint0;
			uint64_t uint64;
		};
		
};
