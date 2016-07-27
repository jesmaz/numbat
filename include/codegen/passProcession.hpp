#pragma once


#include <codegen/genericPass.hpp>
#include <memory>
#include <nir/forward.hpp>
#include <type_traits>


namespace codegen {


class PassProcession {
	
	public:
		
		template <typename T>
		typename std::enable_if <std::is_base_of <GenericPass, T>::value, T*>::type runPass (nir::Module * module) {
			std::shared_ptr <GenericPass> & pass = passes [typeid (T).name ()];
			if (not pass) {
				pass = std::shared_ptr <GenericPass> (new T (this, module));
			}
			return reinterpret_cast <T*> (pass.get ());
		}
		
	protected:
	private:
		
		std::map <const char *, std::shared_ptr <GenericPass>> passes;
		
};


}