#pragma once


#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>


template <typename T>
struct BasicArray {
	
	public:
		
		bool empty () const {return !len;}
		size_t size () const {return len;}
		
		const BasicArray <T> & operator = (const BasicArray <T> & other) {
			if (data) delete [] data;
			len = other.len;
			data = new T [len];
			std::copy (other.data, other.data + len, data);
		}
		const BasicArray <T> & operator = (BasicArray <T> && other) {
			if (data) delete [] data;
			len = other.len;
			data = other.data;
			other.data = nullptr;
			other.len = 0;
		}
		
		T & operator [] (size_t index) {assert (index < len); return data [index];}
		const T & operator [] (size_t index) const {assert (index < len); return data [index];}
		
		template <typename Y>
		BasicArray <Y> map (const std::function <Y (const T&)> & f) const {
			BasicArray <Y> res (len);
			for (size_t i=0; i<len; ++i) {res [i] = f (data [i]);}
			return res;
		}
		
		template <typename Y>
		BasicArray <Y> map (const std::function <Y (T&)> & f) {
			BasicArray <Y> res (len);
			for (size_t i=0; i<len; ++i) {res [i] = f (data [i]);}
			return res;
		}
		
		BasicArray () : data (nullptr), len (0) {}
		BasicArray (size_t len) : BasicArray (len, T ()) {}
		BasicArray (size_t len, const T & d) : data (nullptr), len (len) {
			if (len) {
				data = new T [len];
				std::fill (data, data+len, d);
			}
		}
		BasicArray (std::initializer_list <T> init) : data (nullptr), len (init.size ()) {
			if (len) {
				data = new T [len];
				std::copy (init.begin (), init.end (), data);
			}
		}
		BasicArray (const BasicArray <T> & other) : data (nullptr), len (other.len) {
			if (len) {
				data = new T [len];
				std::copy (other.data, other.data + len, data);
			}
		}
		BasicArray (BasicArray <T> && other) : data (other.data), len (other.len) {other.data = nullptr; other.len = 0;}
		template <typename IT>
		BasicArray (IT beg, IT end) : data (nullptr), len (end-beg) {
			if (len) {
				data = new T [len];
				std::copy (beg, end, data);
			}
		}
		
		~BasicArray () {delete [] data; data=nullptr; len = 0;}
		
		struct iterator {
			typedef size_t difference_type;
			typedef T value_type;
			typedef T& reference;
			typedef T* pointer;
			typedef std::random_access_iterator_tag iterator_category;
			
			T & operator * () {return *ptr;}
			T * operator -> () const {return ptr;}
			bool operator != (iterator rhs) const {return ptr != rhs.ptr;}
			bool operator == (iterator rhs) const {return ptr == rhs.ptr;}
			bool operator < (iterator rhs) const {return ptr < rhs.ptr;}
			bool operator <= (iterator rhs) const {return ptr <= rhs.ptr;}
			bool operator > (iterator rhs) const {return ptr > rhs.ptr;}
			bool operator >= (iterator rhs) const {return ptr >= rhs.ptr;}
			
			iterator operator ++ () {++ptr; return *this;}
			iterator operator -- () {--ptr; return *this;}
			iterator operator + (ssize_t other) const {return ptr + other;}
			size_t operator - (iterator other) const {return ptr - other.ptr;}
			iterator operator - (ssize_t other) const {return ptr - other;}
			operator const void * () const {return ptr;}
			
			iterator () : ptr (nullptr) {}
			iterator (T * p) : ptr (p) {}
			T * ptr;
		};
		struct const_iterator {
			typedef size_t difference_type;
			typedef T value_type;
			typedef const T& reference;
			typedef const T* pointer;
			typedef std::random_access_iterator_tag iterator_category;
			
			const T & operator * () const {return *ptr;}
			const T * operator -> () const {return ptr;}
			bool operator != (const_iterator rhs) const {return ptr != rhs.ptr;}
			bool operator == (const_iterator rhs) const {return ptr == rhs.ptr;}
			bool operator < (const_iterator rhs) const {return ptr < rhs.ptr;}
			bool operator <= (const_iterator rhs) const {return ptr <= rhs.ptr;}
			bool operator > (const_iterator rhs) const {return ptr > rhs.ptr;}
			bool operator >= (const_iterator rhs) const {return ptr >= rhs.ptr;}
			
			const_iterator operator ++ () {++ptr; return *this;}
			const_iterator operator -- () {--ptr; return *this;}
			const_iterator operator + (ssize_t other) const {return ptr + other;}
			size_t operator - (const_iterator other) const {return ptr - other.ptr;}
			const_iterator operator - (ssize_t other) const {return ptr - other;}
			operator const void * () const {return ptr;}
			
			const_iterator () : ptr (nullptr) {}
			const_iterator (T * p) : ptr (p) {}
			const_iterator (iterator it) : ptr (it.ptr) {}
			T * ptr;
		};
		
		struct reverse_iterator {
			typedef size_t difference_type;
			typedef T value_type;
			typedef T& reference;
			typedef T* pointer;
			typedef std::random_access_iterator_tag iterator_category;
			
			T & operator * () {return *ptr;}
			T * operator -> () const {return ptr;}
			bool operator != (reverse_iterator rhs) const {return ptr != rhs.ptr;}
			bool operator == (reverse_iterator rhs) const {return ptr == rhs.ptr;}
			bool operator < (reverse_iterator rhs) const {return ptr < rhs.ptr;}
			bool operator <= (reverse_iterator rhs) const {return ptr <= rhs.ptr;}
			bool operator > (reverse_iterator rhs) const {return ptr > rhs.ptr;}
			bool operator >= (reverse_iterator rhs) const {return ptr >= rhs.ptr;}
			
			reverse_iterator operator ++ () {--ptr; return *this;}
			reverse_iterator operator -- () {--ptr; return *this;}
			reverse_iterator operator + (ssize_t other) const {return ptr - other;}
			size_t operator - (reverse_iterator other) const {return other.ptr - ptr;}
			reverse_iterator operator - (ssize_t other) const {return ptr + other;}
			operator const void * () const {return ptr;}
			
			reverse_iterator () : ptr (nullptr) {}
			reverse_iterator (T * p) : ptr (p) {}
			T * ptr;
		};
		struct reverse_const_iterator {
			typedef size_t difference_type;
			typedef T value_type;
			typedef const T& reference;
			typedef const T* pointer;
			typedef std::random_access_iterator_tag iterator_category;
			
			const T & operator * () const {return *ptr;}
			const T * operator -> () const {return ptr;}
			bool operator != (reverse_const_iterator rhs) const {return ptr != rhs.ptr;}
			bool operator == (reverse_const_iterator rhs) const {return ptr == rhs.ptr;}
			bool operator < (reverse_const_iterator rhs) const {return ptr < rhs.ptr;}
			bool operator <= (reverse_const_iterator rhs) const {return ptr <= rhs.ptr;}
			bool operator > (reverse_const_iterator rhs) const {return ptr > rhs.ptr;}
			bool operator >= (reverse_const_iterator rhs) const {return ptr >= rhs.ptr;}
			
			reverse_const_iterator operator ++ () {--ptr; return *this;}
			reverse_const_iterator operator -- () {--ptr; return *this;}
			reverse_const_iterator operator + (ssize_t other) const {return ptr - other;}
			size_t operator - (reverse_const_iterator other) const {return other.ptr - ptr;}
			reverse_const_iterator operator - (ssize_t other) const {return ptr + other;}
			operator const void * () const {return ptr;}
			
			reverse_const_iterator () : ptr (nullptr) {}
			reverse_const_iterator (T * p) : ptr (p) {}
			reverse_const_iterator (reverse_iterator it) : ptr (it.ptr) {}
			T * ptr;
		};
		
		iterator begin () {return data;}
		const const_iterator begin () const {return data;}
		iterator end () {return data + len;}
		const const_iterator end () const {return data + len;}
		
		reverse_iterator rbegin () {return data + len - 1;}
		const reverse_const_iterator rbegin () const {return data + len - 1;}
		reverse_iterator rend () {return data - 1;}
		const reverse_const_iterator rend () const {return data - 1;}
		
		T & front () {return *data;}
		const T & front () const {return *data;}
		T & back () {return *(data + len-1);}
		const T & back () const {return *(data + len-1);}
		
		void clear () {
			if (len) {
				delete [] data;
				data = nullptr;
				len = 0;
			}
		}
		
	protected:
		
		T * data;
		size_t len;
		
	private:
		
};


template <typename T, size_t GROWTH=16>
struct DynArray : public BasicArray <T> {
	
	public:
		
		const DynArray <T> & operator = (const DynArray <T> & other) {
			BasicArray <T>::operator = (other);
			capacity = this->size ();
			return *this;
		}
		const DynArray <T> & operator = (DynArray <T> && other) {
			BasicArray <T>::operator = (other);
			capacity = this->size ();
			return *this;
		}
		
		template <typename IT>
		typename BasicArray <T>::const_iterator insert (typename BasicArray <T>::const_iterator pos, const IT in_start, const IT in_end) {
			size_t extraSpace = in_end - in_start;
			if (this->size () + extraSpace >= capacity) {
				size_t offset = pos - this->data;
				resize (this->size () + extraSpace);
				pos = this->data + offset;
			}
			typename BasicArray <T>::const_iterator end (this->end ());
			assert (capacity >= this->size () + extraSpace);
			typename BasicArray <T>::iterator itt = (pos.ptr);
			if (pos != end) {
				std::copy (pos, end, itt + extraSpace);
			}
			std::copy (in_start, in_end, itt);
			this->len += extraSpace;
			return pos;
		}
		
		void push_back (const T & d) {
			if (this->size () == capacity) {
				resize (capacity + 1);
			}
			this->data [this->len] = d;
			this->len += 1;
		}
		
		DynArray () : BasicArray <T> (), capacity (this->size ()) {}
		DynArray (size_t len) : DynArray <T, GROWTH> (len, T ()) {}
		DynArray (size_t len, const T & d) : BasicArray <T> (len, d), capacity (this->size ()) {}
		DynArray (std::initializer_list <T> init) : BasicArray <T> (init), capacity (this->size ()) {}
		DynArray (const BasicArray <T> & other) : BasicArray <T> (other), capacity (this->size ()) {}
		DynArray (BasicArray <T> && other) : BasicArray <T> (other), capacity (this->size ()) {}
		DynArray (const DynArray <T> & other) : BasicArray <T> (other), capacity (this->size ()) {}
		DynArray (DynArray <T> && other) : BasicArray <T> (other), capacity (this->size ()) {}
		template <typename IT>
		DynArray (IT beg, IT end) : BasicArray <T> (beg, end), capacity (this->size ()) {}
		
	protected:
	private:
		
		void resize (size_t min) {
			capacity = min + (GROWTH - min % GROWTH);
			T * ptr = new T [capacity];
			std::copy (this->data, this->data + this->len, ptr);
			delete [] this->data;
			this->data = ptr;
		}
		
		size_t capacity;
		
};
