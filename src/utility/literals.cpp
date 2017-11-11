#include <utility/literals.hpp>

literal_virtual_table literal_virtual_table::type_nil = {
	
	// op_eq
	[](const Literal &, const Literal &) {return false;},
	// op_lt
	[](const Literal &, const Literal &) {return false;},
	// op_lte
	[](const Literal &, const Literal &) {return false;},
	// op_not
	[](const Literal &) {return false;},
	
	// conv_double
	[](const Literal &) {return Literal ();},
	// conv_int64
	[](const Literal &) {return Literal ();},
	// conv_uint64
	[](const Literal &) {return Literal ();},
	// conv_aint
	[](const Literal &) {return Literal ();},
	
	// op_add
	[](const Literal &, const Literal &) {return Literal ();},
	// op_band
	[](const Literal &, const Literal &) {return Literal ();},
	// op_bor
	[](const Literal &, const Literal &) {return Literal ();},
	// op_bxor
	[](const Literal &, const Literal &) {return Literal ();},
	// op_concat
	[](const Literal &, const Literal &) {return Literal ();},
	// op_div
	[](const Literal &, const Literal &) {return Literal ();},
	// op_index
	[](const Literal &, size_t) {return Literal ();},
	// op_mul
	[](const Literal &, const Literal &) {return Literal ();},
	// op_sub
	[](const Literal &, const Literal &) {return Literal ();},
	
	// copy_ctr
	[](Literal & lhs, const Literal &) {lhs.vTable = &type_nil; lhs.uint64 = 0;},
	// destroy
	[](Literal &) {}
	
}, literal_virtual_table::type_array = {
	
	// op_eq
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_array == rhs.vTable) {
			return lhs.array->data == rhs.array->data;
		}
		return false;
	},
	// op_lt
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_array == rhs.vTable) {
			return lhs.array->data < rhs.array->data;
		}
		return false;
	},
	// op_lte
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_array == rhs.vTable) {
			return lhs.array->data <= rhs.array->data;
		}
		return false;
	},
	// op_not
	[](const Literal &) {return false;},
	
	// conv_double
	[](const Literal &) {return Literal ();},
	// conv_int64
	[](const Literal &) {return Literal ();},
	// conv_uint64
	[](const Literal &) {return Literal ();},
	// conv_aint
	[](const Literal &) {return Literal ();},
	
	// op_add
	[](const Literal &, const Literal &) {return Literal ();},
	// op_band
	[](const Literal &, const Literal &) {return Literal ();},
	// op_bor
	[](const Literal &, const Literal &) {return Literal ();},
	// op_bxor
	[](const Literal &, const Literal &) {return Literal ();},
	// op_concat
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_array == rhs.vTable) {
			Literal l;
			l.vTable = &type_array;
			l.array = new Literal::ArrayRef {
				BasicArray <Literal> (lhs.array->data.size () + rhs.array->data.size ()),
				1
			};
			auto itt = std::copy (lhs.array->data.begin (), lhs.array->data.end (), l.array->data.begin ());
			std::copy (rhs.array->data.begin (), rhs.array->data.end (), itt);
			return l;
			
		}
		return Literal ();
	},
	// op_div
	[](const Literal &, const Literal &) {return Literal ();},
	// op_index
	[](const Literal & self, size_t i) {
		Literal l;
		l.vTable = &type_array_index;
		l.arr_index = new Literal::ArrayIndex {self.array, i};
		self.array->owners += 1;
		return l;
	},
	// op_mul
	[](const Literal &, const Literal &) {return Literal ();},
	// op_sub
	[](const Literal &, const Literal &) {return Literal ();},
	
	// copy_ctr
	[](Literal & lhs, const Literal & rhs) {
		lhs.vTable = rhs.vTable;
		lhs.array = new Literal::ArrayRef (*rhs.array);
	},
	// destroy
	[](Literal & self) {
		if (--(self.array->owners) == 0) {
			delete self.array;
		}
	}
	
}, literal_virtual_table::type_array_index = {
	
	// op_eq
	[](const Literal & lhs, const Literal & rhs) {
		auto & arg = lhs.arr_index->array->data [lhs.arr_index->index];
		return arg.vTable->op_eq (arg, rhs);
	},
	// op_lt
	[](const Literal & lhs, const Literal & rhs) {
		auto & arg = lhs.arr_index->array->data [lhs.arr_index->index];
		return arg.vTable->op_lt (arg, rhs);
	},
	// op_lte
	[](const Literal & lhs, const Literal & rhs) {
		auto & arg = lhs.arr_index->array->data [lhs.arr_index->index];
		return arg.vTable->op_lte (arg, rhs);
	},
	// op_not
	[](const Literal & self) {
		auto & arg = self.arr_index->array->data [self.arr_index->index];
		return arg.vTable->op_not (arg);
	},
	
	// conv_double
	[](const Literal & self) {
		auto & arg = self.arr_index->array->data [self.arr_index->index];
		return arg.vTable->conv_double (arg);
	},
	// conv_int64
	[](const Literal & self) {
		auto & arg = self.arr_index->array->data [self.arr_index->index];
		return arg.vTable->conv_int64 (arg);
	},
	// conv_uint64
	[](const Literal & self) {
		auto & arg = self.arr_index->array->data [self.arr_index->index];
		return arg.vTable->conv_uint64 (arg);
	},
	// conv_aint
	[](const Literal & self) {
		auto & arg = self.arr_index->array->data [self.arr_index->index];
		return arg.vTable->conv_aint (arg);
	},
	
	// op_add
	[](const Literal & lhs, const Literal & rhs) {
		auto & arg = lhs.arr_index->array->data [lhs.arr_index->index];
		return arg.vTable->op_add (arg, rhs);
	},
	// op_band
	[](const Literal & lhs, const Literal & rhs) {
		auto & arg = lhs.arr_index->array->data [lhs.arr_index->index];
		return arg.vTable->op_band (arg, rhs);
	},
	// op_bor
	[](const Literal & lhs, const Literal & rhs) {
		auto & arg = lhs.arr_index->array->data [lhs.arr_index->index];
		return arg.vTable->op_bor (arg, rhs);
	},
	// op_bxor
	[](const Literal & lhs, const Literal & rhs) {
		auto & arg = lhs.arr_index->array->data [lhs.arr_index->index];
		return arg.vTable->op_bxor (arg, rhs);
	},
	// op_concat
	[](const Literal & lhs, const Literal & rhs) {
		auto & arg = lhs.arr_index->array->data [lhs.arr_index->index];
		return arg.vTable->op_concat (arg, rhs);
	},
	// op_div
	[](const Literal & lhs, const Literal & rhs) {
		auto & arg = lhs.arr_index->array->data [lhs.arr_index->index];
		return arg.vTable->op_div (arg, rhs);
	},
	// op_index
	[](const Literal & self, size_t i) {
		auto & arg = self.arr_index->array->data [self.arr_index->index];
		return arg.vTable->op_index (arg, i);
	},
	// op_mul
	[](const Literal & lhs, const Literal & rhs) {
		auto & arg = lhs.arr_index->array->data [lhs.arr_index->index];
		return arg.vTable->op_mul (arg, rhs);
	},
	// op_sub
	[](const Literal & lhs, const Literal & rhs) {
		auto & arg = lhs.arr_index->array->data [lhs.arr_index->index];
		return arg.vTable->op_sub (arg, rhs);
	},
	
	// copy_ctr
	[](Literal & lhs, const Literal & rhs) {
		lhs.vTable = rhs.vTable;
		lhs.arr_index = new Literal::ArrayIndex (*rhs.arr_index );
		++(lhs.arr_index->array->owners);
	},
	// destroy
	[](Literal & self) {
		if (--(self.arr_index->array->owners) == 0) {
			delete self.arr_index->array;
		}
		delete self.arr_index;
	}
	
}, literal_virtual_table::type_fint64 = {
	
	// op_eq
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint64 == rhs.vTable) {
			return lhs.fint64 == rhs.fint64;
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return lhs.fint64 == l.fint64;
			}
		}
	},
	// op_lt
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint64 == rhs.vTable) {
			return lhs.fint64 < rhs.fint64;
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return lhs.fint64 < l.fint64;
			}
		}
	},
	// op_lte
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint64 == rhs.vTable) {
			return lhs.fint64 <= rhs.fint64;
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return lhs.fint64 <= l.fint64;
			}
		}
	},
	// op_not
	[](const Literal & self) {return not self.fint64;},
	
	// conv_double
	[](const Literal & self) {return self;},
	// conv_int64
	[](const Literal & self) {return Literal (int64_t (self.fint64));},
	// conv_uint64
	[](const Literal & self) {return Literal (uint64_t (self.fint64));},
	// conv_aint
	[](const Literal & self) {return Literal (mpq_class (self.fint64));},
	
	// op_add
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint64 == rhs.vTable) {
			return Literal (lhs.fint64 + rhs.fint64);
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.fint64 + l.fint64);
			}
		}
	},
	// op_band
	[](const Literal &, const Literal &) {return Literal ();},
	// op_bor
	[](const Literal &, const Literal &) {return Literal ();},
	// op_bxor
	[](const Literal &, const Literal &) {return Literal ();},
	// op_concat
	[](const Literal &, const Literal &) {return Literal ();},
	// op_div
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint64 == rhs.vTable) {
			return Literal (lhs.fint64 / rhs.fint64);
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.fint64 / l.fint64);
			}
		}
	},
	// op_index
	[](const Literal &, size_t) {return Literal ();},
	// op_mul
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint64 == rhs.vTable) {
			return Literal (lhs.fint64 * rhs.fint64);
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.fint64 * l.fint64);
			}
		}
	},
	// op_sub
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint64 == rhs.vTable) {
			return Literal (lhs.fint64 - rhs.fint64);
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.fint64 - l.fint64);
			}
		}
	},
	
	// copy_ctr
	[](Literal & lhs, const Literal & rhs) {lhs.vTable = &type_fint64; lhs.fint64 = rhs.fint64;},
	// destroy
	[](Literal &) {}
	
}, literal_virtual_table::type_fint32 = {
	
	// op_eq
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint32 == rhs.vTable) {
			return lhs.fint32 == rhs.fint32;
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return lhs.fint32 == float (l.fint64);
			}
		}
	},
	// op_lt
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint32 == rhs.vTable) {
			return lhs.fint32 < rhs.fint32;
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return lhs.fint32 < float (l.fint64);
			}
		}
	},
	// op_lte
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint32 == rhs.vTable) {
			return lhs.fint32 <= rhs.fint32;
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return lhs.fint32 <= float (l.fint64);
			}
		}
	},
	// op_not
	[](const Literal & self) {return not self.fint32;},
	
	// conv_double
	[](const Literal & self) {return Literal (double (self.fint32));},
	// conv_int64
	[](const Literal & self) {return Literal (int64_t (self.fint32));},
	// conv_uint64
	[](const Literal & self) {return Literal (uint64_t (self.fint32));},
	// conv_aint
	[](const Literal & self) {return Literal (mpq_class (self.fint32));},
	
	// op_add
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint32 == rhs.vTable) {
			return Literal (lhs.fint32 + rhs.fint32);
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.fint32 + float (l.fint64));
			}
		}
	},
	// op_band
	[](const Literal &, const Literal &) {return Literal ();},
	// op_bor
	[](const Literal &, const Literal &) {return Literal ();},
	// op_bxor
	[](const Literal &, const Literal &) {return Literal ();},
	// op_concat
	[](const Literal &, const Literal &) {return Literal ();},
	// op_div
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint32 == rhs.vTable) {
			return Literal (lhs.fint32 / rhs.fint32);
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.fint32 / float (l.fint64));
			}
		}
	},
	// op_index
	[](const Literal &, size_t) {return Literal ();},
	// op_mul
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint32 == rhs.vTable) {
			return Literal (lhs.fint32 * rhs.fint32);
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.fint32 * float (l.fint64));
			}
		}
	},
	// op_sub
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_fint32 == rhs.vTable) {
			return Literal (lhs.fint32 - rhs.fint32);
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.fint32 - float (l.fint64));
			}
		}
	},
	
	// copy_ctr
	[](Literal & lhs, const Literal & rhs) {lhs.vTable = &type_fint32; lhs.fint64 = rhs.fint64;},
	// destroy
	[](Literal &) {}
	
}, literal_virtual_table::type_int64 = {
	
	// op_eq
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_int64 == rhs.vTable) {
			return lhs.int64 == rhs.int64;
		} else {
			Literal l = rhs.vTable->conv_int64 (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return lhs.int64 == l.int64;
			}
		}
	},
	// op_lt
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_int64 == rhs.vTable) {
			return lhs.int64 < rhs.int64;
		} else {
			Literal l = rhs.vTable->conv_int64 (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return lhs.int64 < l.int64;
			}
		}
	},
	// op_lte
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_int64 == rhs.vTable) {
			return lhs.int64 <= rhs.int64;
		} else {
			Literal l = rhs.vTable->conv_int64 (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return lhs.int64 <= l.int64;
			}
		}
	},
	// op_not
	[](const Literal & self) {return not self.int64;},
	
	// conv_double
	[](const Literal & self) {return Literal (double (self.int64));},
	// conv_int64
	[](const Literal & self) {return self;},
	// conv_uint64
	[](const Literal & self) {return Literal (uint64_t (self.int64));},
	// conv_aint
	[](const Literal & self) {return Literal (mpq_class (self.int64));},
	
	// op_add
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_int64 == rhs.vTable) {
			return Literal (lhs.int64 + rhs.int64);
		} else {
			Literal l = rhs.vTable->conv_int64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.int64 + l.int64);
			}
		}
	},
	// op_band
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_int64 == rhs.vTable) {
			return Literal (lhs.int64 & rhs.int64);
		} else {
			Literal l = rhs.vTable->conv_int64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.int64 & l.int64);
			}
		}
	},
	// op_bor
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_int64 == rhs.vTable) {
			return Literal (lhs.int64 | rhs.int64);
		} else {
			Literal l = rhs.vTable->conv_int64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.int64 | l.int64);
			}
		}
	},
	// op_bxor
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_int64 == rhs.vTable) {
			return Literal (lhs.int64 ^ rhs.int64);
		} else {
			Literal l = rhs.vTable->conv_int64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.int64 ^ l.int64);
			}
		}
	},
	// op_concat
	[](const Literal &, const Literal &) {return Literal ();},
	// op_div
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_int64 == rhs.vTable) {
			return Literal (lhs.int64 / rhs.int64);
		} else {
			Literal l = rhs.vTable->conv_int64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.int64 / l.int64);
			}
		}
	},
	// op_index
	[](const Literal &, size_t) {return Literal ();},
	// op_mul
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_int64 == rhs.vTable) {
			return Literal (lhs.int64 * rhs.int64);
		} else {
			Literal l = rhs.vTable->conv_int64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.int64 * l.int64);
			}
		}
	},
	// op_sub
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_int64 == rhs.vTable) {
			return Literal (lhs.int64 - rhs.int64);
		} else {
			Literal l = rhs.vTable->conv_int64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.int64 - l.int64);
			}
		}
	},
	
	// copy_ctr
	[](Literal & lhs, const Literal & rhs) {lhs.vTable = &type_int64; lhs.int64 = rhs.int64;},
	// destroy
	[](Literal &) {}
	
}, literal_virtual_table::type_aint0 = {
	
	// op_eq
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_aint0 == rhs.vTable) {
			return *lhs.aint0 == *rhs.aint0;
		} else {
			Literal l = rhs.vTable->conv_aint (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return *lhs.aint0 == *l.aint0;
			}
		}
	},
	// op_lt
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_aint0 == rhs.vTable) {
			return *lhs.aint0 < *rhs.aint0;
		} else {
			Literal l = rhs.vTable->conv_aint (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return *lhs.aint0 < *l.aint0;
			}
		}
	},
	// op_lte
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_aint0 == rhs.vTable) {
			return *lhs.aint0 <= *rhs.aint0;
		} else {
			Literal l = rhs.vTable->conv_aint (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return *lhs.aint0 <= *l.aint0;
			}
		}
	},
	// op_not
	[](const Literal & self) {return not *self.aint0;},
	
	// conv_double
	[](const Literal & self) {return Literal (self.aint0->get_d ());},
	// conv_int64
	[](const Literal & self) {return Literal (self.aint0->get_den ().get_si () / self.aint0->get_num ().get_si ());},
	// conv_uint64
	[](const Literal & self) {return Literal (self.aint0->get_den ().get_ui () / self.aint0->get_num ().get_ui ());},
	// conv_aint
	[](const Literal & self) {return self;},
	
	// op_add
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_aint0 == rhs.vTable) {
			return Literal (*lhs.aint0 + *rhs.aint0);
		} else {
			Literal l = rhs.vTable->conv_double (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (*lhs.aint0 + *l.aint0);
			}
		}
	},
	// op_band
	[](const Literal &, const Literal &) {return Literal ();},
	// op_bor
	[](const Literal &, const Literal &) {return Literal ();},
	// op_bxor
	[](const Literal &, const Literal &) {return Literal ();},
	// op_concat
	[](const Literal &, const Literal &) {return Literal ();},
	// op_div
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_aint0 == rhs.vTable) {
			return Literal (*lhs.aint0 / *rhs.aint0);
		} else {
			Literal l = rhs.vTable->conv_aint (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (*lhs.aint0 / *l.aint0);
			}
		}
	},
	// op_index
	[](const Literal &, size_t) {return Literal ();},
	// op_mul
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_aint0 == rhs.vTable) {
			return Literal (*lhs.aint0 * *rhs.aint0);
		} else {
			Literal l = rhs.vTable->conv_aint (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (*lhs.aint0 * *l.aint0);
			}
		}
	},
	// op_sub
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_aint0 == rhs.vTable) {
			return Literal (*lhs.aint0 - *rhs.aint0);
		} else {
			Literal l = rhs.vTable->conv_aint (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (*lhs.aint0 - *l.aint0);
			}
		}
	},
	
	// copy_ctr
	[](Literal & lhs, const Literal & rhs) {
		lhs.vTable = &type_aint0;
		lhs.aint0 = new mpq_class (*rhs.aint0);
	},
	// destroy
	[](Literal &) {}
	
}, literal_virtual_table::type_uint64 = {
	
	// op_eq
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_uint64 == rhs.vTable) {
			return lhs.uint64 == rhs.uint64;
		} else {
			Literal l = rhs.vTable->conv_uint64 (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return lhs.uint64 == l.uint64;
			}
		}
	},
	// op_lt
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_uint64 == rhs.vTable) {
			return lhs.uint64 < rhs.uint64;
		} else {
			Literal l = rhs.vTable->conv_uint64 (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return lhs.uint64 < l.uint64;
			}
		}
	},
	// op_lte
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_uint64 == rhs.vTable) {
			return lhs.uint64 <= rhs.uint64;
		} else {
			Literal l = rhs.vTable->conv_uint64 (rhs);
			if (l.isNil ()) {
				return false;
			} else {
				return lhs.uint64 <= l.uint64;
			}
		}
	},
	// op_not
	[](const Literal & self) {return not self.uint64;},
	
	// conv_double
	[](const Literal & self) {return Literal (double (self.uint64));},
	// conv_int64
	[](const Literal & self) {return Literal (int64_t (self.uint64));},
	// conv_uint64
	[](const Literal & self) {return self;},
	// conv_aint
	[](const Literal & self) {return Literal (mpq_class (self.uint64));},
	
	// op_add
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_uint64 == rhs.vTable) {
			return Literal (lhs.uint64 + rhs.uint64);
		} else {
			Literal l = rhs.vTable->conv_uint64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.uint64 + l.uint64);
			}
		}
	},
	// op_band
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_uint64 == rhs.vTable) {
			return Literal (lhs.uint64 & rhs.uint64);
		} else {
			Literal l = rhs.vTable->conv_uint64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.uint64 & l.uint64);
			}
		}
	},
	// op_bor
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_uint64 == rhs.vTable) {
			return Literal (lhs.uint64 | rhs.uint64);
		} else {
			Literal l = rhs.vTable->conv_uint64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.uint64 | l.uint64);
			}
		}
	},
	// op_bxor
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_uint64 == rhs.vTable) {
			return Literal (lhs.uint64 ^ rhs.uint64);
		} else {
			Literal l = rhs.vTable->conv_uint64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.uint64 ^ l.uint64);
			}
		}
	},
	// op_concat
	[](const Literal &, const Literal &) {return Literal ();},
	// op_div
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_uint64 == rhs.vTable) {
			return Literal (lhs.uint64 / rhs.uint64);
		} else {
			Literal l = rhs.vTable->conv_uint64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.uint64 / l.uint64);
			}
		}
	},
	// op_index
	[](const Literal &, size_t) {return Literal ();},
	// op_mul
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_uint64 == rhs.vTable) {
			return Literal (lhs.uint64 * rhs.uint64);
		} else {
			Literal l = rhs.vTable->conv_uint64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.uint64 * l.uint64);
			}
		}
	},
	// op_sub
	[](const Literal & lhs, const Literal & rhs) {
		if (&type_uint64 == rhs.vTable) {
			return Literal (lhs.uint64 - rhs.uint64);
		} else {
			Literal l = rhs.vTable->conv_uint64 (rhs);
			if (l.isNil ()) {
				return l;
			} else {
				return Literal (lhs.uint64 - l.uint64);
			}
		}
	},
	
	// copy_ctr
	[](Literal & lhs, const Literal & rhs) {lhs.vTable = &type_uint64; lhs.uint64 = rhs.uint64;},
	// destroy
	[](Literal &) {}
	
};


std::string Literal::toString (text::PrintMode mode) const {
	return "literal";
}

mpq_class Literal::to_aint0 (bool * success) const {
	auto l = vTable->conv_aint (*this);
	if (l.isNil ()) {
		if (success) *success = false;
		return 0;
	}
	return *l.aint0;
}

double Literal::to_double (bool * success) const {
	auto l = vTable->conv_double (*this);
	if (l.isNil ()) {
		if (success) *success = false;
		return 0;
	}
	return l.fint64;
}

int64_t Literal::to_int64 (bool * success) const {
	auto l = vTable->conv_int64 (*this);
	if (l.isNil ()) {
		if (success) *success = false;
		return 0;
	}
	return l.int64;
}

uint64_t Literal::to_uint64 (bool * success) const {
	auto l = vTable->conv_uint64 (*this);
	if (l.isNil ()) {
		if (success) *success = false;
		return 0;
	}
	return l.uint64;
}
