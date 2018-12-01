#include <stackmachine/instruction.hpp>


namespace stackmachine {


Literal Layout::dataToLiteral (const uint8_t * data) const {
	
	switch (type) {
		case TYPE::f16:
		case TYPE::f32:
			return *reinterpret_cast <const float*> (data);
		case TYPE::f64:
			return *reinterpret_cast <const double*> (data);
		case TYPE::i8:
			return *reinterpret_cast <const int8_t*> (data);
		case TYPE::i16:
			return *reinterpret_cast <const int16_t*> (data);
		case TYPE::i32:
			return *reinterpret_cast <const int32_t*> (data);
		case TYPE::i64:
			return *reinterpret_cast <const int64_t*> (data);
		case TYPE::isize:
			return *reinterpret_cast <const ssize_t*> (data);
		case TYPE::u8:
			return *reinterpret_cast <const uint8_t*> (data);
		case TYPE::u16:
			return *reinterpret_cast <const uint16_t*> (data);
		case TYPE::u32:
			return *reinterpret_cast <const uint32_t*> (data);
		case TYPE::u64:
			return *reinterpret_cast <const uint64_t*> (data);
		case TYPE::usize:
			return *reinterpret_cast <const size_t*> (data);
			
		case TYPE::_META_ARRAY_OF: {
			BasicArray <Literal> members (components.size ());
			for (size_t i=0, l=2; i<l; ++i) {
				members [i] = components [i].dataToLiteral (data + offsets [i]);
			}
			return members;
		}
		
		case TYPE::_META_FUNC_SPLIT:
		case TYPE::_META_STRUCT_BEGIN:
		case TYPE::_META_STRUCT_END: {
			BasicArray <Literal> members (components.size ());
			for (size_t i=0, l=components.size (); i<l; ++i) {
				members [i] = components [i].dataToLiteral (data + offsets [i]);
			}
			return members;
		}
	}
	
	return Literal ();
	
}

size_t Layout::literalToData (const Literal & literal, uint8_t * data, bool init) const {
	
	switch (type) {
		case TYPE::f16:
		case TYPE::f32:
			*reinterpret_cast <float*> (data) = literal.to_double ();
			break;
		case TYPE::f64:
			*reinterpret_cast <double*> (data) = literal.to_double ();
			break;
		case TYPE::i8:
			*reinterpret_cast <int8_t*> (data) = literal.to_int64 ();
			break;
		case TYPE::i16:
			*reinterpret_cast <int16_t*> (data) = literal.to_int64 ();
			break;
		case TYPE::i32:
			*reinterpret_cast <int32_t*> (data) = literal.to_int64 ();
			break;
		case TYPE::i64:
			*reinterpret_cast <int64_t*> (data) = literal.to_int64 ();
			break;
		case TYPE::isize:
			*reinterpret_cast <ssize_t*> (data) = literal.to_int64 ();
			break;
		case TYPE::u8:
			*reinterpret_cast <uint8_t*> (data) = literal.to_uint64 ();
			break;
		case TYPE::u16:
			*reinterpret_cast <uint16_t*> (data) = literal.to_uint64 ();
			break;
		case TYPE::u32:
			*reinterpret_cast <uint32_t*> (data) = literal.to_uint64 ();
			break;
		case TYPE::u64:
			*reinterpret_cast <uint64_t*> (data) = literal.to_uint64 ();
			break;
		case TYPE::usize:
			*reinterpret_cast <size_t*> (data) = literal.to_uint64 ();
			break;
			
		case TYPE::_META_ARRAY_OF:
			
			if (init) {
				
				*reinterpret_cast <uint8_t**> (data) = new uint8_t [literal.length () * components [2].getSize ()];
				*reinterpret_cast <size_t*> (data + sizeof (size_t)) = literal.length ();
				
			} else {
				for (size_t i=0, l=2; i<l; ++i) {
					components [i].literalToData (literal [i], data + offsets [i]);
				}
			}
			
			break;
			
		case TYPE::_META_FUNC_SPLIT:
		case TYPE::_META_STRUCT_BEGIN:
		case TYPE::_META_STRUCT_END: 
			for (size_t i=0, l=components.size (); i<l; ++i) {
				components [i].literalToData (literal [i], data + offsets [i]);
			}
			break;
			
	}
	return size;
	
}


Layout::Layout (const TYPE type) : type (type) {
	
	switch (type) {
		case TYPE::i8:
			size = alignment = 1;
			break;
		case TYPE::i16:
			size = alignment = 2;
			break;
		case TYPE::i32:
			size = alignment = 4;
			break;
		case TYPE::i64:
			size = alignment = 8;
			break;
		case TYPE::isize:
			size = alignment = sizeof (size_t);
			break;
		case TYPE::f16:
		case TYPE::f32:
			size = alignment = 4;
			break;
		case TYPE::f64:
			size = alignment = 8;
			break;
		case TYPE::u8:
			size = alignment = 1;
			break;
		case TYPE::u16:
			size = alignment = 2;
			break;
		case TYPE::u32:
			size = alignment = 4;
			break;
		case TYPE::u64:
			size = alignment = 8;
			break;
		case TYPE::usize:
			size = alignment = sizeof (size_t);
			break;
			
		case TYPE::_META_ARRAY_OF:
		case TYPE::_META_FUNC_SPLIT:
		case TYPE::_META_STRUCT_BEGIN:
		case TYPE::_META_STRUCT_END:
			break;
			
	}
	
}

Layout::Layout (BasicArray <TYPE>::const_iterator * beg, const BasicArray <TYPE>::const_iterator end) {
	
	if (*beg != end ) {
		
		if (**beg == TYPE::_META_STRUCT_BEGIN) {
			
			++*beg;
			DynArray <Layout> m;
			DynArray <uint32_t> off;
			off.push_back (0);
			
			while (*beg != end and **beg != TYPE::_META_STRUCT_END) {
				
				Layout layout (beg, end);
				m.push_back (layout);
				off.back () = off.back () % layout.alignment ? layout.alignment - off.back () % layout.alignment : 0;
				size += layout.size;
				off.push_back (size);
				alignment = std::max (alignment, layout.alignment);
				++*beg;
				
			}
			
			components = m;
			offsets = off;
			type = TYPE::_META_STRUCT_BEGIN;
			
		} else if (**beg == TYPE::_META_ARRAY_OF) {
			
			++*beg;
			DynArray <Layout> m;
			DynArray <uint32_t> off;
			off.push_back (0);
			
			m.push_back (Layout (TYPE::usize));
			size += m.back ().size;
			off.push_back (size);
			
			m.push_back (Layout (TYPE::usize));
			size += m.back ().size;
			off.push_back (size);
			
			Layout layout (beg, end);
			m.push_back (layout);
			off.push_back (0);
			components = m;
			offsets = off;
			type = TYPE::_META_ARRAY_OF;
			
		} else {
			
			*this = Layout (**beg);
			
		}
	
	}
	
}


}
